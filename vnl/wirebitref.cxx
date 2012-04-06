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
#include "vnl/wirebitref.hxx"

namespace vnl {
    using std::ostringstream;

    static string stTypeName = "wirebitref";
    const unsigned WireBitRef::stTypeId = Object::getNextTypeId(stTypeName);

    WireBitRef::WireBitRef(TRcWireBus &wire, unsigned bitIx)
    : m_wireBus(wire), m_bitIx(bitIx) {
    }

    string
    WireBitRef::toString() const {
        ostringstream oss;
        oss << getBusName() << '[' << m_bitIx << ']';
        return oss.str();
    }

    TRcConnList
    WireBitRef::bitBlast(TRcWireBus &bus, const TRcBus &rng) {
        TRcBus range = rng;
        TRcConnList bits = new TConnList();
        TRcWireBitRef wref;
        TRcObject asObj;
        if (range.isNull()) {
            range = new Bus(bus->getLb(), bus->getRb());
        }
        for (unsigned i = range->getLb(), j = 0; j < range->getLength();
                j++, i += range->getIncr()) {
            wref = new WireBitRef(bus, i);
            asObj = upcast(wref);
            bits->push_back(asObj);
        }
        return bits;
    }

    TRcConnList
    WireBitRef::bitBlast(TRcWireBus &bus) {
        TRcBus rng;     //pass in null range
        return bitBlast(bus, rng);
    }

    WireBitRef::~WireBitRef() {
    }
}
