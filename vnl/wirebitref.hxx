/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2012-      George P. Burdell
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
#if !defined(_vnl_wirebusref_hxx_)
#define  _vnl_wirebusref_hxx_

#include "xyzzy/assert.hxx"
#include "vnl/vnl.hxx"
#include "vnl/wirebus.hxx"

namespace vnl {

    /**
     * Reference to WireBus[bit]. WireBitRefs are elements of TConnList.
     */
    class WireBitRef : virtual public Object {
    public:
        explicit WireBitRef(TRcWireBus &wire, unsigned bitIx);

        /**
         * Bit blast to list.
         * @param bus wirebus reference.
         * @param range (sub-range) to expand top bits.
         * @return list of TRcWireBitRef (begin() is MSB).
         */
        static TRcConnList bitBlast(TRcWireBus &bus, TRcBus &range);

        void add(TRcPinRef &conn) {
            add(upcast(conn));
        }

        void add(TRcWire &conn) {
            add(upcast(conn));
        }

        void add(TRcObject conn) {
            getConns()->push_back(conn);
        }

        /**
         * Get connections on this wirebus' [bit].
         * @return connections to this wirebus[bit].
         */
        TRcConnList getConns() {
            return m_wireBus->operator[](m_bitIx);
        }

        const TRcConnList getConns() const {
            return m_wireBus->operator[](m_bitIx);
        }

        unsigned getBitIx() const {
            return m_bitIx;
        }

        string getBusName() const {
            return m_wireBus->getName();
        }

        /**
         * Get increment of base/bus.
         * @return increment.
         */
        int getBusIncr() const {
            return m_wireBus->getIncr();
        }

        string toString() const;

        static const TRcWireBitRef downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, WireBitRef > (r);
        }

        static TRcWireBitRef downcast(TRcObject &r) {
            return xyzzy::downcast<Object, WireBitRef > (r);
        }

        virtual ~WireBitRef();

        static const unsigned stTypeId;

    private:
        TRcWireBus m_wireBus;
        ///Index into m_wireBus.
        unsigned m_bitIx;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(WireBitRef);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }
    };

    inline const TRcObject upcast(const TRcWireBitRef &p) {
        return xyzzy::upcast<Object, WireBitRef > (p);
    }

    inline TRcObject upcast(TRcWireBitRef &p) {
        return xyzzy::upcast<Object, WireBitRef > (p);
    }
}

#endif
