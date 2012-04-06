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
#include <vector>
#include "xyzzy/assert.hxx"
#include "vnl/parser.hxx"
#include "vnl/message.hxx"
#include "vnl/lexer.hxx"
#include "vnl/token.hxx"
#include "vnl/port.hxx"
#include "vnl/portbus.hxx"
#include "vnl/wire.hxx"
#include "vnl/wirebus.hxx"
#include "vnl/constref.hxx"
#include "vnl/wirebitref.hxx"

namespace vnl {
    using std::stringstream;
    using std::ostringstream;
    using std::vector;
    using std::hex;
    using std::oct;

    static Port::EDirection
    getDirection(Token::EType type) {
        Port::EDirection dir = Port::eUnknown;
        switch (type) {
            case Token::eInput: dir = Port::eIn;
                break;
            case Token::eOutput: dir = Port::eOut;
                break;
            case Token::eInout: dir = Port::eInout;
                break;
            default: ASSERT_NEVER;
        }
        return dir;
    }

    static void
    createEntry(TRcConnsByPortName &connsByName, string pin, TRcConnList &conns) {
        const unsigned n = conns.isValid() ? conns->size() : 0;
        TBitConns car(n);
        if (0 < n) {
            TConnList::iterator iter = conns->begin();
            //lb->rb (msb->lsb)
            for (unsigned i = 0; iter != conns->end(); ++iter, i++) {
                car[i] = *iter;
            }
        }
        connsByName->operator[](pin) = car;
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
            error("VNL-PARSE-1", got->getLocation(), Lexer::asString(type),
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

    void
    Parser::start(TRcLibrary &lib) {
        // moduleDeclaration*
        TRcToken tok;
        while (la(0)->getType() == Token::eModule) {
            TRcModule mod = moduleDeclaration(lib);
        }
        expect(Token::eEOF);
    }

    TRcToken
    Parser::expectAccept(EType type) throw (unsigned) {
        if (expect(type)) {
            return accept();
        }
        throw (++m_errCnt);
    }

    TRcModule
    Parser::moduleDeclaration(TRcLibrary &lib) {
        // _ module
        TRcModule mod;
        try {
            accept();
            TRcToken modName = expectAccept(Token::eIdent);
            mod = new Module(modName->getText());
            if (!lib->add(mod)) {
                error("VNL-MOD-1", modName->getLocation(), mod->getName(), lib->getName());
                throw (m_errCnt);
            }
            // module ident _
            if (la(0)->isType(Token::eLParen)) {
                accept();
                portDeclarations(mod);
            }
            // port_decl _ ;
            expectAccept(Token::eSemi);
            moduleItems(mod);
            expectAccept(Token::eEndModule);
        } catch (unsigned cnt) {
            slurpUntil(Token::eEndModule);
        }
        return mod;
    }

    void
    Parser::portDeclarations(TRcModule &mod) throw (unsigned) {
        // module ident ( _
        //TODO: need to add ansi style
        TRcPort port;
        TRcToken tok;
        string nm;
        bool stay = true;
        while (true) {
            if (la(0)->isType(Token::eRParen)) break; //while
            tok = expectAccept(Token::eIdent);
            nm = tok->getText();
            if (mod->hasPort(nm)) {
                error("VNL-DECL-1", tok->getLocation(), "port", nm);
            } else {
                mod->appendPortDecl(nm);
            }
            // ident _ [,)]
            if (la(0)->isType(Token::eComma)) {
                accept();
            }
        }
        expectAccept(Token::eRParen); //) _
    }

    void
    Parser::moduleItems(TRcModule &mod) throw (unsigned) {
        TRcToken tok;
        EType type;
        bool stay = true;
        while (stay) {
            tok = la(0);
            type = tok->getType();
            switch (type) {
                case Token::eInput: case Token::eOutput: case Token::eInout:
                case Token::eWire:
                    wireDefinition(mod);
                    break;
                case Token::eIdent:
                    moduleInstantiation(mod);
                    break;
                case Token::eAssign:
                    assignStatement(mod);
                    break;
                case Token::eEndModule:
                    stay = false;
                    break;
                default:
                    error("VNL-PARSE-2", tok->getLocation(), tok->getText());
                    throw (++m_errCnt);
            }
        }
        expect(Token::eEndModule);
    }

    void
    Parser::wireDefinition(TRcModule &mod) throw (unsigned) {
        // _ input|output|inout|wire
        TRcToken tok = la(0);
        Port::EDirection dir = Port::eUnknown;
        bool isPortDefn = !tok->isType(Token::eWire);
        if (isPortDefn) {
            dir = getDirection(tok->getType());
        }
        accept();
        TRcBus rng;
        TRcWire asWire;
        if (la(0)->isType(Token::eLBracket)) {
            rng = range();
        }
        // [n:m]? _ ident (,ident)*
        for (bool stay = true; stay;) {
            tok = expectAccept(Token::eIdent);
            string nm = tok->getText();
            if (isPortDefn) {
                TRcPort asPort;
                if (mod->hasPort(nm, &asPort)) {
                    if (asPort->getDirection() != Port::eUnknown) {
                        error("VNL-PORT-2", tok->getLocation(), nm);
                    } else if (rng.isNull()) {
                        //scalar, so just change direction
                        asPort->set(dir);
                    } else {
                        //change to bus
                        TRcPortBus pbus = new PortBus(nm, dir, rng);
                        asWire = toWire(pbus);
                        mod->replace(asPort, asWire);
                    }
                } else {
                    error("VNL-PORT-1", tok->getLocation(), nm);
                }
            } else {
                if (mod->hasWire(nm, &asWire)) {
                    error("VNL-WIRE-2", tok->getLocation(), nm);
                } else {
                    if (rng.isValid()) {
                        TRcWireBus wbus = new WireBus(nm, rng);
                        asWire = toWire(wbus);
                    } else {
                        asWire = new Wire(nm);
                    }
                    mod->addWire(asWire);
                }
            }
            tok = la(0);
            if (tok->isType(Token::eComma)) {
                accept();
            } else {
                stay = !tok->isType(Token::eSemi);
            }
        }
        expectAccept(Token::eSemi);
    }

    TRcBus
    Parser::range() throw (unsigned) {
        // _ [ n (:m)? ]
        expectAccept(Token::eLBracket);
        TRcToken tok = expectAccept(Token::eUint);
        unsigned lb = toUnsigned(tok);
        unsigned rb = Bus::stNotUsed;
        if (la(0)->isType(Token::eColon)) {
            accept();
            tok = expectAccept(Token::eUint);
            rb = toUnsigned(tok);
        }
        expectAccept(Token::eRBracket);
        return new Bus(lb, rb);
    }

    void
    Parser::moduleInstantiation(TRcModule &mod) throw (unsigned) {
        // _ ident
        TRcToken refNm = expectAccept(Token::eIdent);
        for (bool stay = true; stay;) {
            TRcToken instNm = expectAccept(Token::eIdent);
            expectAccept(Token::eLParen);
            moduleInstantiation(mod, refNm, instNm);
            if (la(0)->isType(Token::eComma)) {
                accept(1);
            } else {
                stay = false;
            }
        }
        expectAccept(Token::eSemi);
    }

    void
    Parser::moduleInstantiation(TRcModule &mod, TRcToken &refNm,
            TRcToken &instNm) throw (unsigned) {
        // refNm instNm ( _
        //TODO add by position
        /*For now, just do by pin name
         * Create map of connections by pin name.
         */
        TRcConnsByPortName connsByPortName = new TConnsByPortName();
        for (bool stay = true; stay;) {
            TRcToken tok = la(0);
            if (tok->isType(Token::eComma)) {
                accept(1);
            } else if (tok->isType(Token::eRParen)) {
                accept(1);
                stay = false;
            } else {
                expectAccept(Token::eDot);
                TRcToken pinNm = expectAccept(Token::eIdent);
                // .ident _ ( <connections> )
                expectAccept(Token::eLParen);
                TRcConnList conns = connections(mod);
                expectAccept(Token::eRParen);
                // .ident(...) _
                //reconcile and update connsByPinName
                //TODO: Do pinRef onto wire at link time?

                string pnm = pinNm->getText();
                if (connsByPortName->end() != connsByPortName->find(pnm)) {
                    error("VNL-CELL-1", pinNm->getLocation(), pnm);
                } else {
                    createEntry(connsByPortName, pnm, conns);
                }
            }
        }
        string inm = instNm->getText();
        if (mod->hasCell(inm)) {
            error("VNL-CELL-2", instNm->getLocation(), inm);
        } else {
            TRcCell cell = new Cell(refNm->getText(), inm, connsByPortName);
            Cell::updateWires(cell);
            mod->addCell(cell);
        }
    }

    TRcConnList
    Parser::connections(TRcModule &mod) throw (unsigned) {
        TRcConnList items;
        for (bool stay = true; stay;) {
            if (la(0)->isType(Token::eRParen)) {
                stay = false;
            } else {
                TRcConnList nitems = expression(mod);
                if (nitems.isValid()) {
                    if (items.isNull()) {
                        items = new TConnList();
                    }
                    items->splice(items->end(), nitems.asT());
                }
                if (la(0)->isType(Token::eComma)) {
                    accept();
                }
            }
        }
        return items;
    }

    TRcConnList
    Parser::expression(TRcModule &mod) throw (unsigned) {
        /*
         * expression: primary
         *           | concatenation
         */
        TRcConnList nitems;
        TRcToken tok = la(0);
        switch (tok->getType()) {
            case Token::eUint: case Token::eBasedNumber:
            case Token::eIdent:
                nitems = primary(mod);
                break;
            case Token::eLCurly:
                nitems = concatentation(mod);
                break;
            default:
                error("VNL-PARSE-2", tok->getLocation(), tok->getText());
                throw (++m_errCnt);
        }
        return nitems;
    }

    TRcConnList
    Parser::primary(TRcModule &mod) throw (unsigned) {
        /*
         * primary: uint? based
         *        | ident range?
         */
        TRcConnList item;
        TRcToken tok = la(0);
        if (!tok->isType(Token::eIdent)) {
            TRcToken uint, based;
            if (tok->isType(Token::eUint)) {
                uint = accept(1);
            }
            based = expectAccept(Token::eBasedNumber);
            item = toBits(uint, based, mod);
        } else {
            // _ ident range?
            tok = expectAccept(Token::eIdent);
            TRcBus rng;
            if (la(0)->isType(Token::eLBracket)) {
                rng = range();
            }
            TRcWire w = mod->getWire(tok->getText());
            //TODO: implicit wires: add if not declared
            if (w.isNull()) {
                //error("VNL-DECL-2", tok->getLocation(), tok->getText());
                w = new Wire(tok->getText());
                mod->addWire(w);
            } 
            if (w->isBus()) {
                TRcWireBus asBus = toWireBus(w);
                if (rng.isValid() && !asBus->inRange(rng)) {
                    error("VNL-BUS-1", tok->getLocation(), rng->toString(w->getName()));
                } else {
                    item = WireBitRef::bitBlast(asBus, rng);
                }
            } else {
                //scalar wire
                item = new TConnList();
                item->push_back(upcast(w));
            }
        }
        return item;
    }

    TRcConnList
    Parser::concatentation(TRcModule &mod) throw (unsigned) {
        /*
         * concatenation: { expr (, expr)* }
         *              | { uint concatenation }     //repeated concat
         */
        TRcConnList concat;
        expectAccept(Token::eLCurly);
        if (la(0)->isType(Token::eUint) && la(1)->isType(Token::eLCurly)) {
            // _ uint concatenation
            TRcToken uint = expectAccept(Token::eUint);
            unsigned repeat;
            if (!uint->toUnsigned(repeat)) {
                error("VNL-NUM-1", uint->getLocation(), uint->getText());
                throw (++m_errCnt);
            }
            TRcConnList c = concatentation(mod);
            //repeat
            if (0 < repeat) {
                concat = new TConnList();
                for (unsigned i = 0; i < repeat; i++) {
                    TConnList toCopy(c.asT());
                    concat->splice(concat->end(), toCopy);
                }
            }
        } else if (!la(0)->isType(Token::eRCurly)) {
            // _ expr (, expr)*
            for (bool stay = true; stay;) {
                TRcConnList c = expression(mod);
                if (concat.isNull()) {
                    concat = new TConnList();
                }
                concat->splice(concat->end(), c.asT());
                if (la(0)->isType(Token::eComma)) {
                    accept(1);
                } else {
                    stay = false;
                }
            }
        }
        expectAccept(Token::eRCurly);

        return concat;
    }

    static void
    addTo(TRcObject &lhs, TRcObject &rhs) {
        if (lhs->isType(Wire::stTypeId)) {
            TRcWire wire = Wire::downcast(lhs);
            wire->add(rhs);
        } else if (lhs->isType(Port::stTypeId)) {
            TRcPort port = Port::downcast(lhs);
            port->add(rhs);
        } else {
            ASSERT_TRUE(lhs->isType(WireBitRef::stTypeId));
            TRcWireBitRef wireBitRef = WireBitRef::downcast(lhs);
            wireBitRef->add(rhs);
        }
    }

    /**
     * Expand assign statement by updating lhs.conns with entry from rhs.
     * @param lhs entries to update.
     * @param rhs update lhs w/ corresponding entry.
     */
    static void
    expand(TRcConnList &lhs, TRcConnList &rhs) {
        ASSERT_TRUE(lhs->size() == rhs->size());
        TRcObject obj[2];
        TConnList::iterator iter[2] = {lhs->begin(), rhs->begin()};
        for (; iter[0] != lhs->end(); ++iter[0], ++iter[1]) {
            obj[0] = *(iter[0]);
            obj[1] = *(iter[1]);
            addTo(obj[0], obj[1]);
            //TODO: do we want to add lhs to rhs?
            //if so, just addTo(obj[1],obj[0]);
        }
    }

    static string
    toString(unsigned i) {
        ostringstream oss;
        oss << i;
        return oss.str();
    }

    void
    Parser::assignStatement(TRcModule &mod) throw (unsigned) {
        // _ assign
        TRcToken loc = expectAccept(Token::eAssign);
        TRcConnList lhs = expression(mod);
        expectAccept(Token::eEq);
        TRcConnList rhs = expression(mod);
        expectAccept(Token::eSemi);
        unsigned n[2] = {lhs->size(), rhs->size()};
        if (n[0] != n[1]) {
            error("VNL-ASSN-1", loc->getLocation(), toString(n[0]), toString(n[1]));
            throw (++m_errCnt);
        } else {

            expand(lhs, rhs);
        }
    }

    unsigned
    Parser::toUnsigned(TRcToken &tok) throw (unsigned) {
        unsigned uint;
        if (!tok->toUnsigned(uint)) {

            error("VNL-NUM-1", tok->getLocation(), tok->getText());
            throw (++m_errCnt);
        }
        return uint;
    }

    TRcConnList
    Parser::toBits(TRcToken &size, TRcToken &based, TRcModule &mod) throw (unsigned) {
        TRcConstRef cref;
        if (size.isNull()) {
            string s = based->getText();
            bool ok = (s.length() == 3); //'[bdoh]v
            if (ok) {
                ok = (s[2] == '1' || s[2] == '0');
            }
            if (!ok) {
                error("VNL-NUM-2", based->getLocation(), s);
                throw (++m_errCnt);
            }
        }
        try {
            if (size.isNull()) {
                cref = new ConstRef(based);
            } else {
                cref = new ConstRef(size, based);
            }

        } catch (unsigned err) { //remap 1 and 2
            TRcLocation loc = (1 == err) ? size->getLocation() : based->getLocation();
            string text = (1 == err) ? size->getText() : based->getText();
            error("VNL-NUM-1", loc, text);
            throw (++m_errCnt);
        }
        //cref[0] is LSB
        TRcConnList clist = new TConnList();
        TRcWire w;
        for (unsigned i = 0; i < cref->length(); i++) {

            w = mod->getConstWire(cref->at(i));
            clist->push_front(upcast(w)); //lsb->msb (msb at begin())
        }
        return clist;
    }

    Parser::~Parser() {
    }

}

//#define T_PARSER
#if defined(T_PARSER)

#include <iostream>
#include "vnl/parser.hxx"
#include "writer.hxx"

using namespace std;
using namespace vnl;

int main(int argc, char *argv[]) {
    ASSERT_TRUE(3 <= argc);
    string topNm = argv[1];
    TRcToken tok;
    TRcLocation loc;
    TRcLexer lexer;
    TRcLibrary work = new Library();
    for (int i = 2; i < argc; i++) {
        lexer = new Lexer(argv[i]);
        cout << "Info: " << lexer->getFname() << ": processing ..." << endl;
        Parser parser(lexer);
        parser.start(work);
#ifdef DEBUG
        TRcWriter writer = new Writer(string("writer.out.v"));
        const TRcModule &mod = work->getModule(topNm);
        writer->write(mod);
#endif
    }
    return 0;
}
#endif
