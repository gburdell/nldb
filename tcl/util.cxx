//The MIT License
//
//nldb - c++ library to bind tcl stuff
//Copyright (c) 2006-2010  Karl W. Pfalzer
//Copyright (c) 2012       George P. Burdell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
#include <cstdarg>
#include <sstream>
#include <vector>
#include "tcl.h"
#include "tclDecls.h"
#include "message.hxx"
#include "xyzzy/assert.hxx"
#include "util.hxx"
#include "tclInt.h"

namespace nldb {
    using namespace std;

    Jni *Jni::stpTheOne = 0;
    jmethodID *JObject::stpMids = 0;

    JObject::JObject(const jobject obj) {
        init(obj);
    }

    JObject::JObject(const JObject &ref) {
        init(ref.m_obj);
    }

    JObject& JObject::operator=(const JObject &ref) {
        init(ref.m_obj);
        return *this;
    }

    JObject::~JObject() {
        Jni::getTheOne()->getEnv()->DeleteGlobalRef(m_obj);
    }

    //We'll use define to make jmethodID use readable
    //JL_ for java/lang
#define JL_Object_hashCode 0

    void
    JObject::init(const jobject obj) {
        if (0 == stpMids) {
            stpMids = new jmethodID[1];
            stpMids[JL_Object_hashCode] = getClassMethodId("java/lang/Object", "hashCode", "()I");
        }
        m_obj = Jni::getTheOne()->getEnv()->NewGlobalRef(obj);
    }

    string
    JObject::objAsString() const {
        JNIEnv *penv = Jni::getTheOne()->getEnv();
        jint hash = penv->CallIntMethod(m_obj, stpMids[JL_Object_hashCode]);
        Jni::getTheOne()->checkForExceptions();
        ostringstream oss;
        oss << "0x" << ios::hex << hash;
        return oss.str();
    }

    char* strdup(const string s, int *pn) {
        int n = s.length();
        char *buf = new char[n + 1];
        strcpy(buf, s.c_str());
        if (0 != pn) {
            *pn = n;
        }
        return buf;
    }

    char*
    JObject::objAsString(int &n) const {
        string s = objAsString();
        char *buf = strdup(s, &n);
        n++; //add nul
        //TODO: does tcl free this space?
        return buf;
    }

#undef JL_Object_hashCode

    TclError::TclError()
    : m_type(eMsgDone) {
    }

    TclError::TclError(Tcl_Interp *interp, string msg)
    : m_type(eRawMsg) {
        Message::getTheOne().error(interp, msg);
    }

    TclError::TclError(Tcl_Interp *interp, string code,
            string s1, string s2, string s3, string s4)
    : m_type(eFmtMsg) {
        Message::getTheOne().error(interp, code, s1, s2, s3, s4);
    }

    Jni::Jni() {
        TRcStringAr args = new TStringAr(2);
        args[0] = "-Djava.class.path=../dist/nldb.jar";
        args[1] = "-Xmx8g";
        init(args);
    }

    Jni::Jni(const char *penv, char delimiter) {
        TRcStringAr args;
        if (0 != penv) {
            vector<string> opts;
            stringstream ss(penv);
            string buf;
            while (getline(ss, buf, delimiter)) {
                opts.push_back(buf);
            }
            args = new TStringAr(opts);
        }
        init(args);
    }

    Jni::Jni(TRcStringAr args) {
        init(args);
    }

    void
    Jni::init(TRcStringAr args) {
        m_hadException = false;
        ASSERT_TRUE(0 == stpTheOne);
        stpTheOne = this;
        mp_env = 0;
        mp_jvm = 0;
        const unsigned nopts = args.isValid() ? args.length() : 0;
        m_vmArgs.nOptions = nopts;
        mp_vmOpts = new JavaVMOption[nopts];
        for (unsigned i = 0; i < nopts; i++) {
            mp_vmOpts[i].optionString = strdup(args[i]);
        }
        m_vmArgs.version = JNI_VERSION_1_6;
        m_vmArgs.options = mp_vmOpts;
        m_vmArgs.ignoreUnrecognized = JNI_TRUE;
        /* Create the Java VM */
        jint ok = JNI_CreateJavaVM(&mp_jvm, (void**) &mp_env, &m_vmArgs);
        ASSERT_TRUE(0 == ok);
    }

