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

#include <sstream>
#include "xyzzy/assert.hxx"
#include "slf/parser.hxx"
#include "keyvalue.hxx"

namespace slf {
    using std::ostringstream;

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
                        error("SLF-PARSE-2", tok->getLocation(), tok->getText());
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
        return ele;  //could be nil
    }

    TRcLibCell
    Parser::cell() throw (unsigned) {
        // _ cell
        TRcLibCell libcell;
        expectAccept(Token::eCell);
        //TODO
        return libcell;
    }

    TRcKeyValue
    Parser::keyValue() throw (unsigned) {
        TRcKeyValue keyval;
        return keyval;
    }

    Parser::~Parser() {
    }

}

