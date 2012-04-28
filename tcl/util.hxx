//The MIT License
//
//vnl - verilog netlist
//Copyright (c) 2006-2010  Karl W. Pfalzer
//Copyright (c) 2012-      George P. Burdell
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
#ifndef VNLTCL_UTIL_HXX
#define	VNLTCL_UTIL_HXX

#include <string>
#include "tcl.h"
#include "xyzzy/array.hxx"
#include "tcl/vnltcl.hxx"

namespace vnltcl {
    using xyzzy::PTArray;
    using xyzzy::PTRcArray;

    typedef PTArray<string> TStringAr;
    typedef PTRcArray<string> TRcStringAr;

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

    inline
    Tcl_Obj* newStringObj(string s) {
        return Tcl_NewStringObj(s.c_str(), -1);
    }

    inline
    string to_s(Tcl_Obj CONST* obj) {
        return Tcl_GetString(const_cast<Tcl_Obj*> (obj));
    }

    int to_i(Tcl_Interp *interp, Tcl_Obj CONST* obj) throw (TclError);

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
     * Check that file is readable.
     * @param fn filename to check.
     * @return true if readable.
     */
    bool isFileReadable(string fn);
}

#endif	/* VNLTCL_UTIL_HXX */

