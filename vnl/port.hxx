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
#if !defined(_vnl_port_hxx_)
#define  _vnl_port_hxx_

#include <ostream>
#include "vnl/vnl.hxx"
#include "vnl/wire.hxx"

namespace vnl {
    using std::ostream;
    
    class Port : virtual public Object, virtual public Wire {
    public:

        enum EDirection {
            eIn = 1, eOut = 2, eInout = eIn | eOut, eUnknown
        };

        explicit Port(string name, EDirection dir = eUnknown)
        : Wire(name), m_dir(dir) {
        }

        void set(EDirection dir) {
            m_dir = dir;
        }

        EDirection getDirection() const {
            return m_dir;
        }

        static const TRcPort downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, Port > (r);
        }

        static TRcPort downcast(TRcObject &r) {
            return xyzzy::downcast<Object, Port > (r);
        }

        bool isPort() const {
            return true;
        }

        /**
         * Write declaration to stream
         * @param os output stream.
         * @return output stream.
         */
        virtual ostream& operator<<(ostream &os) const;
        
        virtual ~Port();

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;
        
    protected:
        /**
         * Write port direction to stream.
         * @param os output stream.
         * @return output stream.
         */
        ostream& writeDirection(ostream &os) const;
        
    private:
        EDirection m_dir;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(Port);
    };

    inline
    ostream& operator<<(ostream &os, const TRcPort &port) {
        return port->operator <<(os);
    }
    
    inline const TRcObject upcast(const TRcPort &p) {
        return xyzzy::upcast<Object, Port > (p);
    }

    inline TRcObject upcast(TRcPort &p) {
        return xyzzy::upcast<Object, Port > (p);
    }

    inline const TRcWire toWire(const TRcPort &p) {
        return xyzzy::upcast<Wire, Port > (p);
    }

    inline TRcWire toWire(TRcPort &p) {
        return xyzzy::upcast<Wire, Port > (p);
    }

    inline const TRcPort toPort(const TRcWire &w) {
        return xyzzy::downcast<Wire, Port > (w);
    }

    inline TRcPort toPort(TRcWire &w) {
        return xyzzy::downcast<Wire, Port > (w);
    }
}

#endif
