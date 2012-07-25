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
#if !defined(_vnl_bus_hxx_)
#define  _vnl_bus_hxx_

#include "vnl/vnl.hxx"

namespace vnl {

    class Bus : virtual public Object {
    public:

        explicit Bus(unsigned lb) : m_lb(lb), m_rb(stNotUsed) {
        }

        explicit Bus(unsigned lb, unsigned rb) : m_lb(lb), m_rb(rb) {
        }

        explicit Bus(const TRcBus &bus);

        bool hasRb() const {
            return (m_rb != stNotUsed);
        }

        bool isScalar() const {
            return !hasRb();
        }

        /**
         * Get increment to iterate lb->rb.
         * @return 1 for ascending; -1 for descending; 0 for lb==rb.
         */
        int getIncr() const {
            return hasRb() ? ((m_lb < m_rb) ? 1 : -1) : 0;
        }

        unsigned getLb() const {
            return m_lb;
        }

        unsigned getMsb() const {
            return getLb();
        }
        
        unsigned getRb() const {
            return hasRb() ? m_rb : m_lb;
        }
        
        unsigned getLsb() const {
            return getRb();
        }

        unsigned getLength() const {
            return hasRb() ? length(m_lb, m_rb) : 1;
        }

        /**
         * Get actual bit index [lb...i...rb]
         * @param lbo left-bound offset.
         * @return actual bit index.
         */
        unsigned getActualBit(unsigned lbo) const {
            return getLb() + (lbo * getIncr());
        }
        
        string toString(string nm = "") const;
        
        string getName(bool unsed = false) const {
            return toString();
        }
        
        /**
         * Check if bus is in range by this bus.
         * @param b bus to check.
         * @return if bus in range.
         */
        bool inRange(const TRcBus &b) const;
        
        static const TRcBus downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, Bus > (r);
        }
        static TRcBus downcast(TRcObject &r) {
            return xyzzy::downcast<Object, Bus > (r);
        }

        virtual ~Bus();

        static const unsigned stNotUsed = ~(0u);

    protected:

        bool inBounds(unsigned i) const;

        /**
         * Get offset from left bound.
         * @param i index [lb...i...rb]
         * @return left bound offset.
         */
        unsigned getLbOffset(unsigned i) const;

    private:
        unsigned m_lb, m_rb;

        bool inRange(unsigned ix) const;
        
        //Not allowed
        COPY_CONSTRUCTOR_DECL(Bus);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;
    };

    inline const TRcObject upcast(const TRcBus &p) {
        return xyzzy::upcast<Object, Bus > (p);
    }
    inline TRcObject upcast(TRcBus &p) {
        return xyzzy::upcast<Object, Bus > (p);
    }

    bool operator ==(const TRcBus &a, const TRcBus &b);

    inline bool operator !=(const TRcBus &a, const TRcBus &b) {
        return !(a == b);
    }

}


#endif
