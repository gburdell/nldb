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

#include <cctype>
#include <map>
#include <cstdio>
#include "vnl/lexer.hxx"
#include "vnl/message.hxx"
#include "xyzzy/assert.hxx"

namespace vnl {
    using namespace std;

    typedef map<string, Token::EType> t_keywordMap;
    static t_keywordMap stKeywordMap;
    typedef map<Token::EType, string> t_tokenTextMap;
    static t_tokenTextMap stTokenTextMap;

    static bool initKeywords() {
        stKeywordMap["module"] = Token::eModule;
        stKeywordMap["endmodule"] = Token::eEndModule;
        stKeywordMap["input"] = Token::eInput;
        stKeywordMap["output"] = Token::eOutput;
        stKeywordMap["inout"] = Token::eInout;
        stKeywordMap["wire"] = Token::eWire;
        stKeywordMap["assign"] = Token::eAssign;
        //
        stTokenTextMap[Token::eModule] = "module";
        stTokenTextMap[Token::eEndModule] = "endmodule";
        stTokenTextMap[Token::eInput] = "input";
        stTokenTextMap[Token::eOutput] = "output";
        stTokenTextMap[Token::eInout] = "inout";
        stTokenTextMap[Token::eWire] = "wire";
        stTokenTextMap[Token::eAssign] = "assign";
        stTokenTextMap[Token::eEq] = "=";
        stTokenTextMap[Token::eLParen] = "(";
        stTokenTextMap[Token::eRParen] = ")";
        stTokenTextMap[Token::eLCurly] = "{";
        stTokenTextMap[Token::eRCurly] = "}";
        stTokenTextMap[Token::eLBracket] = "[";
        stTokenTextMap[Token::eRBracket] = "]";
        stTokenTextMap[Token::eComma] = ",";
        stTokenTextMap[Token::eSemi] = ";";
        stTokenTextMap[Token::eColon] = ":";
        stTokenTextMap[Token::eDot] = ".";
        stTokenTextMap[Token::eIdent] = "<ident>";
        stTokenTextMap[Token::eBasedNumber] = "<based_number>";
        stTokenTextMap[Token::eUint] = "<unsigned>";
        stTokenTextMap[Token::eEOF] = "<EOF>";
    }

    Lexer::Lexer(string fname)
    : m_fname(fname), m_ifs(fname.c_str()), m_lnum(1), m_col(0),
    m_type(Token::eInvalid), m_ch(0), m_pos(0), m_buf(N) {
        info("VNL-FILE-1", m_fname);
        time(&m_start);
        static bool doOnce = initKeywords();
    }

    string
    Lexer::asString(EType type) {
        return stTokenTextMap[type];
    }

    void
    Lexer::reset(int pushBack) {
        m_pos = 0;
        if (0 <= m_ch) {
            m_ch = pushBack; //dont reset on EOF
            m_type = Token::eInvalid;
        }
    }

    TRcToken
    Lexer::createToken(EType type, unsigned col, bool doReset) {
        m_buf[m_pos] = '\0';
        string text(&m_buf[0]);
        TRcLocation loc = new Location(m_fname, m_lnum, col);
        TRcToken tok = new Token(type, text, loc);
        if (doReset) reset();
        return tok;
    }

    bool
    Lexer::getCharCheckEOF() {
        m_ch = m_ifs.get();
        m_col++;
        return m_ifs.eof();
    }

    TRcToken
    Lexer::nextToken() {
        TRcToken tok;
        while (!tok.isValid()) {
            if (0 > m_ch) {
                tok = createToken(Token::eEOF, true);
            } else if (0 == m_ch) {
                if (getCharCheckEOF()) {
                    m_ch = -1;
                    m_pos = 0;
                    tok = createToken(Token::eEOF, true);
                } else {
                    ASSERT_TRUE(m_ifs.good());
                }
            }
            if (tok.isNull()) {
                m_buf[m_pos++] = m_ch;
                if (isspace(m_ch)) {
                    whiteSpace();
                } else if (isalpha(m_ch) || (m_ch == '_') || (m_ch == '\\')) {
                    tok = ident();
                } else if (isdigit(m_ch)) {
                    tok = uint();
                } else if ('\'' == m_ch) {
                    tok = basedNumber();
                } else if ('/' == m_ch) {
                    comment();
                } else {
                    //single characters
                    //eEq, eLParen, eRParen, eLCurly, eRCurly,
                    //eLBracket, eRBracket,
                    //eComma, eSemi, eColon,
                    switch (m_ch) {
                        case '=':
                            m_type = Token::eEq;
                            break;
                        case '(':
                            m_type = Token::eLParen;
                            break;
                        case ')':
                            m_type = Token::eRParen;
                            break;
                        case '{':
                            m_type = Token::eLCurly;
                            break;
                        case '}':
                            m_type = Token::eRCurly;
                            break;
                        case '[':
                            m_type = Token::eLBracket;
                            break;
                        case ']':
                            m_type = Token::eRBracket;
                            break;
                        case ',':
                            m_type = Token::eComma;
                            break;
                        case ';':
                            m_type = Token::eSemi;
                            break;
                        case ':':
                            m_type = Token::eColon;
                            break;
                        case '.':
                            m_type = Token::eDot;
                            break;
                        default:
                            ASSERT_NEVER;
                    }
                    tok = createToken(m_col, true);
                }
            }
        }
        return tok;
    }

