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
#if !defined(_vnl_wirebus_hxx_)
#define  _vnl_wirebus_hxx_

#include <utility>
#include "vnl/vnl.hxx"
#include "vnl/wire.hxx"
#include "vnl/bus.hxx"

namespace vnl {
    using std::pair;
    using std::make_pair;

    class WireBus : virtual public Object, virtual public Wire, public Bus {
    public:
        explicit WireBus(string name, const TRcBus &bus);

        bool isBus() const {
            return true;
        }

        /**
         * Write declaration to stream
         * @param os output stream.
         * @return output stream.
         */
        virtual ostream& operator<<(ostream &os) const;

        static const TRcWireBus downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, WireBus > (r);
        }

        static TRcWireBus downcast(TRcObject &r) {
            return xyzzy::downcast<Object, WireBus > (r);
        }

        virtual ~WireBus();

        /**
         * Get PinConns at [lb...i...rb]
         * @param i bounded index.
         * @return pin connections.
         */
        TRcConnList operator [](unsigned i);

        const TRcConnList operator [](unsigned i) const;

    private:
        typedef PTArray<TRcConnList> t_pinConns;
        /**
         * The connection for [lb] is done using Wire.m_conn,
         * a little tricky.
         */
        t_pinConns m_pinConns;

        TRcConnList getConnList(unsigned i, bool create = false);

        //Not allowed
        COPY_CONSTRUCTOR_DECL(WireBus);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;

    };

    inline
    ostream& operator<<(ostream &os, const TRcWireBus &wire) {
        return wire->operator <<(os);
    }

    inline const TRcObject upcast(const TRcWireBus &p) {
        return xyzzy::upcast<Object, WireBus > (p);
    }

    inline TRcObject upcast(TRcWireBus &p) {
        return xyzzy::upcast<Object, WireBus > (p);
    }

    inline const TRcWire toWire(const TRcWireBus &p) {
        return xyzzy::upcast<Wire, WireBus > (p);
    }

    inline TRcWire toWire(TRcWireBus &p) {
        return xyzzy::upcast<Wire, WireBus > (p);
    }

    inline const TRcWireBus toWireBus(const TRcWire &p) {
        return xyzzy::downcast<Wire, WireBus > (p);
    }

    inline TRcWireBus toWireBus(TRcWire &p) {
        return xyzzy::downcast<Wire, WireBus > (p);
    }
}

#endif
