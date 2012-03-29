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
#include <iostream>
#include "slf/keyvalue.hxx"
#include "xyzzy/assert.hxx"

namespace slf {
    using namespace std;

    KeyValue::KeyValue(string key, TRcValueType &val) : m_key(key) {
        if (val.isValid()) {
            m_vals = new PTArray<TRcValueType > (1);
            m_vals[0] = val;
        }
    }

    KeyValue::KeyValue(string key, const TRcValueTypeList &valList,
            TRcValueSet &valSet) : m_key(key) {
        if (valList.isValid()) {
            m_vals = new PTArray<TRcValueType > (valList.asT());
            m_valSet = valSet;
        }
    }

    KeyValue::~KeyValue() {
    }

    const TRcValueType&
    KeyValue::getVal(unsigned ix, unsigned expectN) const {
        ASSERT_TRUE(hasVals() && (expectN == getVals().length()));
        return getVals()[ix];
    }

    ValueSet::trc_kvByKey 
    ValueSet::asMap(bool allowDups) const {
        trc_kvByKey kvmap;
        if (0 < length()) {
            kvmap = new t_kvByKey();
            for (unsigned i = 0; i < length(); ++i) {
                const TRcKeyValue &kv = (*this)[i];
                const string &key = kv->getKey();
                ASSERT_TRUE(allowDups || !mapHasKey(kvmap.asT(), key));
                kvmap.asT()[key] = kv;
            }
        }
        return kvmap;
    }

#ifdef DEBUG
    DebugOstream&
    operator<<(DebugOstream &dos, const TRcValueSet &rcvset) {
        dos << "ValueSet {";
        const ValueSet &vset = rcvset.asT();
        for (unsigned i = 0; i < vset.length(); ++i) {
            dos << vset[i] << endl;
        }
        dos << "}" << endl;
    }

    DebugOstream&
    operator<<(DebugOstream &dos, const TRcKeyValue &kv) {
        dos << "DBG: KeyValue {" << endl
                << "m_key=" << kv->getKey() << endl
                << "m_vals=";
        if (kv->hasVals()) {
            const KeyValue::t_vals &vals = kv->getVals();
            for (unsigned i = 0; i < vals.length(); ++i) {
                dos << vals[i] << endl;
            }
        } else {
            dos << endl;
        }
        dos << "m_valSet=";
        if (kv->hasValSet()) {
            dos << kv->getValSet();
        } else {
            dos << endl;
        }
        dos << "}" << endl;
        return dos;
    }
#endif

}
