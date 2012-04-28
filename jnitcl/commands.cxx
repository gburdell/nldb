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
#include <map>
#include <cstring>
#include <sstream>
#include <tcl.h>
#include <tclDecls.h>
#include "xyzzy/assert.hxx"
#include "xyzzy/array.hxx"
#include "message.hxx"
#include "commands.hxx"

namespace nldb {

    using xyzzy::PTArray;
    using xyzzy::PTRcArray;
    using nldb::Message;
    using namespace std;

    typedef PTArray<string> TStringAr;
    typedef PTRcArray<string> TRcStringAr;

    string Commands::stPeerClsNm = "nldb/jni/TclBridge";

    namespace config {

        static bool toBool(Tcl_Interp *interp, const char *varnm) {
            const char *v = Tcl_GetVar(interp, varnm, TCL_GLOBAL_ONLY);
            return (string(v) == "true");
        }

        static int toInt(Tcl_Interp *interp, const char *varnm) {
            const char *v = Tcl_GetVar(interp, varnm, TCL_GLOBAL_ONLY);
            int rv;
            sscanf(v, "%d", &rv);
            return rv;
        }

#ifdef NOT_YET
#define WRITER_CONFIG(_nm, _type) \
        Writer::stConfig.m_##_nm = to##_type (interp, "config::vlogo("#_nm")")

        static void getConfigSettings(Tcl_Interp *interp) {
            //Transfer Tcl to here
            WRITER_CONFIG(ansiPortDecl, Bool);
            WRITER_CONFIG(compressBusConnExpr, Int);
            WRITER_CONFIG(instDeclBusConnIndent, Int);
            WRITER_CONFIG(instDeclIndent, Int);
            WRITER_CONFIG(instTopological, Bool);
            WRITER_CONFIG(portDeclIndent, Int);
            WRITER_CONFIG(topModuleLast, Bool);
            WRITER_CONFIG(wireDefnIndent, Int);
        }

#undef WRITER_CONFIG
#endif //NOT_YET
    }; //namespace config

    static inline bool isEq(const Tcl_Obj *obj, string s) {
        return to_s(obj) == s;
    }

    //extern

    Tcl_Interp*
    getTclInterp() {
        return Commands::getTheOne().getInterp();
    }

    Commands Commands::stTheOne;

    Commands::Commands()
    : mp_objType(0), mp_interp(0), mp_jni(0) {
    }

    Commands::~Commands() {
        delete mp_jni;
    }

    void Commands::info(string code, string s1, string s2, string s3, string s4) {
        Message::getTheOne().message(Message::eInfo, 0, code, s1, s2, s3, s4);
    }

    void Commands::warn(string code, string s1, string s2, string s3, string s4) {
        Message::getTheOne().message(Message::eWarn, 0, code, s1, s2, s3, s4);
    }

    void Commands::error(string code, string s1, string s2, string s3, string s4) {
        Message::getTheOne().message(Message::eError, 0, code, s1, s2, s3, s4);
    }

    Tcl_Obj*
    Commands::dispatch(Tcl_Interp *interp, const int argc, Tcl_Obj *CONST argv[])
    throw (TclError) {
        if ((3 == argc) && ("_is_object" == to_s(argv[1]))) {
            bool isObj = (&NldbshTclObjType == argv[2]->typePtr);
            return Tcl_NewIntObj(isObj ? 1 : 0);
        }
        static jclass stCls = 0;
        static jmethodID stMid = 0;
        if (0 == stMid) {
            static const string sig = "([Ljava/lang/Object;)Ljava/lang/Object;";
            stCls = findClass(stPeerClsNm);
            stMid = getStaticMethodId(stPeerClsNm, "execute", sig);
        }
        mp_interp = interp;
        jobjectArray jar = toJObjectArray(argc - 1, &argv[1]);
        jobject rval = Jni::getTheOne()->getEnv()->CallStaticObjectMethod(stCls, stMid, jar);
        checkForExceptions();
        Tcl_Obj *pobj = convertPeerObj(rval);
        return pobj;
    }

#define TclBridge_asBool  0
#define TclBridge_asInt   1
#define TclBridge_asDbl   2
#define TclBridge_asStr   3
#define TclBridge_getType 4