    jclass
    Jni::findClass(string clsNm, bool glbl) {
        jclass cls;
        cls = getEnv()->FindClass(clsNm.c_str());
        ASSERT_TRUE(NULL != cls);
        if (glbl) {
            cls = reinterpret_cast<jclass> (getEnv()->NewGlobalRef(cls));
        }
        return cls;
    }

    jmethodID
    Jni::getStaticMethodId(string clsNm, string methodNm, string sig) {
        jclass cls = findClass(clsNm.c_str(), false);
        jmethodID mid = getEnv()->GetStaticMethodID(cls, methodNm.c_str(),
                sig.c_str());
        ASSERT_TRUE(NULL != mid);
        return mid;
    }

    jmethodID
    Jni::getClassMethodId(string clsNm, string methodNm, string sig) {
        jclass cls = findClass(clsNm.c_str(), false);
        jmethodID mid = getEnv()->GetMethodID(cls, methodNm.c_str(), sig.c_str());
        ASSERT_TRUE(NULL != mid);
        return mid;
    }

    bool
    Jni::checkForExceptions() {
        if (JNI_FALSE != getEnv()->ExceptionCheck()) {
            getEnv()->ExceptionDescribe();
            ASSERT_NEVER;
        }
    }

    jobjectArray
    Jni::getNewJObjectArray(int sz) {
        static jclass stObjArCls = 0;
        if (0 == stObjArCls) {
            stObjArCls = findClass("java/lang/Object");
        }
        jobjectArray jar = getEnv()->NewObjectArray(sz, stObjArCls, NULL);
        ASSERT_TRUE(NULL != jar);
        return jar;
    }

    Jni::~Jni() {
        if (0 != mp_jvm) mp_jvm->DestroyJavaVM();
        delete mp_env;
        delete mp_jvm;
        delete mp_vmOpts;
    }

    TRcStringAr listAsStringAr(Tcl_Interp *interp, Tcl_Obj *lobj)
    throw (TclError) {
        TRcStringAr rval;
        int cnt;
        Tcl_Obj **peles;
        if (TCL_ERROR == Tcl_ListObjGetElements(interp, lobj, &cnt, &peles)) {
            throw TclError();
        }
        if (0 < cnt) {
            rval = new TStringAr(cnt);
            string s;
            for (int i = 0; i < cnt; i++) {
                s = to_s(peles[i]);
                rval[i] = s;
            }
        }
        return rval;
    }

    int to_i(Tcl_Interp *interp, Tcl_Obj CONST* obj) throw (TclError) {
        int i;
        if (TCL_ERROR == Tcl_GetIntFromObj(interp, const_cast<Tcl_Obj*> (obj), &i)) {
            throw TclError();
        }
        return i;
    }

    double to_double(Tcl_Interp *interp, Tcl_Obj CONST* obj) throw (TclError) {
        double d;
        if (TCL_ERROR == Tcl_GetDoubleFromObj(interp, const_cast<Tcl_Obj*> (obj), &d)) {
            throw TclError();
        }
        return d;
    }

    string to_s(int i) {
        static char buf[64];
        sprintf(buf, "%d", i);
        string r = buf;
        return r;
    }

    Tcl_Obj* newStringObj(bool b) {
        string bs = b ? "true" : "false";
        return newStringObj(bs);
    }

    Tcl_Obj* newDoubleObj(const char *pDouble) {
        double dbl;
        sscanf(pDouble, "%lf", &dbl);
        return Tcl_NewDoubleObj(dbl);
    }

