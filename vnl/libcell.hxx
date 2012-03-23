/* The MIT License
 * 
 * vnl - verilog netlist
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
#if !defined(_vnl_libcell_hxx_)
#define  _vnl_libcell_hxx_

#include "vnl/vnl.hxx"

namespace vnl {
    using std::map;

    class LibCell : virtual public Object {
    public:
        explicit LibCell(string name)
        : m_name(name) {
        }

        string getName() const {
            return m_name;
        }

        virtual ~LibCell();

    private:
        const string m_name;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(LibCell);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;

    };

    inline const TRcObject upcast(const TRcLibCell &p) {
        return xyzzy::upcast<Object, LibCell > (p);
    }

    inline TRcObject upcast(TRcLibCell &p) {
        return xyzzy::upcast<Object, LibCell > (p);
    }
}

#endif
