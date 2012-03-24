//The MIT License
//
//slf - Synopsys Liberty File
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

#include "xyzzy/assert.hxx"
#include "slf/parser.hxx"
#include "slf/keyvalue.hxx"
#include "slf/message.hxx"
#include "slf/libcell.hxx"
#include "slf/valueset.hxx"
#include "slf/bus.hxx"
#include "slf/expr.hxx"
#include "slf/exprop.hxx"
#include "slf/exprval.hxx"
#include "slf/libraryele.hxx"
#include "slf/valuetype.hxx"
#include "slf/number.hxx"

namespace slf {

    static
    void
    error2(const TRcToken &tok) {
        error("SLF-PARSE-2", tok->getLocation(), tok->getText());
    }

    Parser::Parser(TRcLexer &lexer)
    : m_lexer(lexer), m_errCnt(0) {
    }

    TRcToken
    Parser::la(unsigned i) {
        TRcToken tok;
        if (i < m_toks.size()) {
            tok = m_toks[i];
        } else {
            unsigned n = 1 + (i - m_toks.size());
            for (int j = 0; j < n; j++) {
                tok = m_lexer->nextToken();
                m_toks.push_back(tok);
            }
        }
        return tok;
    }

    bool
    Parser::test(EType type, unsigned i) {
        TRcToken tok = la(i);
        return (tok->getType() == type);
    }

    TRcToken
    Parser::accept(unsigned n) {
        ASSERT_TRUE(n <= m_toks.size());
        TRcToken tok = m_toks[n - 1];
        for (unsigned i = 0; i < n; i++) {
            m_toks.pop_front();
        }
        return tok;
    }

    bool
    Parser::expect(EType type, unsigned i) {
        bool ok = test(type, i);
        if (!ok) {
            TRcToken got = la(i);
            error("SLF-PARSE-1", got->getLocation(), Lexer::asString(type),
                  got->getText());
        }
        return ok;
    }

    void
    Parser::slurpUntil(EType type) {
        TRcToken tok;
        while (true) {
            tok = la(0);
            accept();
            if (tok->isEOF() || (tok->isType(type))) {
                break;
            }
        }
    }

    TRcToken
    Parser::expectAccept(EType type) throw (unsigned) {
        if (expect(type)) {
            return accept();
        }
        throw (++m_errCnt);
    }

    void
    Parser::start(TRcLibrary &lib) {
        sourceText(lib);
    }

    void
    Parser::sourceText(TRcLibrary &lib) throw (unsigned) {
        TRcToken tok;
        try {
            while (la(0)->getType() == Token::eLibrary) {
                accept();
                expectAccept(Token::eLParen);
                switch (la(0)->getType()) {
                    case Token::eIdent: case Token::eString:
                        break;
                    default:
                        error2(tok);
                        throw (++m_errCnt);
                }
                accept();
                expectAccept(Token::eRParen);
                expectAccept(Token::eLCurly);
                while (Token::eRCurly != la(0)->getType()) {
                    TRcLibraryEle lele = libraryEle();
                    //NOTE: lele.isNull iff. la(1)=='}'
                    //TODO: put cells in lib and otherstuff???
                }
                expectAccept(Token::eRCurly);
            }
        } catch (unsigned cnt) {
            slurpUntil(Token::eEOF); //basic error recovery if we get this far
        }
        expect(Token::eEOF);
    }

    TRcLibraryEle
    Parser::libraryEle() throw (unsigned) {
        TRcLibraryEle ele;
        Token::EType type = la(0)->getType();
        if (Token::eCell == type) {
            TRcLibCell libcell = cell();
            ele = toLibraryEle(libcell);
        } else if (Token::eRCurly != type) {
            TRcKeyValue keyval = keyValue();
            ele = toLibraryEle(keyval);
        } //else '}'
        return ele; //could be nil
    }

    TRcLibCell
    Parser::cell() throw (unsigned) {
        // _ cell
        TRcLibCell libcell;
        expectAccept(Token::eCell);
        expectAccept(Token::eLParen);
        TRcToken tok = accept();
        string cellNm;
        if (Token::eIdent == tok->getType()) {
            cellNm = tok->getText();
        } else if (Token::eString == tok->getType()) {
            string s = tok->getText();
            cellNm = s.substr(1, s.size() - 2); //lose ""
        } else {
            error2(tok);
            throw (++m_errCnt);
        }
        expectAccept(Token::eRParen);
        TRcValueSet valset = valueSet();
        //TODO: add valset to libcell
        return libcell;
    }

    TRcKeyValue
    Parser::keyValue() throw (unsigned) {
        TRcKeyValue keyval;
        TRcToken id = expectAccept(Token::eIdent);
        TRcToken tok = accept();
        if (Token::eColon == tok->getType()) {
            // COLON _ value_type (SEMI)?
            TRcValueType valtype = valueType();
            //TODO: something w/ valtype
            if (Token::eSemi == la(0)->getType()) {
                accept();
            }
        } else if (Token::eLParen == tok->getType()) {
            // LPAREN _ (value_type_list)? RPAREN
            if (Token::eRParen != la(0)->getType()) {
                TRcValueTypeList valList = valueTypeList();
                //TODO: something w/ valList;
            }
            expectAccept(Token::eRParen);
            // _ (valueSet | SEMI | nil)
            Token::EType type = la(0)->getType();
            if (Token::eLCurly == type) {
                TRcValueSet valset = valueSet();
                //TODO: something w/ valset
            } else if (Token::eSemi == type) {
                accept();
            } //else nil
        } else {
            error2(tok);
            throw (++m_errCnt);
        }
        return keyval;
    }

