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
#if !defined(_slf_libraryset_hxx_)
#define  _slf_libraryset_hxx_

#include <map>
#include "vnl/library.hxx"
#include "vnl/libcell.hxx"
#include "slf/slf.hxx"

namespace slf {
    using std::map;
    using std::pair;
    using vnl::TRcLibrary;

    /**
     * Container for named libraries (of LibCell).
     */
    class LibrarySet : virtual public TRcObj {
    public:
        typedef pair<TRcLibrary, TRcLibCell> t_libAndCell;
        typedef map<string, TRcLibrary> t_libByName;
        
        explicit LibrarySet() {
        }

        /**
         * Add library to this set.
         * @param lib library to add.
         */
        void addLibrary(TRcLibrary &lib);

        bool hasLibCell(string nm) const;

        /**
         * Get 1st occurrence of libcell.
         * @param name of libcell.
         * @return library/libcell.
         */
        t_libAndCell getLibCell(string name);

        /**
         * Check if named library already exists.
         * @param library name.
         * @return true if library by nm already exists.
         */
        bool hasLibrary(const string &nm) const;

        /**
         * Check if library by same name exists.
         * @param lib check if another library with this lib's name exists.
         * @return true if another library with same name as lib exists.
         */
        bool hasLibrary(const TRcLibrary &lib) const {
            return hasLibrary(lib->getName());
        }

        t_libByName& getEntries() {
            return m_libsByName;
        }
        
        virtual ~LibrarySet() {
        }

    private:
        ///Libraries by name.
        t_libByName m_libsByName;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(LibrarySet);
    };
}

#endif
