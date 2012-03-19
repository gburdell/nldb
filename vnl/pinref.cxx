/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2006-2010  Karl W. Pfalzer
 * Copyright (c) 2012-      George P. Burdell
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
#include <sstream>
#include "xyzzy/assert.hxx"
#include "vnl/pinref.hxx"
#include "vnl/module.hxx"
#include "vnl/portbus.hxx"

namespace vnl {

#define UNCONST const_cast<PinRef*>(this)

    using std::ostringstream;

    static string stTypeName = "pinref";
    const unsigned PinRef::stTypeId = Object::getNextTypeId(stTypeName);

    PinRef::PinRef(TRcCell &cell, string portNm, unsigned lbOffset)
    : m_cell(cell), m_portNm(portNm), m_lbOffset(lbOffset) {
    }

    TRcPort
    PinRef::getPort() {
        TRcObject obj = m_cell->getRef();
        ASSERT_TRUE(obj.isValid());
        TRcModule ref = Module::downcast(obj);
        TRcPort port = ref->getPort(getPortNm());
        return port;
    }

    const TRcPort
    PinRef::getPort() const {
        return UNCONST->getPort();
    }

    string
    PinRef::getBitNm() const {
        ostringstream oss;
        oss << getPortNm();
        TRcPort port = getPort();
        if (port->isBus()) {
            TRcPortBus bus = PortBus::downcast(port);
            unsigned actualBit = bus->getActualBit(m_lbOffset);
            oss << '[' << actualBit << ']';
        }
        return oss.str();
    }

    string
    PinRef::getFullName(string sep) const {
        string s = getCellNm() + sep + getBitNm();
        return s;
    }

    PinRef::~PinRef() {
    }

#undef UNCONST
}