    TRcValueSet
    Parser::valueSet() throw (unsigned) {
        TRcValueSet valueSet;
        expectAccept(Token::eLCurly);
        while (Token::eRCurly != la(0)->getType()) {
            TRcKeyValue kv = keyValue();
            //TODO: add kv
        }
        expectAccept(Token::eRCurly);
        return valueSet;
    }

    TRcValueType
    Parser::valueType() throw (unsigned) {
        TRcValueType valueType;
        Token::EType types[2] = {la(0)->getType(), la(1)->getType()};

        /*:   ((IDENT|number) expr_op)=> expr
              |   kident (bus)?
              |   STRING
              |   number (options{greedy=true;}: unit)?
              |   bool         
         */
        if (((Token::eIdent == types[0]) || (Token::eFloat == types[0]) || (Token::eInteger == types[0]))
                && (Token::eStar == types[1] || Token::ePlus == types[1])) {
            TRcExpr ex = expr();
            //TODO: something w/ ex
        } else {
            switch (types[0]) {
                case Token::eIdent: case Token::eCell: case Token::eLibrary: //kident
                {
                    TRcToken kident = accept();
                    TRcBus bs;
                    if (Token::eLBracket == types[1]) {
                        bs = bus();
                        //TODO: something w/ bus
                    }
                }
                    break;
                case Token::eString:
                {
                    TRcToken str = accept();
                    //TODO: something w/ str
                }
                    break;
                case Token::eInteger: case Token::eFloat:
                {
                    TRcNumber num = number();
                    TRcToken unit;
                    if (Token::eIdent == types[1]) {
                        unit = accept();
                    }
                }
                    break;
                case Token::eTrue: case Token::eFalse:
                {
                    TRcToken boolVal = accept();
                    //TODO: something w/ boolVal
                }
                    break;
                default:
                {
                    TRcToken bad = la(0);
                    error2(bad);
                    throw (++m_errCnt);
                }
            }
        }
        //TODO
        return valueType;
    }

    TRcBus
    Parser::bus() throw (unsigned) {
        // _ [
        TRcBus bus;
        expectAccept(Token::eLBracket);
        TRcToken lb = expectAccept(Token::eInteger);
        TRcToken rb;
        if (Token::eColon == la(0)->getType()) {
            accept();
            rb = expectAccept(Token::eInteger);
        }
        expectAccept(Token::eRBracket);
        return bus;
    }

    TRcValueTypeList
    Parser::valueTypeList() throw (unsigned) {
        TRcValueTypeList valList;
        while (true) {
            TRcValueType valType = valueType();
            //TODO: something w/ valType
            if (Token::eComma == la(0)->getType()) {
                accept();
            } else {
                break; // _ ')'
            }
        }
        return valList;
    }

    TRcExpr
    Parser::expr() throw (unsigned) {
        TRcExpr ex;
        TRcExprVal exval = exprVal();
        //TODO: something t/ exval
        while (true) {
            Token::EType type = la(0)->getType();
            if (Token::eStar == type || Token::ePlus == type) {
                TRcExprOp eop = exprOp();
                //TODO: something w/ eop
                exval = exprVal();
                //TODO: something w/ exval
            } else {
                break;
            }
        }
        return ex;
    }

    TRcExprVal
    Parser::exprVal() throw (unsigned) {
        TRcExprVal eval;
        Token::EType type = la(0)->getType();
        if (Token::eIdent == type) {
            TRcToken id = accept(); //TODO: something w/ id
        } else if (Token::eInteger == type || Token::eFloat == type) {
            TRcNumber num = number(); //TODO: something w/ num
        } else {
            TRcToken bad = accept();
            error2(bad);
            throw (++m_errCnt);
        }
        return eval;
    }

    TRcExprOp
    Parser::exprOp() {
        //already validated  first-set
        TRcExprOp eop;
        TRcToken op = accept(); // + | *
        //TODO: something w/ op
        return eop;
    }

    TRcNumber
    Parser::number() {
        //already validated  first-set
        TRcNumber num;
        TRcToken n = accept(); // FLOAT | INTEGER
        //TODO: something w/ n
        return num;
    }

    Parser::~Parser() {
    }

}

#if defined(T_PARSER)

#include <iostream>
#include "vnl/parser.hxx"

using namespace std;
using namespace slf;
using vnl::Library;
using vnl::TRcLibrary;

int main(int argc, char *argv[]) {
    ASSERT_TRUE(2 <= argc);
    TRcToken tok;
    TRcLocation loc;
    TRcLexer lexer;
    TRcLibrary work = new Library();
    for (int i = 1; i < argc; i++) {
        lexer = new Lexer(argv[i]);
        //cout << "Info: " << lexer->getFname() << ": processing ..." << endl;
        Parser parser(lexer);
        parser.start(work);
    }
    return 0;
}
#endif

