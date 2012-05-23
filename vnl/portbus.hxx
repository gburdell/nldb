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
#if !defined(_vnl_portbus_hxx_)
#define  _vnl_portbus_hxx_

#include "vnl/vnl.hxx"
#include "vnl/wirebus.hxx"
#include "vnl/port.hxx"

namespace vnl {

    class PortBus : virtual public Object, virtual public WireBus, public Port {
    public:
        explicit PortBus(string name, EDirection dir, const TRcBus &bus);

        virtual ostream& operator<<(ostream &os) const;

        static const TRcPortBus downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, PortBus > (r);
        }

        static TRcPortBus downcast(TRcObject &r) {
            return xyzzy::downcast<Object, PortBus > (r);
        }

        static const TRcPortBus downcast(const TRcPort &r) {
            return xyzzy::downcast<Port, PortBus > (r);
        }

        static TRcPortBus downcast(TRcPort &r) {
            return xyzzy::downcast<Port, PortBus > (r);
        }

        virtual ~PortBus();

    private:
        //Not allowed
        COPY_CONSTRUCTOR_DECL(PortBus);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;
    };

    inline
    ostream& operator<<(ostream &os, const TRcPortBus &port) {
        return port->operator <<(os);
    }

    inline const TRcObject upcast(const TRcPortBus &p) {
        return xyzzy::upcast<Object, PortBus > (p);
    }

    inline TRcObject upcast(TRcPortBus &p) {
        return xyzzy::upcast<Object, PortBus > (p);
    }

    inline const TRcWire toWire(const TRcPortBus &p) {
        return xyzzy::upcast<Wire, PortBus > (p);
    }

    inline TRcWire toWire(TRcPortBus &p) {
        return xyzzy::upcast<Wire, PortBus > (p);
    }
}

#endif
