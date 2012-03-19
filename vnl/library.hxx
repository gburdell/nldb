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
#if !defined(_vnl_library_hxx_)
#define  _vnl_library_hxx_

#include <map>
#include "vnl/vnl.hxx"
#include "vnl/module.hxx"

namespace vnl {
    using std::map;

    class Library : virtual public Object {
    public:
        typedef map<string, TRcModule> t_modulesByName;

        explicit Library(string name = "work")
        : m_name(name) {
        }

        string getName() const {
            return m_name;
        }

        bool hasModule(string nm) const {
            return (m_modulesByName.end() != m_modulesByName.find(nm));
        }

        const TRcModule getModule(string name) const;

        TRcModule getModule(string name);

        /**
         * Add module to library.
         * @param mod module to add.
         * @return false if module already exists; else true (if added).
         */
        bool add(TRcModule &mod);

        /**
         * Get first module added.
         * @return first module successfully added (could be null).
         */
        const TRcModule& getFirstAdded() const {
            return m_firstAdded;
        }

        TRcModule& getFirstAdded() {
            return m_firstAdded;
        }

        const t_modulesByName& getModulesByName() const {
            return m_modulesByName;
        }

        t_modulesByName& getModulesByName() {
            return m_modulesByName;
        }

        static const TRcLibrary downcast(const TRcObject &r) {
            return xyzzy::downcast<Object, Library > (r);
        }

        static TRcLibrary downcast(TRcObject &r) {
            return xyzzy::downcast<Object, Library > (r);
        }

        virtual ~Library();

    private:
        t_modulesByName m_modulesByName;
        const string m_name;
        TRcModule m_firstAdded;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(Library);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;

    };

    inline const TRcObject upcast(const TRcLibrary &p) {
        return xyzzy::upcast<Object, Library > (p);
    }

    inline TRcObject upcast(TRcLibrary &p) {
        return xyzzy::upcast<Object, Library > (p);
    }
}

#endif
