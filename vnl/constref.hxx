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
#if !defined(_vnl_constref_hxx_)
#define  _vnl_constref_hxx_

#include "xyzzy/array.hxx"
#include "vnl/vnl.hxx"
#include "vnl/token.hxx"

namespace vnl {
    using xyzzy::PTArray;

    /**
     * Wrapper around constant value.
     */
    class ConstRef {
    public:
        /**
         * Construct based number.
         * @param size size of based number.
         * @param basedNumber based number.
         * @throw 1 for size has NUM-1 error; 2 for basedNumber has NUM-1 error.
         */
        explicit ConstRef(TRcToken size, TRcToken basedNumber) throw (unsigned);

        /**
         * Construct based number.
         * @param basedNumber based number.
         * @throw 2 for basedNumber has NUM-1 error.
         */
        explicit ConstRef(TRcToken basedNumber) throw (unsigned);

        string toString() const;

        bool operator [](unsigned i) const {
            return at(i);
        }

        bool at(unsigned i) const {
            return m_bits[i];
        }
        
        unsigned length() const {
            return m_bits.length();
        }

        virtual ~ConstRef();

    private:
        PTArray<bool> m_bits;

        /**
         * Initialize based number.
         * @param basedNumber number to convert.
         * @param n number of bits.
         * @throw 1 or 2 (see above constructors) on malformed number.
         */
        void init(TRcToken basedNumber, unsigned n = 1) throw (unsigned);

        //Not allowed
        COPY_CONSTRUCTOR_DECL(ConstRef);
    };
}

#endif
