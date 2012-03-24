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
    Unate::addRelation(string in, string out, EUnate unate) {
        if (m_outsByIn.isNull()) {
            m_outsByIn = new t_outsByIn();
        }
        t_unate rel = make_pair(out, unate);
        trc_unateList outs;
        if (m_outsByIn->find(in) == m_outsByIn->end()) {
            outs = new t_unateList();
            m_outsByIn.asT()[in] = outs;
        } else {
            outs = m_outsByIn.asT()[in];
        }
        outs->push_back(rel);
    }

    void
    Unate::reduce() {
        Unate reduced;
        t_outsByIn::const_iterator byKey;
        for (byKey = m_outsByIn->begin(); byKey != m_outsByIn->end(); ++byKey) {
            string in = byKey->first;
            const t_unateList& outs = byKey->second.asT();
            //For "in", iterate over all affected outputs and
            //track singular unateness
            typedef map<string, EUnate> t_reduced;
            t_reduced reducedForIn;
            t_unateList::const_iterator byOut;
            for (byOut = outs.begin(); byOut != outs.end(); ++byOut) {
                string out = byOut->first;
                if (reducedForIn.find(out) == reducedForIn.end()) {
                    reducedForIn[out] = byOut->second;
                } else if (reducedForIn[out] != byOut->second) {
                    reducedForIn[out] = eBoth;
                }
            }
            //Update reduced
            t_reduced::const_iterator redByOut;
            for (redByOut = reducedForIn.begin(); redByOut != reducedForIn.end();
                    ++redByOut) {
                string out = redByOut->first;
                reduced.addRelation(in, out, redByOut->second);
            }
        }
        //Replace w/ reduced
        m_outsByIn = reduced.m_outsByIn;
    }
    
    Unate::trc_unateList
    Unate::getAffectedOutputs(string in) {
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

