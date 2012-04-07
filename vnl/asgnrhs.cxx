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

#include "vnl/asgnrhs.hxx"
#include "vnl/port.hxx"
#include "vnl/wirebitref.hxx"

namespace vnl {
    static string stTypeName = "asgnrhs";
    const unsigned AsgnRhs::stTypeId = Object::getNextTypeId(stTypeName);

    AsgnRhs::AsgnRhs(const TRcPort &rhs) {
        m_ele = upcast(rhs);
    }

    AsgnRhs::AsgnRhs(const TRcWire &rhs) {
        m_ele = upcast(rhs);
    }

    AsgnRhs::AsgnRhs(const TRcWireBitRef &rhs) {
        m_ele = upcast(rhs);
    }

    AsgnRhs::AsgnRhs(const TRcObject &rhs) {
        const unsigned type = rhs->getTypeId();
        if (Wire::stTypeId == type || type == Port::stTypeId || type == WireBitRef::stTypeId) {
            m_ele = rhs;
        } else {
            ASSERT_NEVER;
        }
    }

    AsgnRhs::~AsgnRhs() {
    }
};
