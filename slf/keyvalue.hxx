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
#if !defined(_slf_keyvalue_hxx_)
#define  _slf_keyvalue_hxx_

#include <list>
#include <map>
#include "xyzzy/array.hxx"
#include "slf/slf.hxx"
#include "slf/libraryele.hxx"
#include "slf/valuetype.hxx"

namespace slf {
    using xyzzy::PTRcArray;
    using std::list;
    using std::map;

    class KeyValue : virtual public TRcObj, public LibraryEle {
    public:
        typedef PTRcArray<TRcValueType> t_vals;

        explicit KeyValue(string key, TRcValueType &val);

        explicit KeyValue(string key, const TRcValueTypeList &valList, TRcValueSet &valSet);

        EType getType() const {
            return LibraryEle::eKeyValue;
        }

        const string& getKey() const {
            return m_key;
        }

        bool hasVals() const {
            return m_vals.isValid();
        }

        bool hasValSet() const {
            return m_valSet.isValid();
        }

        const t_vals& getVals() const {
            return m_vals;
        }

        const TRcValueSet& getValSet() const {
            return m_valSet;
        }

        static const TRcKeyValue downcast(const TRcLibraryEle &r) {
            return xyzzy::downcast<LibraryEle, KeyValue > (r);
        }

        static TRcKeyValue downcast(TRcLibraryEle &r) {
            return xyzzy::downcast<LibraryEle, KeyValue > (r);
        }

        virtual ~KeyValue();

    private:
        const string m_key;
        t_vals m_vals;
        TRcValueSet m_valSet;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(KeyValue);

    };

    DebugOstream& operator<<(DebugOstream &dos, const TRcKeyValue &kv);

    class ValueSet : public PTArray<TRcKeyValue> {
    public:
        typedef list<TRcKeyValue> t_keyValues;
        typedef map<string, TRcKeyValue> t_kvByKey;
        typedef PTRcPtr<t_kvByKey> trc_kvByKey;

        explicit ValueSet(const t_keyValues &vals)
        : PTArray<TRcKeyValue>(vals) {
        }

        /**
         * Get ValueSet as a map of KeyValue by key.
         * @return map of key values by key.
         */
        trc_kvByKey asMap() const;

        virtual ~ValueSet() {
        }

    private:
        //Not allowed
        COPY_CONSTRUCTOR_DECL(ValueSet);

    };

    inline const TRcLibraryEle asLibraryEle(const TRcKeyValue &p) {
        return xyzzy::upcast<LibraryEle, KeyValue > (p);
    }

    inline TRcLibraryEle asLibraryEle(TRcKeyValue &p) {
        return xyzzy::upcast<LibraryEle, KeyValue > (p);
    }

}

#endif
