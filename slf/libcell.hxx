/* The MIT License
 * 
 * slf - verilog netlist
 * Copyright (c) 2012       George P. Burdell
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
#if !defined(_slf_libcell_hxx_)
#define  _slf_libcell_hxx_

#include "vnl/libcell.hxx"
#include "slf/slf.hxx"
#include "slf/libraryele.hxx"
#include "slf/bus.hxx"
#include "slf/keyvalue.hxx"

namespace slf {

    class LibCell : virtual public vnl::Object, public vnl::LibCell, public LibraryEle {
    public:

        enum ECellType {eCombo, eFF, eLatch, eMemory};
        
        typedef ValueSet::trc_byOneKey trc_byOneKey;
        
        explicit LibCell(string name) : vnl::LibCell(name), m_cellType(eCombo) {
        }

        void setCellType(ECellType ctype) {
            m_cellType = ctype;
        }
        
        ECellType getCellType() const {
            return m_cellType;
        }
        
        EType getType() const {
            return LibraryEle::eLibCell;
        }

        /**
         * Add scalar port.
         * @param nm name of port.
         * @param direction port direction.
         */
        void addPort(const string &nm, const string &direction);
        
        /**
         * Added bus port.
         * @param nm name of port.
         * @param bus bus spec.
         * @param direction port direction.
         */
        void addPort(const string &nm, const TRcBus &bus, const string &direction);
        
        bool isSequential() const {
            return (eCombo != getCellType());
        }
        
        /**
         * Add timing info (currently unateness).
         * @param opin output pin
         * @param timinfo all timing{} associated with pin.
         */
        void addTiming(const string &opin, const trc_byOneKey &timinfo);
        
        static const TRcLibCell downcast(const TRcLibraryEle &r) {
            return xyzzy::downcast<LibraryEle, LibCell > (r);
        }

        static TRcLibCell downcast(TRcLibraryEle &r) {
            return xyzzy::downcast<LibraryEle, LibCell > (r);
        }

        virtual ~LibCell();

    private:
        ECellType   m_cellType;
        
        //Not allowed
        COPY_CONSTRUCTOR_DECL(LibCell);

    };

    inline const TRcLibraryEle asLibraryEle(const TRcLibCell &p) {
        return xyzzy::upcast<LibraryEle, LibCell > (p);
    }

    inline TRcLibraryEle asLibraryEle(TRcLibCell &p) {
        return xyzzy::upcast<LibraryEle, LibCell > (p);
    }

    inline const vnl::TRcModule asModule(const TRcLibCell &p) {
        return xyzzy::upcast<vnl::Module, LibCell > (p);
    }

    inline vnl::TRcModule asModule(TRcLibCell &p) {
        return xyzzy::upcast<vnl::Module, LibCell > (p);
    }

}

#endif
