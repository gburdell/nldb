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

#include <cstdarg>
#include <cstdio>
#include "xyzzy/assert.hxx"
#include "vnl/message.hxx"
#include "vnl/location.hxx"

using namespace std;

namespace vnl {
    Message Message::stTheOne;

    const string Message::stPfxs[] = {"Info", "Warn", "Error", "Fatal"};
    ostream* stpOS[] = {&cout, &cout, &cerr, &cerr};

    Message::Message() {
        for (unsigned i = 0; i < eLast;) {
            m_msgCnts[i++] = 0;
        }
        //alphabetical order
        m_fmtByCode["VNL-ASSN-1"] = "%s: lhs.length %s != rhs.length %s";
        m_fmtByCode["VNL-BUS-1"] = "%s: '%s' invalid bus range";
        m_fmtByCode["VNL-CELL-1"] = "%s: connection for pin '%s' already specified";
        m_fmtByCode["VNL-CELL-2"] = "%s: instance name '%s' already specified";
        m_fmtByCode["VNL-DECL-1"] = "%s: %s '%s' already defined";
        m_fmtByCode["VNL-DECL-2"] = "%s: '%s' undefined";
        m_fmtByCode["VNL-FILE-1"] = "%s: processing ..";
        m_fmtByCode["VNL-MOD-1"] = "%s: module '%s' already defined in library '%s'";
        m_fmtByCode["VNL-NUM-1"] = "%s: '%s' malformed number";
        m_fmtByCode["VNL-NUM-2"] = "%s: based number '%s' missing size";
        m_fmtByCode["VNL-PARSE-1"] = "%s: expected '%s', found '%s'";
        m_fmtByCode["VNL-PARSE-2"] = "%s: '%s' unexpected";
        m_fmtByCode["VNL-PARSE-3"] = "%s: processed %s seconds";
        m_fmtByCode["VNL-PORT-1"] = "%s: port '%s' defined, but not declared in port list";
        m_fmtByCode["VNL-PORT-2"] = "%s: port '%s' redefined";
        m_fmtByCode["VNL-WIRE-2"] = "%s: wire '%s' redefined";
    }

    void
    Message::message(EType type, string code,
            const TRcLocation &loc, string s1, string s2, string s3) {
        string xloc = loc->toString();
        string msg = fmessage(type, code, xloc, s1, s2, s3);
        message(type, msg);
    }

    string
    Message::fmessage(EType type, string code,
            string s1, string s2, string s3, string s4) {
        static const unsigned N = 1024;
        static char pbuf[N];
        t_fmtByCode::const_iterator fmt = m_fmtByCode.find(code);
        ASSERT_TRUE(m_fmtByCode.end() != fmt);
        const char *pfmt = fmt->second.c_str();
        ASSERT_TRUE(0 <= snprintf(pbuf, N, pfmt,
                s1.c_str(), s2.c_str(), s3.c_str(), s4.c_str()));
        string msg = stPfxs[type] + ": " + pbuf + ".  (" + code + ")";
        return msg;
    }

    void
    Message::message(EType type, string msg) {
        m_msgCnts[type]++;
        *stpOS[type] << msg << endl;
    }

    void error(string code,
            const TRcLocation &loc, string s1, string s2, string s3) {
        Message::getTheOne().message(Message::eError, code, loc, s1, s2, s3);
    }
    
    void info(string code, string s1, string s2, string s3, string s4) {
        string msg = Message::getTheOne().fmessage(Message::eInfo, code, s1, s2, s3, s4);
        Message::getTheOne().message(Message::eInfo, msg);
    }

}
