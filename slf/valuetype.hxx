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
#if !defined(_slf_valuetype_hxx_)
#    define  _slf_valuetype_hxx_

#include "slf/slf.hxx"
#include "slf/expr.hxx"
#include "slf/bus.hxx"
#include "slf/number.hxx"

namespace slf {
    class ValueType {
    public:
        enum EType {
            eExpr, eKident, eString, eNumber, eBool
        };
        
        //Value type constructors jive w/ above types.
        explicit ValueType(const TRcExpr &expr);
        
        explicit ValueType(const string &kident, const TRcBus &bus);
        
        explicit ValueType(const string &str);
        
        explicit ValueType(const TRcNumber &num, const string &unit);
        
        explicit ValueType(bool b);
        
        virtual ~ValueType();
        
    private:
        //The implementation is a big secret.
        class Impl;
        const Impl  *mp_impl;
        
        //Not allowed
        COPY_CONSTRUCTOR_DECL(ValueType);
    };
    
    class ValueTypeList : public list<TRcValueType> {
    public:
        explicit ValueTypeList();
        
        virtual ~ValueTypeList();
        
    private:
        //Not allowed
        COPY_CONSTRUCTOR_DECL(ValueTypeList);

    };

}

#endif
