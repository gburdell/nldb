//The MIT License
//
//xyzzy - c++ library
//Copyright (c) 2007-2010  Karl W. Pfalzer
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

#include <regex.h>
#include "xyzzy/assert.hxx"
#include "xyzzy/regexp.hxx"

namespace xyzzy {
    
    /*
     * NOTE: mp_rex is opaque so this implementation can work w/ tcl,
     * which sadly had an include called regex.h which conflicts w/
     * the stdlib regex.h.
     */

    inline regex_t* asP(void* p) {
        return (regex_t*)p;
    }
    
    TRegExp::TRegExp(string rex) throw (const string&) {
        static const unsigned N = 128;
        static char buf[N];
        mp_rex = new regex_t();
        int code;
        code = regcomp(asP(mp_rex), rex.c_str(), REG_EXTENDED | REG_NOSUB);
        if (0 != code) {                
            regerror(code, asP(mp_rex), &buf[0], N);
            static string emsg;
            emsg = buf;
            throw emsg;
        }
    }
    
    bool 
    TRegExp::match(string s) const {
        const char *p = s.c_str();
        return (0 == regexec(asP(mp_rex), p, 0, NULL, 0));
    }
    
    TRegExp::~TRegExp() {
        regfree(asP(mp_rex));
    }
}
