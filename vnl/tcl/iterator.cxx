//The MIT License
//
//vnl - verilog netlist
//Copyright (c) 2006-2010  Karl W. Pfalzer
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
#include "vnl/tcl/iterator.hxx"

namespace vnltcl {
    unsigned Iterator::stTypeId = Object::getNextTypeId("iterator");

    Iterator Iterator::stOneOf;
    
    /*
     * NOTE: This constructor is used just for the private stOneOf.
     * As shwon, the iterator itself is pure garbage; but, we need an
     * initializer for the iterator member, so viola...
     */
    Iterator::Iterator()
    : m_iter(m_iter) {
    }

    TRcObject& 
    Iterator::getNext() {
        static TRcObject stNullObj;
        TRcObject &rval = stNullObj;
        while (m_iter.hasMore()) {
            rval = *m_iter++;
            if (rval.isValid()) {
                return rval;
            }
        }
        return stNullObj;
    }

    Iterator::Iterator(TRcCollection &coll)
    : m_iter(coll.asT()) {
        //TODO setup 1st value
    }

    unsigned
    Iterator::getTypeId() const {
        return stTypeId;
    }
}
