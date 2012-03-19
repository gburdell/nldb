//The MIT License
//
//vnl - verilog netlist
//Copyright (c) 2006-2010  Karl W. Pfalzer
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
#if !defined(_vnl_lexer_hxx_)
#define  _vnl_lexer_hxx_

#include <fstream>
#include <ctime>
#include "vnl/vnl.hxx"
#include "vnl/token.hxx"
#include "vnl/location.hxx"

namespace vnl {
    using std::ifstream;

    class Lexer {
    public:
        typedef Token::EType EType;

        explicit Lexer(string fname);

        TRcToken nextToken();

        string getFname() const {
            return m_fname;
        }

        static string asString(EType type);
        
        ~Lexer();

    private:
        ifstream m_ifs;
        string m_fname;
        unsigned m_lnum, m_col;
        time_t m_start;

        //Used while building token
        const static int N = 1024;
        PTArray<char> m_buf;
        unsigned m_pos;
        EType m_type;
        int m_ch;

        TRcToken createToken(unsigned col, bool reset = false) {
            return createToken(m_type, col, reset);
        }

        TRcToken createToken(EType type, unsigned col, bool reset = false);

        TRcToken createToken(EType type, bool reset = false) {
            return createToken(type, m_col, reset);
        }

        void eoln();

        void reset(int pushBack = 0);

        bool getCharCheckEOF();

        void whiteSpace();
        void comment();
        TRcToken basedNumber();
        TRcToken uint();
        TRcToken ident();
    };
}

#endif
