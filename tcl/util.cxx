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
#include <fstream>
#include <cstdarg>
#include <iosfwd>
#include "tcl.h"
#include "tclDecls.h"
#include "tcl/util.hxx"
#include "tcl/message.hxx"

namespace vnltcl {
    using namespace std;
    
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

    bool isFileReadable(string fn) {
        ifstream ifs(fn.c_str());
        bool ok = ifs.good();
        ifs.close();
        return ok;
    }

}

