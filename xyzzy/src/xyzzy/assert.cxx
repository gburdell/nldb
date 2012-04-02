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
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <execinfo.h>
#include "xyzzy/assert.hxx"

#ifndef NO_EXIT_ON_ASSERT
#define EXIT exit(EXIT_FAILURE)
#else
#define EXIT if(0)
#endif

namespace xyzzy {
    using namespace std;

    static
    void
    dumpBacktrace(ostream &os = cerr) {
#ifdef DEBUG
        os << "Stack dump:" << endl 
                << "Use 'addr2line' to convert to actual source-code locations"
                << endl;
        int j, nptrs;
        static unsigned SIZE = 16;
        void *buffer[SIZE];
        char **strings;
        nptrs = backtrace(buffer, SIZE);
        strings = backtrace_symbols(buffer, nptrs);
        if (strings == NULL) {
            perror("backtrace_symbols");
            exit(EXIT_FAILURE);
        }
        for (j = 0; j < nptrs; j++)
            os << strings[j] << " ";
        os << endl;
        free(strings);
#endif
    }
    
    bool assertTrue(const bool cond) {
        if (false == cond) {
            cerr << "Assertion failed." << endl;
            dumpBacktrace();
            EXIT;
        }
        return cond;
    }

    bool assertTrue(const bool cond, const char* cexpr,
            const char* fname, unsigned lnum) {
        if (false == cond) {
            cerr << "Assertion failed: "
                    << fname << ":" << lnum
                    << ": \"" << cexpr << "\"" << endl;
            dumpBacktrace();
            EXIT;
        }
        return cond;
    }

    void die(const char *pfx) {
        perror(pfx);
        exit(EXIT_FAILURE);
    }

};
