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
#ifndef NLDB_UTIL_HXX
#define	NLDB_UTIL_HXX

#include <string>
#include <jni.h>
#include "tcl.h"
#include "xyzzy/array.hxx"

//The value is defined in tclbridge.cxx
extern Tcl_ObjType NldbshTclObjType;

//Copy constructor definitions.  
//These are usu. placed in private to inhibit copy constructors.
#define DECL_COPY_CONSTRUCTORS(cls)          \
        cls(const cls&);                     \
        cls& operator=(const cls&)


namespace nldb {
    //Defined in commands.cxx
    extern Tcl_Interp* getTclInterp();
    /*TODO: getTheInterp() is here to eliminate need for Jni stuff to know
     *about intricacies of tcl side.  Should (TODO) eliminate need for other
     *methods to require in args, and just use this getTclInterp.
     */

    using std::string;
    using xyzzy::PTArray;
    using xyzzy::PTRcArray;

    typedef PTArray<string> TStringAr;
    typedef PTRcArray<string> TRcStringAr;

    // Proxy (design pattern) object to actual java-side Object
    // nldb.jni.TclBridge$Rval.
    class JObject {
    public:
        explicit JObject(const jobject obj);

        explicit JObject(const JObject &ref);

        JObject& operator=(const JObject &ref);

        jobject asObject() {
            return m_obj;
        }

        const jobject asObject() const {
            return m_obj;
        }

        /**
         * Get string representation: object/address; unique/readable
         * but not directly usable (other than opaque representation).
         * Storage for string is allocated here, so rcvr needs to free.
         * @param n reference to (returned) length of string.
         * @return object as text.
         */
        char* objAsString(int &n) const;

        /**
         * Same as above, but no extra allocation done.
         * @return string representation.
         */
        string objAsString() const;

        virtual ~JObject();

    private:
        void init(const jobject obj);

        static jmethodID *stpMids;

        jobject m_obj;  //proxy to nldb.jni.TclBridge$Rval.
    };

    /**
     * A convenience for handling errors.
     */
    class TclError {
    public:

        enum EType {
            eMsgDone, eRawMsg, eFmtMsg
        };

        /**
         * An error where tcl result was already updated.
         */
        explicit TclError();
        /**
         * The message gets appended to tcl result.
         * @param interp the tcl interpreter.
         * @param msg the message.
         */
        explicit TclError(Tcl_Interp *interp, string msg);

        /**
         * Append formatted error message to tcl result.
         * @param interp  the tcl interpreter.
         * @param unused  not used (here to give different signature).
         * @param code format code
         * @param ... arguments to format.
         */
        explicit TclError(Tcl_Interp *interp, string code,
                string s1, string s2 = "", string s3 = "", string s4 = "");

        EType getType() const {
            return m_type;
        }

        //Allow copy contructors
    private:
        const EType m_type;
    };

    /**
     * Wrap jni interface.
     */
    class Jni {
    public:
        explicit Jni();

        explicit Jni(TRcStringAr args);

        /**
         * Create Jni/JVM with delimited options.
         * @param penv delimited options.
         * @delimiter option delimiter.
         */
        explicit Jni(const char *penv, char delimiter=':');

        /**
         * Find class.
         * @param clsNm class name.
         * @param glbl set true to create global reference; else local reference.
         * @return class.
         */
        jclass findClass(string clsNm, bool glbl = true) ;

        /**
         * Get static method ID.
         * @param clsNm class name.
         * @param methodNm method name.
         * @param sig method signature.
         * @return method ID.  Can be assigned directly to static var.
         */
        jmethodID getStaticMethodId(string clsNm, string methodNm, string sig) ;

        /**
         * Get class method ID.
         * @param clsNm class name.
         * @param methodNm method name.
         * @param sig method signature.
         * @return method ID.  Can be assigned directly to static var.
         */
        jmethodID getClassMethodId(string clsNm, string methodNm, string sig) ;

        /**
         * Create java.lang.Object[].
         * @param sz size of array.
         * @return object array.
         */
        jobjectArray getNewJObjectArray(int sz) ;

        /**
         * Check for new exception.
         * @return true on new exception.
         */
        bool checkForExceptions();
        
        /**
         * Return true if any exceptions have occurred.
         * @return true if any exceptions have occurred.
         */
        bool hadException() const {
            return m_hadException;
        }

        const JNIEnv* getEnv() const {
            return mp_env;
        }

        JNIEnv* getEnv() {
            return mp_env;
        }

        static Jni* getTheOne() {
            return stpTheOne;
        }

        virtual ~Jni();

    private:
        static Jni *stpTheOne;

        void init(TRcStringAr args);

        DECL_COPY_CONSTRUCTORS(Jni);

        JNIEnv *mp_env;
        JavaVM *mp_jvm;
        JavaVMOption *mp_vmOpts;
        JavaVMInitArgs m_vmArgs;
        bool    m_hadException;
    };

    /**
     * Allocate memory for copy of s.
     * @param s string to dup.
     * @param pn pointer to int to return num of chars (0 for no return).
     * @return newly alloc'd dup.
     */
    char* strdup(const string s, int *pn = 0);
    
    inline
    jmethodID getStaticMethodId(string clsNm, string methodNm, string sig) {
        return Jni::getTheOne()->getStaticMethodId(clsNm, methodNm, sig);
    }

    inline
    jmethodID getClassMethodId(string clsNm, string methodNm, string sig) {
        return Jni::getTheOne()->getClassMethodId(clsNm, methodNm, sig);
    }

    inline
    jobjectArray getNewJObjectArray(int sz) {
        return Jni::getTheOne()->getNewJObjectArray(sz);
    }

    inline
    jclass findClass(string clsNm, bool glbl = true) {
        return Jni::getTheOne()->findClass(clsNm, glbl);
    }

    inline
    bool checkForExceptions() {
        return Jni::getTheOne()->checkForExceptions();
    }

    inline
    bool hasException() {
        return Jni::getTheOne()->hadException();
    }
    
    inline
    Tcl_Obj* newStringObj(string s) {
        return Tcl_NewStringObj(s.c_str(), -1);
    }

    inline
    string to_s(Tcl_Obj CONST* obj) {
        return Tcl_GetString(const_cast<Tcl_Obj*> (obj));
    }

    int to_i(Tcl_Interp *interp, Tcl_Obj CONST* obj) throw (TclError);

    double to_double(Tcl_Interp *interp, Tcl_Obj CONST* obj) throw (TclError);

    string to_s(int i);

    Tcl_Obj* newDoubleObj(const char *pDouble);

    /**
     * Convert tcl list to string array.
     * @param interp interpreter to use.
     * @param lobj tcl list object.
     * @return string array
     */
    TRcStringAr listAsStringAr(Tcl_Interp *interp, Tcl_Obj *lobj) throw (TclError);

    Tcl_Obj* newStringObj(bool b);

    /**
     * Convert argv elements to jobject (java.lang.Object) types.
     * @param argc number of elements in argv
     * @param argv elements to convert.
     * @return local reference (not global) to jobject array.
     */
    jobjectArray toJObjectArray(const int argc, Tcl_Obj *CONST argv[]);

    jobject toJBooleanObj(const Tcl_Obj *obj)  throw (TclError);

    jobject toJIntObj(const Tcl_Obj *obj) throw (TclError);

    jobject toJDoubleObj(const Tcl_Obj *obj) throw (TclError);

    jobject toJStringObj(const Tcl_Obj *obj) throw (TclError);
}

#endif	/* NLDB_UTIL_HXX */

