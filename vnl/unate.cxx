/* The MIT License
 * 
 * vnl - verilog netlist
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

#include "vnl/unate.hxx"

namespace vnl {

    void
    Unate::addRelation(const string &in, const string &out, EUnate unate) {
        if (m_outsByIn.isNull()) {
            m_outsByIn = new t_outsByIn();
        }
        trc_unateList outs;
        bool addIt = true;
        if (m_outsByIn->find(in) == m_outsByIn->end()) {
            //No out by this name
            outs = new t_unateList();
            m_outsByIn.asT()[in] = outs;
        } else {
            //Check for consistent unate
            outs = m_outsByIn.asT()[in];
            t_unateList::iterator byOut = outs->begin();
            for (; byOut != outs->end(); ++byOut) {
                if (out == byOut->first) {
                    if (unate != byOut->second) {
                        //switch to both
                        byOut->second = eBoth;
                    }
                    addIt = false;
                    break; //for
                }
            }
        }
        if (addIt) {
            t_unate rel = make_pair(out, unate);
            outs->push_back(rel);
        }
    }

    void
    Unate::addRelation(const string &in, const string &out, const string &unate) {
        EUnate eun;
        if ("positive_unate" == unate) {
            eun = ePositive;
        } else if ("negative_unate" == unate) {
            eun = eNegative;
        } else {
            eun = eBoth;
        }
        addRelation(in, out, eun);
    }

    Unate::trc_unateList
    Unate::getAffectedOutputs(const string &in) {
        trc_unateList outs;
        if (m_outsByIn->find(in) != m_outsByIn->end()) {
            outs = m_outsByIn.asT()[in];
        }
        return outs;
    }

    Unate::~Unate() {
    }

#undef UNCONST

};

