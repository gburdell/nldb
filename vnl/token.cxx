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

#include <sstream>
#include "xyzzy/assert.hxx"
#include "vnl/token.hxx"

namespace vnl {
    using std::stringstream;
    using std::hex;
    using std::oct;

    bool
    Token::toUnsigned(unsigned &res) const {
        ASSERT_TRUE(isType(Token::eUint));
        stringstream ss(getText());
        ss >> res;
        return !ss.fail();
    }

    bool 
    Token::toBits(PTArray<bool> &bits) const {
        string s = getText();
        ASSERT_TRUE('\'' == s[0]);
        string val = s.substr(2);
        bool ok = true;
        switch (s[1]) {
            case 'b': case 'h': case 'o':
                break;
            case 'd':
            { //convert to hex
                stringstream ss(val);
                long unsigned n;
                ss >> n;
                stringstream oss;
                oss << hex << n;
                ok = !(ss.fail() || oss.fail());
                if (ok) {
                    val = oss.str();
                    s[1] = 'h';
                }
            }
                break;
            default:
                ok = false;
        }
        char c;
        unsigned k = 0;
        for (int i = val.length() - 1; ok && (i >= 0); i--, k++) { //lsb->msb
            c = val[i];
            if (s[1] == 'b') {
                bits[k] = (c == '1'); //true|false
            } else {
                int nbit;
                unsigned u;
                stringstream ss;
                if (s[1] == 'h') {
                    ss << hex << c;
                    nbit = 4;
                } else {
                    ss << oct << c;
                    nbit = 3;
                }
                ss >> u;
                ok = !ss.fail();
                if (ok) {
                    unsigned mask = 0x01;
                    for (int j = 0; j < nbit; j++, mask <<= 1) {
                        bits[k] = ((u & mask) != 0);
                    }
                }
            }
        }
        for (; ok && (k < bits.length()); k++) {
            bits[k] = false;
        }
        return ok;
    }
}

