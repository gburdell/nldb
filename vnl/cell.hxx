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
#if !defined(_vnl_cell_hxx_)
#define  _vnl_cell_hxx_

#include "vnl/vnl.hxx"

namespace vnl {

    class Cell : virtual public Object {
    public:
        explicit Cell(string refNm, string instNm, TRcConnsByPortName &conns);

        /**
         * This should be called after cell is created.
         * Update wires w/ PinRef (to cell).
         * @param cell update wires w/ PinRef to this cell.
         */
        static void updateWires(TRcCell &cell);
        
        string getRefName() const {
            return m_refNm;
        }

        string getInstName() const {
            return m_instNm;
        }

        const TRcObject& getRef() const {
            return m_ref;
        }
        TRcObject& getRef() {
            return m_ref;
        }
        
        bool isHierarchical() const;
        
        bool isResolved() const {
            return m_ref.isValid();
        }
        
        void setRef(TRcObject &ref) {
            m_ref = ref;
        }
        
        static const TRcCell downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, Cell > (r);
        }
        static TRcCell downcast(TRcObject &r) {
            return xyzzy::downcast<Object, Cell > (r);
        }

        virtual ~Cell();

    private:
        string      m_refNm, m_instNm;
        /**
         * Can't use TRcModule since Cell includes/need Module,
         * and Module includes/needs Cell.
         */
        TRcObject   m_ref; //upcast from TRcModule (or Libcell)
        
        /**
         * Map of pin connections by pin/port name.
         * Each entry is an array, where [0] is connection
         * to left-most bit of pin.  
         * NOTE: If no connections, as in ".p()", the array has length==0.
         */
        TRcConnsByPortName      m_connsByPortName;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(Cell);

        void init();
        
        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;
    };

    inline const TRcObject upcast(const TRcCell &p) {
        return xyzzy::upcast<Object, Cell > (p);
    }
    inline TRcObject upcast(TRcCell &p) {
        return xyzzy::upcast<Object, Cell > (p);
    }
}

#endif
