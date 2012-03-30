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
#include "vnl/library.hxx"
#include "slf/parser.hxx"
#include "slf/keyvalue.hxx"
#include "slf/message.hxx"
#include "slf/libcell.hxx"
#include "slf/bus.hxx"
#include "slf/expr.hxx"
#include "slf/exprop.hxx"
#include "slf/exprval.hxx"
#include "slf/libraryele.hxx"
#include "slf/valuetype.hxx"
#include "slf/number.hxx"

namespace slf {
    static const string cArea = "area";
    static const string cArray = "array";
    static const string cBaseType = "base_type";
    static const string cBit = "bit";
    static const string cBitFrom = "bit_from";
    static const string cBitTo = "bit_to";
    static const string cBitWidth = "bit_width";
    static const string cBus = "bus";
    static const string cBusType = "bus_type";
    static const string cDataType = "data_type";
    static const string cDirection = "direction";
    static const string cFF = "ff";
    static const string cFunction = "function";
    static const string cLatch = "latch";
    static const string cPin = "pin";
    static const string cTrue = "true";
    static const string cType = "type";

    static
    void
    error2(const TRcToken &tok) {
        error("SLF-PARSE-2", tok->getLocation(), tok->getText());
    }

    inline
    string
    rmEnclQuotes(const string &s) {
        return s.substr(1, s.size() - 2); //lose ""    
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

    TRcToken
    Parser::nextToken() {
        la(0);
        return accept();
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
                //initialize for library-scope busses
                m_libBusTypes = new t_busTypes();
                expectAccept(Token::eLCurly);
                while (Token::eRCurly != la(0)->getType()) {
                    pair<TRcLibraryEle, TRcValueSet> lset = libraryEle(lib);
                    TRcLibraryEle &lele = lset.first;
                    if (lele.isValid()) {
                        //TODO: put cells in lib and otherstuff???
                        if (LibraryEle::eKeyValue == lele->getType()) {
                            //Look for these keys
                            if (lele->getKey() == cType) {
                                TRcKeyValue asKv = KeyValue::downcast(lele);
                                addTypeGroup(m_libBusTypes, asKv);
                            }
                        } else {
                            ASSERT_TRUE(LibraryEle::eLibCell == lele->getType());
                            TRcLibCell lc = LibCell::downcast(lele);
                            createLibCell(lib, lc, lset.second);
                        }
                    }
                }
                expectAccept(Token::eRCurly);
            }
        } catch (unsigned cnt) {
            slurpUntil(Token::eEOF); //basic error recovery if we get this far
        }
        expect(Token::eEOF);
    }

    pair<TRcLibraryEle, TRcValueSet>
    Parser::libraryEle(TRcLibrary &lib) throw (unsigned) {
        TRcLibraryEle ele;
        TRcValueSet vset;
        Token::EType type = la(0)->getType();
        if (Token::eCell == type) {
            pair<TRcLibCell, TRcValueSet> libcell = cell(lib);
            ele = asLibraryEle(libcell.first);
            vset = libcell.second;
        } else if (Token::eRCurly != type) {
            TRcKeyValue keyval = keyValue();
            ele = asLibraryEle(keyval);
        } //else '}'
        return std::make_pair(ele, vset); //could be nil
    }

    pair<TRcLibCell, TRcValueSet>
    Parser::cell(TRcLibrary &lib) throw (unsigned) {
        // _ cell
        TRcLibCell libcell;
        expectAccept(Token::eCell);
        expectAccept(Token::eLParen);
        TRcToken tok = nextToken();
        string cellNm;
        if (Token::eIdent == tok->getType()) {
            cellNm = tok->getText();
        } else if (Token::eString == tok->getType()) {
            cellNm = rmEnclQuotes(tok->getText());
        } else {
            error2(tok);
            throw (++m_errCnt);
        }
        libcell = new LibCell(cellNm);
        expectAccept(Token::eRParen);
        TRcValueSet valset = valueSet();
#ifdef DEBUG   //enable to get dump of cell attributes
        dbgOs << "DBG: cell=" << cellNm << "{ ";
        dbgOs << valset << " }" << std::endl;
#endif
        return std::make_pair(libcell, valset);
    }

    TRcKeyValue
    Parser::keyValue() throw (unsigned) {
        TRcKeyValue keyval;
        TRcToken key = expectAccept(Token::eIdent);
        TRcToken tok = nextToken();
        TRcValueType valtype;
        if (Token::eColon == tok->getType()) {
            // COLON _ value_type (SEMI)?
            valtype = valueType();
            if (Token::eSemi == la(0)->getType()) {
                accept();
            }
            keyval = new KeyValue(key->getText(), valtype);
        } else if (Token::eLParen == tok->getType()) {
            // LPAREN _ (value_type_list)? RPAREN
            TRcValueTypeList valList;
            if (Token::eRParen != la(0)->getType()) {
                valList = valueTypeList();
            }
            expectAccept(Token::eRParen);
            // _ (valueSet | SEMI | nil)
            Token::EType type = la(0)->getType();
            TRcValueSet valset;
            if (Token::eLCurly == type) {
                valset = valueSet();
            } else if (Token::eSemi == type) {
                accept();
            } //else nil
            keyval = new KeyValue(key->getText(), valList, valset);
        } else {
            error2(tok);
            throw (++m_errCnt);
        }
        return keyval;
    }

    TRcValueSet
    Parser::valueSet() throw (unsigned) {
        TRcValueSet valueSet;
        ValueSet::t_keyValues vals;
        expectAccept(Token::eLCurly);
        while (Token::eRCurly != la(0)->getType()) {
            TRcKeyValue kv = keyValue();
            if (kv.isValid()) {
                vals.push_back(kv);
            }
        }
        expectAccept(Token::eRCurly);
        if (0 < vals.size()) {
            valueSet = new ValueSet(vals);
        }
        return valueSet;
    }

    TRcValueType
    Parser::valueType() throw (unsigned) {
        TRcValueType valt;
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
            valt = new ValueType(ex);
        } else {
            switch (types[0]) {
                case Token::eIdent: case Token::eCell: case Token::eLibrary: //kident
                {
                    TRcToken kident = accept();
                    TRcBus bs;
                    if (Token::eLBracket == types[1]) {
                        bs = bus();
                    }
                    valt = new ValueType(kident->getText(), bs);
                }
                    break;
                case Token::eString:
                {
                    TRcToken str = accept();
                    valt = new ValueType(rmEnclQuotes(str->getText()));
                }
                    break;
                case Token::eInteger: case Token::eFloat:
                {
                    TRcNumber num = number();
                    TRcToken unit;
                    if (Token::eIdent == types[1]) {
                        unit = accept();
                    }
                    string us = (unit.isNull()) ? "" : unit->getText();
                    valt = new ValueType(num, us);
                }
                    break;
                case Token::eTrue: case Token::eFalse:
                {
                    TRcToken boolVal = accept();
                    valt = new ValueType(boolVal->getText() == cTrue);
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
        return valt;
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
        TRcValueTypeList valList = new ValueTypeList();
        while (true) {
            TRcValueType valType = valueType();
            valList->push_back(valType);
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
        TRcToken n = accept(); // FLOAT | INTEGER
        const string s = n->getText();
        TRcNumber num = new Number(s,
                (n->getType() == Token::eFloat) ? Number::eFloat : Number::eInteger);
        return num;
    }

    void
    Parser::addTypeGroup(trc_busTypes &busses, const TRcKeyValue &kv) {
#ifdef DEBUG
        dbgOs << kv;
#endif
        if (!kv->hasValSet()) return;
        ValueSet::trc_kvByKey rkvByKey = kv->getValSet()->asMap();
        ValueSet::t_kvByKey &kvByKey = rkvByKey.asT();
        ASSERT_TRUE(cArray == kvByKey[cBaseType]->getVal()->asIdent());
        ASSERT_TRUE(cBit == kvByKey[cDataType]->getVal()->asIdent());
        long int msb = kvByKey[cBitFrom]->getVal()->asNumber()->asInt();
        long int lsb = kvByKey[cBitTo]->getVal()->asNumber()->asInt();
        long int n = kvByKey[cBitWidth]->getVal()->asNumber()->asInt();
        ASSERT_TRUE(n == (1 + ((msb > lsb) ? (msb - lsb) : (lsb - msb))));
        string typeNm = kv->getVal()->asIdent();
        TRcBus bus = new Bus(msb, lsb);
        busses.asT()[typeNm] = bus;
    }

    void
    Parser::createLibCell(TRcLibrary &lib, TRcLibCell &lcel, TRcValueSet &rcvset) {
        if (rcvset.isNull()) return;
#ifdef DEBUG
        dbgOs << "DBG: createLibCell (" << lcel->getName() << ") {" << std::endl;
        dbgOs << rcvset << " }" << std::endl;
#endif
        ASSERT_TRUE(!lib->hasModule(lcel->getName()));
        {
            vnl::TRcModule mod = asModule(lcel);
            lib->add(mod);
        }
        //Need to walk linearly, since need to retain port order
        //and can get repeat keys, like bus,...
        const ValueSet &vset = rcvset.asT();
        for (unsigned i = 0; i < vset.length(); i++) {
            const TRcKeyValue &kv = vset[i];
            const string &key = kv->getKey();
            if (key == cArea) {
                lcel->setArea(kv->getVal()->asNumber()->asDouble());
            } else if (key == cBus) {
                const string &portNm = kv->getVal()->asIdent();
                ASSERT_TRUE(kv->hasValSet());
                //get as map, allow dups, since bus expanded into pin(s)
#ifdef xDEBUG
                dbgOs << "DBG: port(" << portNm << ") {" << std::endl;
                dbgOs << kv->getValSet() << " }" << std::endl;
#endif
                ValueSet::trc_kvByKey rcvbk = kv->getValSet()->asMap(true);
                ValueSet::t_kvByKey &vbk = rcvbk.asT();
                ASSERT_TRUE(m_libBusTypes.isValid());
                const string &busType = vbk[cBusType]->getVal()->asIdent();
                ASSERT_TRUE(mapHasKey(m_libBusTypes.asT(), busType));
                TRcBus bus = m_libBusTypes.asT()[busType];
                const string &direction = vbk[cDirection]->getVal()->asIdent();
                lcel->addPort(portNm, bus, direction);
            } else if (key == cPin) {
                const string &portNm = kv->getVal()->asIdent();
#ifdef DEBUG
                dbgOs << "DBG: pin(" << portNm << ") {" << std::endl;
                dbgOs << kv->getVal() << std::endl;
                dbgOs << kv->getValSet() << " }" << std::endl;
#endif
                ASSERT_TRUE(kv->hasValSet());
                //allow dups for timing keys
                ValueSet::trc_kvByKey rcvbk = kv->getValSet()->asMap(true);
                ValueSet::t_kvByKey &vbk = rcvbk.asT();
                const string &direction = vbk[cDirection]/*rcvbk->find(cDirection)->second*/->getVal()->asIdent();
                lcel->addPort(portNm, direction);
                //TODO: pin unateness
            } else if (key == cFF) {
                lcel->setCellType(LibCell::eFF);
            } else if (key == cLatch) {
                lcel->setCellType(LibCell::eLatch);
            } else if (key == cFunction) {
                lcel->setFunction(kv->getVal()->asString());
            }
        }
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

