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
#include "vnl/bus.hxx"

namespace vnl {
    using std::ostringstream;
    static string stTypeName = "bus";
    const unsigned Bus::stTypeId = Object::getNextTypeId(stTypeName);

    Bus::Bus(const TRcBus &bus)
    : m_lb(bus->m_lb), m_rb(bus->m_rb) {
    }

    bool
    Bus::inBounds(unsigned i) const {
        if (0 < getIncr()) {
            return (m_lb <= i) && (i <= m_rb);
        } else if (0 > getIncr()) {
            return (m_lb >= i) && (i >= m_rb);
        } else {
            return i == m_lb;
        }
    }

    unsigned
    Bus::getLbOffset(unsigned i) const {
        ASSERT_TRUE(inBounds(i));
        return (m_lb < m_rb) ? (i - m_lb) : (m_lb - i);
    }

    bool
    Bus::inRange(unsigned i) const {
        bool ok;
        if (0 < getIncr()) {
            //ascending bus
            ok = (getLb() <= i) && (getRb() >= i);
        } else {
            ok = (getLb() >= i) && (getRb() <= i);
        }
        return ok;
    }

    bool
    Bus::inRange(const TRcBus &b) const {
        bool ok = inRange(b->getLb()) && inRange(b->getRb());
        return ok;
    }

    string Bus::toString(string base) const {
        ostringstream oss;
        oss << base << '[' << getLb();
        if (hasRb()) oss << ':' << getRb();
        oss << ']';
        return oss.str();
    }

    Bus::~Bus() {
    }

    bool
    operator ==(const TRcBus &a, const TRcBus &b) {
        return (a->isScalar() == b->isScalar()) &&
                (a->getLb() == b->getLb()) &&
                (a->getRb() == b->getRb());
    }
}

