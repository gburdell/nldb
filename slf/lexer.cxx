//The MIT License
//
//slf - Synopsys Library File
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

#include <cctype>
#include <map>
#include <cstdio>
#include "xyzzy/assert.hxx"
#include "slf/lexer.hxx"
#include "slf/message.hxx"

namespace slf {
    using namespace std;

    typedef map<string, Token::EType> t_keywordMap;
    static t_keywordMap stKeywordMap;
    typedef map<Token::EType, string> t_tokenTextMap;
    static t_tokenTextMap stTokenTextMap;

    static bool initKeywords() {
        stKeywordMap["library"] = Token::eLibrary;
        stKeywordMap["cell"] = Token::eCell;
        stKeywordMap["true"] = Token::eTrue;
        stKeywordMap["false"] = Token::eFalse;
        //
        stTokenTextMap[Token::eLibrary] = "library";
        stTokenTextMap[Token::eCell] = "cell";
        stTokenTextMap[Token::eTrue] = "true";
        stTokenTextMap[Token::eFalse] = "false";

        stTokenTextMap[Token::eColon] = ":";
        stTokenTextMap[Token::eComma] = ",";
        stTokenTextMap[Token::eLCurly] = "{";
        stTokenTextMap[Token::eRCurly] = "}";
        stTokenTextMap[Token::eLParen] = "(";
        stTokenTextMap[Token::eRParen] = ")";
        stTokenTextMap[Token::eLBracket] = "[";
        stTokenTextMap[Token::eRBracket] = "]";
        stTokenTextMap[Token::eSemi] = ";";
        stTokenTextMap[Token::eStar] = "*";
        stTokenTextMap[Token::ePlus] = "+";
        stTokenTextMap[Token::eIdent] = "<ident>";
        stTokenTextMap[Token::eInteger] = "<integer>";
        stTokenTextMap[Token::eFloat] = "<float>";
        stTokenTextMap[Token::eString] = "<string>";
        stTokenTextMap[Token::eEOF] = "<EOF>";
        return true;
    }

    static bool initialize() {
        //add messages
        vnl::Message &msg = vnl::Message::getTheOne();
        msg.addFmtByCode("SLF-FILE-1", "%s: processing ..");
        msg.addFmtByCode("SLF-PARSE-1", "%s: expected '%s', found '%s'");
        msg.addFmtByCode("SLF-PARSE-2", "%s: '%s' unexpected");
        msg.addFmtByCode("SLF-PARSE-3", "%s: processed %s seconds");
        return initKeywords();
    }

    Lexer::Lexer(string fname)
    : m_fname(fname), m_ifs(fname.c_str()), m_lnum(1), m_col(0),
    m_type(Token::eInvalid), m_ch(0), m_pos(0), m_buf(N) {
        static bool doOnce = initialize();
        info("SLF-FILE-1", m_fname);
        time(&m_start);
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
    Lexer::nextToken() throw (TRcToken) {
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
                } else if (isdigit(m_ch) || (m_ch == '-')) {
                    tok = number();
                } else if ('"' == m_ch) {
                    tok = stringx();
                } else if ('/' == m_ch) {
                    comment();
                } else {
                    //single characters
                    switch (m_ch) {
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
                        case '*':
                            m_type = Token::eStar;
                            break;
                        case '+':
                            m_type = Token::ePlus;
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
            if ((isEscaped && !isspace(m_ch)) || 
                    ('_' == m_ch) || 
                    isalnum(m_ch) || 
                    ('.' == m_ch)       //Yuk! really allowed in ident
                    ) {
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
    Lexer::stringx() throw (TRcToken) {
        //enter on "
        m_type = Token::eString;
        unsigned col = m_col;
        m_pos = 0;
        m_buf[m_pos++] = m_ch;
        bool prevWasEscapeChar = false;
        bool syntaxErr = false;
        bool stay = true;
        while (stay && !syntaxErr) {
            if (getCharCheckEOF()) {
                m_ch = -1;
                syntaxErr = true; //premature EOF
            } else if (prevWasEscapeChar) {
                //do nothing
                prevWasEscapeChar = false;
            } else if ('\\' == m_ch) {
                prevWasEscapeChar = true;
            } else if ('"' == m_ch) {
                stay = false;
            } else {
                //do nothing
            }
            if (!syntaxErr) {
                m_buf[m_pos++] = m_ch;
            }
        }
        TRcToken tok = createToken(col);
        reset(0); //dont pushback
        if (syntaxErr) {
            throw (tok);
        }
        return tok;
    }

    // Return eInteger | eFloat which 

    TRcToken
    Lexer::number() throw (TRcToken) {
        //enter on - | digit

        //Use state to track lexer
        //(INTEGER) ('.' (DIGIT)+)? (('e'|'E') ('-')? (DIGIT)+)?
        //state: s0  s1  s2          s3        s4     s5

        enum EState {
            s0, s1, s2, s3, s4, s5
        };
        EState state = s0;
        m_type = Token::eInteger; //until a float
        unsigned col = m_col;
        m_pos = 0;
        m_buf[m_pos++] = m_ch; // - | digit
        bool stay = true;
        bool syntaxErr = false;
        while (stay && !syntaxErr) {
            ASSERT_TRUE(!getCharCheckEOF());
            switch (state) {
                case s0:
                    if (isdigit(m_ch)) {
                        //stay
                    } else if ('.' == m_ch) {
                        m_type = Token::eFloat;
                        state = s1;
                    } else if (('e' == m_ch) || ('E' == m_ch)) {
                        m_type = Token::eFloat;
                        state = s3;
                    } else {
                        stay = false;
                    }
                    break;
                case s1:
                    if (isdigit(m_ch)) {
                        state = s2;
                    } else {
                        syntaxErr = true;
                    }
                    break;
                case s2:
                    if (isdigit(m_ch)) {
                        //stay
                    } else if (('e' == m_ch) || ('E' == m_ch)) {
                        m_type = Token::eFloat;
                        state = s3;
                    } else {
                        stay = false;
                    }
                    break;
                case s3:
                    if (isdigit(m_ch)) {
                        state = s5;
                    } else if ('-' == m_ch) {
                        state = s4;
                    } else {
                        syntaxErr = true;
                    }
                    break;
                case s4:
                    if (isdigit(m_ch)) {
                        state = s5;
                    } else {
                        syntaxErr = true;
                    }
                    break;
                case s5:
                    if (isdigit(m_ch)) {
                        //stay
                    } else {
                        stay = false;
                    }
                    break;
                default:
                    ASSERT_NEVER;
            }
            if (stay && !syntaxErr) {
                m_buf[m_pos++] = m_ch;
            }
        }
        TRcToken tok = createToken(col);
        reset(m_ch);
        if (syntaxErr) {
            throw (tok);
        }
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
        info("SLF-PARSE-3", m_fname, string(buf));
    }
}


//
//BEGIN: TEST
//
//#define T_LEXER
#if defined(T_LEXER)

#include <iostream>
using namespace std;
using namespace slf;

int main(int argc, char *argv[]) {
    ASSERT_TRUE(2 <= argc);
    TRcToken tok;
    TRcLocation loc;
    for (int i = 1; i < argc; i++) {
        Lexer lexer(argv[i]);
        try {
            while (true) {
                tok = lexer.nextToken();
                if (Token::eEOF == tok->getType()) break; //while
                loc = tok->getLocation();
                //cout << "DBG:" << tok->toString() << endl;
            }
        } catch (TRcToken badToken) {
            cerr << "BADTOKEN: " << badToken->toString() << endl;
        }
    }
    return 0;
}
#endif

