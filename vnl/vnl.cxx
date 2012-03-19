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

#include <map>
#include "vnl/vnl.hxx"

namespace vnl {

    unsigned
    length(unsigned lb, unsigned rb) {
        return 1 + ((lb < rb) ? (rb - lb) : (lb - rb));
    }

    using std::map;

    typedef map<unsigned, string> TRttiMap;

    /**
     * Get reference to the RTTI map.
     * This style alleviates order of static initialization headaches,
     * since static objects of methods/functions are init'd 1st time flow
     * passes through.
     * @return the RTTI map.
     */
    static TRttiMap& getRttiMap() {
        static TRttiMap theMap;
        return theMap;
    }

    unsigned Object::stTypeId = Object::getNextTypeId("object");

    unsigned
    Object::getNextTypeId(string typeName) {
        static unsigned stLastId = 0;
        getRttiMap()[stLastId] = typeName;
        return stLastId++;
    }

    unsigned
    Object::getTypeId() const {
        return stTypeId;
    }

    bool
    Object::isA(const TRcObject &o) const {
        return (getTypeId() == o->getTypeId());
    }

    string
    Object::getTypeName() const {
        return getRttiMap()[getTypeId()];
    }
}