    void
    Lexer::comment() {
        // enter upon '/'

        enum EState {
            eEnter, eLineComment, eWaitStar, eWaitSlash, eDone
        };
        EState state = eEnter;
        while (eDone != state) {
            if (getCharCheckEOF() && (eLineComment == state)) {
                m_ch = -1;
                return;
            }
            ASSERT_TRUE(!m_ifs.eof());
            if ('\n' == m_ch) {
                eoln();
            }
            switch (state) {
                case eEnter:
                    if ('/' == m_ch) {
                        state = eLineComment;
                    } else {
                        ASSERT_TRUE('*' == m_ch);
                        state = eWaitStar;
                    }
                    break;
                case eLineComment:
                    if ('\n' == m_ch) {
                        state = eDone;
                    }
                    break;
                case eWaitStar:
                    if ('*' == m_ch) {
                        state = eWaitSlash;
                    }
                    break;
                case eWaitSlash:
                    if ('/' == m_ch) {
                        state = eDone;
                    } else if ('*' != m_ch) {
                        state = eWaitStar;
                    }
                    break;
                default:
                    ASSERT_NEVER;
            }
        }
        reset();
    }

    void
    Lexer::eoln() {
        m_lnum++;
        m_col = 0;
    }

    void
    Lexer::whiteSpace() {
        if ('\n' == m_ch) {
            eoln();
        }
        while (true) {
            if (getCharCheckEOF()) {
                m_ch = -1;
                return;
            }
            if ('\n' == m_ch) {
                eoln();
            } else if (!isspace(m_ch)) {
                break;
            }
        }
        reset(m_ch);
    }

    TRcToken
    Lexer::ident() { //or keyword
        m_type = Token::eIdent;
        unsigned col = m_col;
        bool isEscaped = ('\\' == m_ch);
        m_pos = 0;
        if (!isEscaped) { //drop escape
            m_buf[m_pos++] = m_ch;
        }
        while (true) {
            if (getCharCheckEOF()) {
                m_ch = -1;
                break;
            }
            if ((isEscaped && !isspace(m_ch)) || ('_' == m_ch) || isalnum(m_ch)) {
                m_buf[m_pos++] = m_ch;
            } else {
                break; //while
            }
        }
        TRcToken tok = createToken(col);
        if (!isEscaped) {
            //check for keyword
            t_keywordMap::const_iterator iter = stKeywordMap.find(tok->m_text);
            if (iter != stKeywordMap.end()) {
                tok->m_type = iter->second;
            }
        }
        reset(m_ch);
        return tok;
    }

    TRcToken
    Lexer::uint() {
        m_type = Token::eUint;
        unsigned col = m_col;
        m_pos = 0;
        m_buf[m_pos++] = m_ch;
        while (true) {
            ASSERT_TRUE(!getCharCheckEOF());
            if (isdigit(m_ch)) {
                m_buf[m_pos++] = m_ch;
            } else if ('_' != m_ch) { //dont keep _
                break; //while
            }
        }
        TRcToken tok = createToken(col);
        reset(m_ch);
        return tok;
    }

    TRcToken
    Lexer::basedNumber() {
        m_type = Token::eBasedNumber;
        unsigned col = m_col;
        m_pos = 0;
        m_buf[m_pos++] = m_ch;

        enum EState {
            eWaitBase, eNumber, eDone
        };
        EState state = eWaitBase;
        while (eDone != state) {
            ASSERT_TRUE(!getCharCheckEOF());
            m_ch = tolower(m_ch);
            switch (state) {
                case eWaitBase:
                    switch (m_ch) {
                        case 'b': case 'd': case 'o': case 'h':
                            m_buf[m_pos++] = m_ch;
                            break;
                        default:
                            ASSERT_NEVER;
                    }
                    state = eNumber;
                    break;
                case eNumber:
                    if (isdigit(m_ch) || ('a' <= m_ch && 'f' >= m_ch)) {
                        m_buf[m_pos++] = m_ch;
                    } else if ('_' != m_ch) {
                        state = eDone;
                    }
                    break;
                default:
                    ASSERT_NEVER;
            }
        }
        TRcToken tok = createToken(col);
        reset(m_ch);
        return tok;
    }

    Lexer::~Lexer() {
        m_ifs.close();
        time_t finish;
        time(&finish);
        double deltaSec = difftime(finish, m_start);
        //need sprintf since stringstream cannot set decimal places
        //after dot.
        char buf[128];
        sprintf(buf, "%d line(s) in %.01f", m_lnum - 1, deltaSec);
        info("VNL-PARSE-3", m_fname, string(buf));
    }
}


//
//BEGIN: TEST
//
//#define T_LEXER
#if defined(T_LEXER)

#include <iostream>
using namespace std;
using namespace vnl;

int main(int argc, char *argv[]) {
    ASSERT_TRUE(2 <= argc);
    TRcToken tok;
    TRcLocation loc;
    for (int i = 1; i < argc; i++) {
        Lexer lexer(argv[i]);
        cout << "Info: " << lexer.getFname() << ": processing ..." << endl;
        while (true) {
            tok = lexer.nextToken();
            if (Token::eEOF == tok->getType()) break; //while
            loc = tok->getLocation();
            //cout << "DBG:" << tok->toString() << endl;
        }
    }
    return 0;
}
#endif

