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
#ifndef VNLTCL_ITERATOR_HXX
#define	VNLTCL_ITERATOR_HXX

#include "vnl/tcl/vnltcl.hxx"
#include "vnl/tcl/collection.hxx"

namespace vnltcl {
    DECL_CLASS(Iterator);

    /*
     * A wrapper around the base xyzzy::PSlist::Iterator.
     * This Iterator is exposed to the Tcl side for use 
     * in foreach_in_collection ... {}
     * Do NOT confuse w/ the TCollectionIter which is used
     * internally.
     */
    class Iterator : public Object {
    public:
        /**
         * Create iterator over collection.
         * @param coll collection to iterate over.
         */
        explicit Iterator(TRcCollection &coll);

        /**
         * getNext() will return valid data.
         * @return true if getNext() will return valid data.
         */
        bool hasNext() const {
            return m_iter.hasMore();
        }
        
        TRcObject& getNext();
        
        static bool isA(const TRcObject &obj) {
            //NOTE: make sure you call base class, else get infinite loop.
            return stOneOf.Object::isA(obj);
        }

        static string getTypeName() {
            return stOneOf.Object::getTypeName();
        }
        
        virtual ~Iterator() {
        }
        
    private:
        /**
         * Get typeId for this class.
         * @return typeId for this class.
         */
        unsigned getTypeId() const;
        
        explicit Iterator();

        DECL_COPY_CONSTRUCTORS(Iterator);

        static unsigned	stTypeId;
        static Iterator stOneOf;
        
        /**
         */
        CollectionIter m_iter;
    };
    
    inline TRcObject upcast(const TRcIterator &p) {
        return xyzzy::upcast<Object,Iterator> (p);
    }    

    inline TRcIterator toIterator(const TRcObject &p) {
        return xyzzy::downcast<Object,Iterator> (p);
    }    
}

#endif	/* VNLTCL_ITERATOR_HXX */