    /**
     * Get jobjectArray version of Tcl_Obj[].
     * @param argc number of items in argv.
     * @param argv items to convert to jobject.
     * @return array.
     */
    jobjectArray toJObjectArray(const int argc, Tcl_Obj *CONST argv[]) {
        //cache up the tcl types, so we can compare pointers instead of strings

        enum EType {
            eBool = 0, eDouble, eInt, eString, eCmdName, eLast
        };
        static Tcl_ObjType **stTypePtrs = 0;
        if (0 == stTypePtrs) {
            typedef Tcl_ObjType* P;
            stTypePtrs = new P[eLast];
            stTypePtrs[eBool] = Tcl_NewBooleanObj(0)->typePtr;
            stTypePtrs[eDouble] = Tcl_NewDoubleObj(0)->typePtr;
            stTypePtrs[eInt] = Tcl_NewIntObj(0)->typePtr;
            stTypePtrs[eString] = 0; //dont get from: Tcl_NewStringObj("", -1)->typePtr;
            stTypePtrs[eCmdName] = 0; //TODO: how to get one?
        }
        jobjectArray jar = getNewJObjectArray(argc);
        JNIEnv *penv = Jni::getTheOne()->getEnv();
        for (int i = 0; i < argc; i++) {
            const Tcl_Obj *pitem = argv[i];
            jobject jobj = 0;
            if (pitem->typePtr == stTypePtrs[eBool]) {
                jobj = toJBooleanObj(pitem);
            } else if (pitem->typePtr == stTypePtrs[eDouble]) {
                jobj = toJDoubleObj(pitem);
            } else if (pitem->typePtr == stTypePtrs[eInt]) {
                jobj = toJIntObj(pitem);
            } else if ((pitem->typePtr == stTypePtrs[eString]) ||
                    (pitem->typePtr == stTypePtrs[eCmdName])) {
                jobj = toJStringObj(pitem);
            } else if (pitem->typePtr == &NldbshTclObjType) {
                const JObject *pjobj = reinterpret_cast<const JObject*> (pitem->internalRep.otherValuePtr);
                jobj = pjobj->asObject();
            } else {
                string s = pitem->typePtr->name;
                if ((0 == stTypePtrs[eCmdName]) && (s == "cmdName")) {
                    stTypePtrs[eCmdName] = pitem->typePtr;
                    jobj = toJStringObj(pitem);
                } else if ((0 == stTypePtrs[eString]) && (s == "string")) {
                    stTypePtrs[eString] = pitem->typePtr;
                    jobj = toJStringObj(pitem);
                } else {
                    ASSERT_NEVER;
                }
            }
            penv->SetObjectArrayElement(jar, i, jobj);
            checkForExceptions();
            penv->DeleteLocalRef(jobj);
            checkForExceptions();
        }
        return jar;
    }

    jobject toJBooleanObj(const Tcl_Obj *obj) throw (TclError) {
        static jmethodID stMid = 0;
        static jclass stClazz = 0;
        static const string stClsNm = "java/lang/Boolean";
        if (0 == stMid) {
            stClazz = findClass(stClsNm);
            stMid = getClassMethodId(stClsNm, "<init>", "(Z)V");
        }
        int b;
        if (TCL_ERROR == Tcl_GetBooleanFromObj(getTclInterp(), const_cast<Tcl_Obj*> (obj), &b)) {
            throw TclError();
        }
        jboolean jb = (0 == b) ? JNI_FALSE : JNI_TRUE;
        jobject jobj = Jni::getTheOne()->getEnv()->NewObject(stClazz, stMid, jb);
        checkForExceptions();
        return jobj;
    }

    jobject toJIntObj(const Tcl_Obj *obj) throw (TclError) {
        static jmethodID stMid = 0;
        static jclass stClazz = 0;
        static const string stClsNm = "java/lang/Integer";
        if (0 == stMid) {
            stClazz = findClass(stClsNm);
            stMid = getClassMethodId(stClsNm, "<init>", "(I)V");
        }
        jint i = to_i(getTclInterp(), obj);
        jobject jobj = Jni::getTheOne()->getEnv()->NewObject(stClazz, stMid, i);
        checkForExceptions();
        return jobj;
    }

    jobject toJDoubleObj(const Tcl_Obj *obj) throw (TclError) {
        static jmethodID stMid = 0;
        static jclass stClazz = 0;
        static const string stClsNm = "java/lang/Double";
        if (0 == stMid) {
            stClazz = findClass(stClsNm);
            stMid = getClassMethodId(stClsNm, "<init>", "(D)V");
        }
        jdouble d = to_double(getTclInterp(), obj);
        jobject jobj = Jni::getTheOne()->getEnv()->NewObject(stClazz, stMid, d);
        checkForExceptions();
        return jobj;
    }

    jobject toJStringObj(const Tcl_Obj *obj) throw (TclError) {
        string s = to_s(obj);
        jstring js = Jni::getTheOne()->getEnv()->NewStringUTF(s.c_str());
        checkForExceptions();
        return js;
    }
}

