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
#include "vnl/wirebus.hxx"

namespace vnl {

#define UNCONST const_cast<WireBus*>(this)

    static string stTypeName = "wirebus";
    const unsigned WireBus::stTypeId = Object::getNextTypeId(stTypeName);

    WireBus::WireBus(string name, const TRcBus &bus)
    : Wire(name), Bus(bus), m_pinConns(bus->getLength() - 1) {
    }

    TRcConnList
    WireBus::getConnList(unsigned i, bool create) {
        unsigned lbo = Bus::getLbOffset(i);
        TRcConnList clist = (0 == lbo) ? Wire::m_conns : m_pinConns[lbo - 1];
        if (create && clist.isNull()) {
            clist = new TConnList();
            if (0 == lbo) {
                Wire::m_conns = clist;
            } else {
                m_pinConns[lbo - 1] = clist;
            }
        }
        return clist;
    }

    TRcConnList
    WireBus::operator [](unsigned i) {
        return getConnList(i, true);
    }

    const TRcConnList
    WireBus::operator [](unsigned i) const {
        return UNCONST->getConnList(i);
    }

    ostream&
    WireBus::operator<<(ostream &os) const {
        os << "wire " << Bus::toString() << " " << getName();
        return os;
    }

    string
    WireBus::getName(bool full) const {
        string s = Wire::getName();
        if (full) {
            s += Bus::getName();
        }
        return s;
    }
    
    WireBus::~WireBus() {
    }

#undef UNCONST
}
