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

#ifndef NLDB_COMMANDS_HXX
#define	NLDB_COMMANDS_HXX

#include <map>
#include "tcl.h"
#include "util.hxx"

namespace nldb {
    /**
     * Singleton object of commands.
     */
    class Commands {
    public:

        static Commands& getTheOne() {
            return stTheOne;
        }
        
        void info(string code, string s1, string s2="", string s3="", string s4="");
        
        void warn(string code, string s1, string s2="", string s3="", string s4="");

        void error(string code, string s1, string s2="", string s3="", string s4="");

        /**
         * Create Tcl object.
         * @param obj jobject (peer-side (i.e., java-side) of proxy).
         * @return tcl object.
         */
        Tcl_Obj* createTclObj(jobject obj) const ;
        
        JObject* getObject(Tcl_Obj *p) const throw (TclError);
                
        Commands& setObjType(const Tcl_ObjType &r);
        
        Commands& setJni(Jni *pjni) {
            mp_jni = pjni;
            return *this;
        }

        /**
         * Execute command specified by argv[0].
         * @param interp interpreter to use.
         * @param argc number of arguments.
         * @param argv arguments.
         * @return result or 0 if error. 
         */
        Tcl_Obj* dispatch(Tcl_Interp *interp, const int argc, Tcl_Obj *CONST argv[])
                throw (TclError);

        /**
         * Get interpreter used within dispatch.
         * @return the interpreter.
         */
        Tcl_Interp* getInterp() const {
            return mp_interp;
        }
        
        ~Commands();

    private:
        /**
         * Convert peer object (nldb.jni.TclBridge$Rval) to Tcl_Obj.
         * @param obj peer object.
         * @return tcl object.
         */
        static Tcl_Obj* convertPeerObj(const jobject obj);
        
        explicit Commands();

        DECL_COPY_CONSTRUCTORS(Commands);

        static Commands stTheOne;
        static string stPeerClsNm;

        const Tcl_ObjType       *mp_objType;
        Tcl_Interp              *mp_interp;
        Jni                     *mp_jni;
    };
}

#endif	/* NLDB_COMMANDS_HXX */

