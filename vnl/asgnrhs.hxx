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
#if !defined(_vnl_asgnrhs_hxx_)
#define  _vnl_asgnrhs_hxx_

#include "vnl/vnl.hxx"

namespace vnl {

    /** Proxy for the righthand side element of an assign stmt,
     *  which can be a Wire, Port or WireBitRef.
     */
    class AsgnRhs : virtual public Object {
    public:
        explicit AsgnRhs(const TRcPort &rhs);

        explicit AsgnRhs(const TRcWire &rhs);

        explicit AsgnRhs(const TRcWireBitRef &rhs);

        explicit AsgnRhs(const TRcObject &rhs);

        TRcObject& getEle() {
            return m_ele;
        }

        const TRcObject& getEle() const {
            return m_ele;
        }

        static const TRcAsgnRhs downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, AsgnRhs > (r);
        }

        static TRcAsgnRhs downcast(TRcObject &r) {
            return xyzzy::downcast<Object, AsgnRhs > (r);
        }

        string getName(bool unused = false) const;
        
        virtual ~AsgnRhs();

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;

    private:
        TRcObject m_ele;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(AsgnRhs);
    };

    inline const TRcObject upcast(const TRcAsgnRhs &p) {
        return xyzzy::upcast<Object, AsgnRhs > (p);
    }

    inline TRcObject upcast(TRcAsgnRhs &p) {
        return xyzzy::upcast<Object, AsgnRhs > (p);
    }
}

#endif
