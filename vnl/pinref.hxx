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
#if !defined(_vnl_pinref_hxx_)
#define  _vnl_pinref_hxx_

#include "vnl/vnl.hxx"
#include "vnl/cell.hxx"

namespace vnl {

    /**
     * Named instance.port[bit].  PinRefs are elements of TConnList.
     */
    class PinRef : virtual public Object {
    public:
        /**
         * Reference to inst.portNm[lbOffset]
         * @param inst cell containing pin.
         * @param portNm port name.
         * @param lbOffset offset from leftmost bit of portNm.
         *  portNm does not have to be a bus, in which case
         *  lbOffset is 0.  Upon use, if lbOffset>0, then portNm needs to be
         *  a bus.
         */
        explicit PinRef(TRcCell &inst, string portNm, unsigned lbOffset = 0);

        const TRcCell& getCell() const {
            return m_cell;
        }

        TRcCell& getCell() {
            return m_cell;
        }

        /**
         * Get port of cell referenced by pin.
         * @return scalar or bus.
         */
        const TRcPort getPort() const;
        TRcPort getPort();

        string getCellNm() const {
            return m_cell->getInstName();
        }

        string getPortNm() const {
            return m_portNm;
        }

        /**
         * Get full name.
         * @return full name (including bit-select, if applicable).
         */
        string getFullName(string sep = "/") const;

        /**
         * Get pin name.
         * @return pin name including bit-select.
         */
        string getBitNm() const;

        /**
         * Get pin name.
         * @return pin name with optional bit-select.
         */
        string getName(bool full = false) const {
            return (full) ? getFullName() : getBitNm();
        }
        
        unsigned getLbOffset() const {
            return m_lbOffset;
        }

        static const TRcPinRef downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, PinRef > (r);
        }

        static TRcPinRef downcast(TRcObject &r) {
            return xyzzy::downcast<Object, PinRef > (r);
        }

        virtual ~PinRef();

        static const unsigned stTypeId;

    private:
        TRcCell m_cell;
        const string m_portNm;
        const unsigned m_lbOffset;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(PinRef);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }
    };

    inline const TRcObject upcast(const TRcPinRef &p) {
        return xyzzy::upcast<Object, PinRef > (p);
    }

    inline TRcObject upcast(TRcPinRef &p) {
        return xyzzy::upcast<Object, PinRef > (p);
    }
}

#endif
