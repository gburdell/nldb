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

#include "vnl/wire.hxx"

namespace vnl {
    static string stTypeName = "wire";
    const unsigned Wire::stTypeId = Object::getNextTypeId(stTypeName);

    const string Wire::stConstNm[2] = {"1'b0", "1'b1"};

    ostream&
    Wire::operator<<(ostream &os) const {
        os << "wire " << getName();
        return os;
    }

    void
    Wire::add(TRcObject conn) {
        if (m_conns.isNull()) {
            m_conns = new TConnList();
        }
        m_conns->push_back(conn);
    }

    void
    Wire::add(TRcPinRef &conn) {
        add(upcast(conn));
    }

    void
    Wire::add(TRcWire &conn) {
        add(upcast(conn));
    }

    Wire::~Wire() {
    }
};
