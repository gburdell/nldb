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
#include "xyzzy/assert.hxx"
#include "vnl/tcl/collection.hxx"

namespace vnltcl {
    static const string stTypeName = "collection";
    
    unsigned Collection::stTypeId = Object::getNextTypeId(stTypeName);
    
    string Collection::getTypeName() {
        return stTypeName;
    }
    
    Collection::Collection() {    
    }
    
    Collection::Collection(TRcCollection &base) {
        add(base);
    }

    Collection::Collection(const TRcObject &scalar) {
        ASSERT_TRUE(! Collection::isA(scalar));        
        add(scalar);
    }
    
    unsigned
    Collection::getTypeId() const {
        return stTypeId;
    }

    bool
    Collection::add(const TRcObject &item, bool allowHetero) {
        bool ok = allowHetero || isEmpty();
        if (!ok) {
            //can only add the same thing
            ok = peek()->isA(item);
        }
        if (ok) {
            append(item);
        }
        return ok;
    }

    bool
    Collection::add(TRcCollection &from, bool allowHetero) {
        bool rval = false;
        for (CollectionIter iter(from.asT()); iter.hasMore(); ++iter) {
            rval |= add(*iter, allowHetero);
        }
        return rval;
    }

}
