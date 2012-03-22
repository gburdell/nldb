//The MIT License
//
//vnl - verilog netlist
//Copyright (c) 2006-2010  Karl W. Pfalzer
//Copyright (c) 2012-      George P. Burdell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
#ifndef VNLTCL_COLLECTION_HXX
#define	VNLTCL_COLLECTION_HXX

#include "xyzzy/slist.hxx"
#include "vnl/vnl.hxx"
#include "vnl/tcl/vnltcl.hxx"

namespace vnltcl {
	using xyzzy::PTSlist;

	DECL_CLASS(Collection);

    typedef PTSlist<TRcObject>::Iterator CollectionIter;

    /**
     * A collection of TRcObject.
     */
    class Collection : public Object, public PTSlist<TRcObject> {
    public:
        explicit Collection();

        /**
         * Create collection of 1 scalar (not collection) element.
         * @param scalar scalar element to initialize collection.
         */
        explicit Collection(const TRcObject &scalar);

        /**
         * Create collection as copy of existing collection.
         * @param base initialize with copy of this base.
         */
        explicit Collection(TRcCollection &base);

        /**
         * Add object to collection.
         * @param obj object to add.
         * @param allowHetero set true to allow heterogeneous collection.
         * @return true if obj added, else false.
         */
        bool add(const TRcObject &obj, bool allowHetero = false);
        
        /**
         * Add collection to collection.
         * @param from object to add.
         * @param allowHetero set true to allow heterogeneous collection.
         * @return true if any of obj added, else false.
         */
        bool add(TRcCollection &from, bool allowHetero = false);
        
        static bool isA(const TRcObject &obj) {
            return (stTypeId == obj->getTypeId());
        }

        static string getTypeName();
        
        virtual ~Collection() {
        }
    private:
        /**
         * Get typeId for this class.
         * @return typeId for this class.
         */
        unsigned getTypeId() const;

        DECL_COPY_CONSTRUCTORS(Collection);

        static unsigned         stTypeId;
    };
    
    inline TRcObject upcast(const TRcCollection &p) {
        return xyzzy::upcast<Object,Collection> (p);
    }    

    inline TRcCollection toCollection(const TRcObject &p) {
        return xyzzy::downcast<Object,Collection> (p);
    }    
}
#endif	/* VNLTCL_COLLECTION_HXX */

