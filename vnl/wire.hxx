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
#if !defined(_vnl_wire_hxx_)
#define  _vnl_wire_hxx_

#include <ostream>
#include "vnl/vnl.hxx"
#include "vnl/pinref.hxx"

namespace vnl {
    using std::ostream;
    
    class Wire : virtual public Object {
    public:
        static const string stConstNm[2]; //names for logic[01]

        explicit Wire(string name) : m_name(name) {
        }

        /**
         * Check if wire is a constant 0 or 1.
         * @return true if name is one of stConst[01]
         */
        bool isConst() const {
            return isConst0() || isConst1();
        }

        bool isConst0() const {
            return (getName() == stConstNm[0]);
        }

        bool isConst1() const {
            return (getName() == stConstNm[1]);
        }

        virtual string getName(bool unused = false) const {
            return m_name;
        }

        virtual bool isBus() const {
            return false;
        }

        /**
         * Write declaration to stream
         * @param os output stream.
         * @return output stream.
         */
        virtual ostream& operator<<(ostream &os) const;
        
        bool isScalar() const {
            return !isBus();
        }

        void add(TRcPinRef &conn);
        void add(TRcWire &conn);
        void add(TRcObject conn);

        virtual bool isPort() const {
            return false;
        }
        
        /**
         * Return connections to this wire.
         * A connection element is Wire, WireBitRef, PinRef.
         * If wire, then assign statement.
         * @return connections to this wire.
         */
        TRcConnList& getConns() {
            return m_conns;
        }

        const TRcConnList& getConns() const {
            return m_conns;
        }

        static const TRcWire downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, Wire > (r);
        }

        static TRcWire downcast(TRcObject &r) {
            return xyzzy::downcast<Object, Wire > (r);
        }

        virtual ~Wire();


        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;

    protected:
        const string m_name;
        ///List of PinRef or Wire
        TRcConnList m_conns;

    private:
        //Not allowed
        COPY_CONSTRUCTOR_DECL(Wire);
    };

    inline
    ostream& operator<<(ostream &os, const TRcWire &wire) {
        return wire->operator <<(os);
    }
    
    inline const TRcObject upcast(const TRcWire &p) {
        return xyzzy::upcast<Object, Wire > (p);
    }

    inline TRcObject upcast(TRcWire &p) {
        return xyzzy::upcast<Object, Wire > (p);
    }
}

#endif