    Tcl_Obj* Commands::convertPeerObj(const jobject obj) {
        //TODO: keep in sync w/ cXXX types of nldb.jni.TclBridge$Rval

        enum EType {
            eBool = 1, eDouble, eInt, eObject, eString
        };
        Tcl_Obj *tclObj = 0;
        Tcl_Interp *interp = Commands::getTheOne().getInterp();
        static jclass stCls = 0;
        static jmethodID *stMids = 0;
        if (0 == stMids) {
            stMids = new jmethodID[5];
            static const string sig = "(Ljava/lang/Object;)I";
            static const string clsnm = stPeerClsNm + "$Rval";
            stMids[TclBridge_asBool] = getClassMethodId(clsnm, "asBool", "()Z");
            stMids[TclBridge_asInt] = getClassMethodId(clsnm, "asInt", "()I");
            stMids[TclBridge_asDbl] = getClassMethodId(clsnm, "asDouble", "()D");
            stMids[TclBridge_asStr] = getClassMethodId(clsnm, "asString", "()Ljava/lang/String;");
            stMids[TclBridge_getType] = getClassMethodId(clsnm, "getType", "()I");
        }
        JNIEnv *penv = Jni::getTheOne()->getEnv();
        jint itype = penv->CallIntMethod(obj, stMids[TclBridge_getType]);
        checkForExceptions();
        switch (itype) {
            case eBool:
            {
                jboolean jbool = penv->CallBooleanMethod(obj, stMids[TclBridge_asBool]);
                checkForExceptions();
#ifdef NLSH_USES_BOOL
                bool b = (jbool == JNI_TRUE);
                tclObj = Tcl_NewBooleanObj(b);
#else
                int b = (jbool == JNI_TRUE) ? 1 : 0;
                tclObj = Tcl_NewIntObj(b);
#endif
            }
                break;
            case eDouble:
            {
                jdouble jdbl = penv->CallDoubleMethod(obj, stMids[TclBridge_asDbl]);
                checkForExceptions();
                tclObj = Tcl_NewDoubleObj(jdbl);
            }
                break;
            case eInt:
            {
                jint ji = penv->CallIntMethod(obj, stMids[TclBridge_asInt]);
                checkForExceptions();
                tclObj = Tcl_NewIntObj(ji);
            }
                break;
            case eObject:
                tclObj = Commands::getTheOne().createTclObj(obj);
                break;
            case eString:
            {
                jstring jstr = (jstring) penv->CallObjectMethod(obj, stMids[TclBridge_asStr]);
                checkForExceptions();
                const char *p = penv->GetStringUTFChars(jstr, NULL);
                tclObj = Tcl_NewStringObj(p, -1); //creates copy
                penv->ReleaseStringUTFChars(jstr, p);
            }
                break;
            default:
                ASSERT_NEVER;
        }
        ASSERT_TRUE(0 != tclObj);
        return tclObj;
    }

    Tcl_Obj*
    Commands::createTclObj(jobject obj) const {
        Tcl_Obj *pobj = (Tcl_Obj*) ckalloc(sizeof (Tcl_Obj));
        JObject *prox = new JObject(obj);
        pobj->refCount = 0;
        pobj->typePtr = const_cast<Tcl_ObjType*> (mp_objType);
        pobj->internalRep.otherValuePtr = prox;
        char *p = prox->objAsString(pobj->length);
        pobj->bytes = p;
        return pobj;
    }

    Commands&
    Commands::setObjType(const Tcl_ObjType &r) {
        ASSERT_TRUE(0 == mp_objType);
        mp_objType = &r;
        return *this;
    }

    JObject*
    Commands::getObject(Tcl_Obj *p) const throw (TclError) {
        if (mp_objType != p->typePtr) {
            throw TclError(getInterp(), "OBJ-2", to_s(p));
        }
        JObject *prox = reinterpret_cast<JObject*> (p->internalRep.otherValuePtr);
        return prox;
    }
}
