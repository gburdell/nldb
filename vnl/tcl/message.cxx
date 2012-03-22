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
#include <cstdio>
#include "xyzzy/assert.hxx"
#include "vnl/tcl/message.hxx"

namespace vnltcl {
	using namespace std;

    Message Message::stTheOne;
    
    const string Message::stPfxs[] = {"Info", "Warn", "Error", "Fatal"};
    ostream* stpOS[] = {&cout, &cout, &cerr, &cerr};
        
    Message::Message() {
        for (unsigned i = 0; i < eLast; ) {
            m_msgCnts[i++] = 0;
        }
        //alphabetical order
        m_fmtByCode["ATTR-1"]  = "'%s' attribute not valid for '%s' type object";
        m_fmtByCode["COLL-1"]  = "'%s' >= %s (length of collection)";
        m_fmtByCode["DES-1"]   = "current_design is not set";
        m_fmtByCode["EDIT-1"]  = "unable to insert %s '%s' into design '%s'";
        m_fmtByCode["EDIT-2"]  = "insert %s '%s' into design '%s'";
        m_fmtByCode["EDIT-3"]  = "unable to remove %s '%s' from design '%s'";
        m_fmtByCode["EDIT-4"]  = "remove %s '%s' from design '%s'";
        m_fmtByCode["LINK-1"]  = "'%s' reference unresolved";
        m_fmtByCode["OBJ-1"]   = "%s '%s' does not exist in current design";
        m_fmtByCode["OBJ-2"]   = "'%s' does not refer to an object";
        m_fmtByCode["OBJ-3"]   = "'%s' is not '%s' type object";
        m_fmtByCode["OBJ-4"]   = "%s '%s' exists in design '%s'";
        m_fmtByCode["OBJ-5"]   = "%s '%s' does not exist in design '%s'";
        m_fmtByCode["OBJ-6"]   = "expected collection of size 1";
        m_fmtByCode["OPTS-1"]  = "'%s' and '%s' options are mutually exclusive";
        m_fmtByCode["PARSE-1"] = "cannot continue due to previous error(s)";
        m_fmtByCode["VLOGO-1"] = "write design '%s' to netlist '%s'";
    }
    
    void 
    Message::error(Tcl_Interp *interp, string code, 
                   string s1, string s2, string s3, string s4) {
        message(eError, interp, code, s1, s2, s3, s4);
    }
    
    void 
    Message::warn(string code, string s1, string s2, string s3, string s4) {
        message(eWarn, 0, code, s1, s2, s3, s4);
    }
    
    void 
    Message::info(string code, string s1, string s2, string s3, string s4) {
        message(eInfo, 0, code, s1, s2, s3, s4);
    }
    
    void
    Message::error(Tcl_Interp *interp, string msg) {
        message(eError, msg, interp);
    }
    
    void 
    Message::message(EType type, Tcl_Interp *interp, string code, 
                     string s1, string s2, string s3, string s4) {
        string msg = message(type, code, s1, s2, s3, s4);
        message(type, msg, interp);
    }

    string
    Message::message(EType type, string code, 
                     string s1, string s2, string s3, string s4) {
        static const unsigned N = 1024;
        static char pbuf[N];
        fmtByCode_t::const_iterator fmt = m_fmtByCode.find(code);
        ASSERT_TRUE(m_fmtByCode.end() != fmt);
        const char *pfmt = fmt->second.c_str();
        ASSERT_TRUE(0 <= snprintf(pbuf, N, pfmt, 
                s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str()));
        string msg = stPfxs[type] + ": " + pbuf + ".  (" + code + ")";
        return msg;
    }
    
    void 
    Message::message(EType type, string msg, Tcl_Interp *interp) {
        m_msgCnts[type]++;
        if (0 != interp) {
            Tcl_AppendResult(interp, msg.c_str(), NULL);
        } else {
            *stpOS[type] << msg << endl;
        }                
    }
}
