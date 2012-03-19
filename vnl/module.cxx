/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2006-2010  Karl W. Pfalzer
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "xyzzy/assert.hxx"
#include "vnl/module.hxx"

namespace vnl {

#define UNCONST const_cast<Module*>(this)

    static string stTypeName = "module";
    const unsigned Module::stTypeId = Object::getNextTypeId(stTypeName);

    Module::Module(string nm)
    : m_name(nm) {
        //add constant wires
        TRcWire w;
        for (int i = 0; i < 2; i++) {
            w = new Wire(Wire::stConstNm[i]);
            addWire(w);
        }
    }

    void
    Module::appendPortDecl(string nm) {
        ASSERT_TRUE(!hasPort(nm));
        m_portDeclOrder.push_back(nm);
        TRcPort port = new Port(nm);
        TRcWire asWire = toWire(port);
        m_wiresByName[nm] = asWire;
    }

    TRcWire
    Module::getWire(string nm) {
        TRcWire asWire;
        t_wiresByName::const_iterator where = m_wiresByName.find(nm);
        if (m_wiresByName.end() != where) {
            asWire = where->second;
        }
        return asWire;
    }

    const TRcWire
    Module::getWire(string nm) const {
        return UNCONST->getWire(nm);
    }

    bool
    Module::hasPort(string nm, TRcPort *pport) {
        TRcWire asWire = getWire(nm);
        bool ok = asWire.isValid();
        if (ok) {
            ok &= asWire->isPort();
            if (ok && (0 != pport)) {
                *pport = toPort(asWire);
            }
        }
        return ok;
    }

    bool
    Module::hasPort(string nm, const TRcPort *pport) const {
        return UNCONST->hasPort(nm, const_cast<TRcPort*> (pport));
    }

    bool
    Module::hasWire(string nm, TRcWire *pwire) {
        TRcWire asWire = getWire(nm);
        bool ok = asWire.isValid();
        if (ok) {
            ok &= !asWire->isPort();
            if (ok && (0 != pwire)) {
                *pwire = asWire;
            }
        }
        return ok;
    }

    bool
    Module::hasWire(string nm, const TRcWire *pwire) const {
        return UNCONST->hasWire(nm, const_cast<TRcWire*> (pwire));
    }

    TRcPort
    Module::getPort(string nm) {
        TRcPort port;
        hasPort(nm, &port);
        return port;
    }

    const TRcPort
    Module::getPort(string nm) const {
        return UNCONST->getPort(nm);
    }

    void
    Module::getPortsInDeclOrder(t_ports &ports) const {
        ASSERT_TRUE(ports.empty());
        TRcPort port;
        for (t_portDeclOrder::const_iterator iter = m_portDeclOrder.begin();
                iter != m_portDeclOrder.end(); ++iter) {
            port = getPort(*iter);
            ports.push_back(port);
        }
    }

    void
    Module::replace(TRcPort &from, TRcWire &to) {
        string nm = to->getName();
        ASSERT_TRUE(from->getName() == nm);
        t_wiresByName::iterator where = m_wiresByName.find(nm);
        ASSERT_TRUE(where != m_wiresByName.end());
        m_wiresByName.erase(where);
        m_wiresByName[nm] = to;
    }

    void
    Module::addWire(TRcWire &wire) {
        string nm = wire->getName();
        ASSERT_TRUE(!hasWire(nm));
        m_wiresByName[nm] = wire;
    }

    bool
    Module::hasCell(string nm) const {
        return (m_cellsByName.end() != m_cellsByName.find(nm));
    }

    void
    Module::addCell(TRcCell &cell) {
        string nm = cell->getInstName();
        ASSERT_TRUE(!hasCell(nm));
        m_cellsByName[nm] = cell;
    }

    Module::~Module() {
    }

#undef UNCONST
};
