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
#include <sstream>
#include "xyzzy/assert.hxx"
#include "vnl/constref.hxx"

namespace vnl {
    using std::ostringstream;

    ConstRef::ConstRef(TRcToken basedNumber) throw (unsigned) {
        init(basedNumber);
    }

    ConstRef::ConstRef(TRcToken size, TRcToken basedNumber) throw (unsigned) {
        unsigned n;
        if (! size->toUnsigned(n)) {
            throw (1);
        }
        init(basedNumber, n);
    }

    void
    ConstRef::init(TRcToken basedNumber, unsigned n) throw (unsigned) {
        m_bits.resize(n);
        if (! basedNumber->toBits(m_bits)) {
            throw (2);
        }
    }

    string
    ConstRef::toString() const {
        ostringstream oss;
        oss << m_bits.length() << "'b";
        for (unsigned i = 0; i < m_bits.length(); i++) {
            oss << (m_bits[i] ? '1' : '0');
        }
        return oss.str();
    }

    ConstRef::~ConstRef() {
    }

}
