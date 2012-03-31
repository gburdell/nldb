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

#include "vnl/libcell.hxx"

namespace vnl {

#define UNCONST const_cast<LibCell*>(this)

    static string stTypeName = "libcell";
    const unsigned LibCell::stTypeId = Object::getNextTypeId(stTypeName);

    LibCell::LibCell(const string &name)
    : Module(name), m_area(0.0) {
    }

    void LibCell::setOpinFunction(const string &opin, const string &func) {
        bool noDups = true;
        for (t_opinFuncs::const_iterator i = m_funcs.begin();
                i != m_funcs.end(); ++i) {
            noDups &= (opin != i->first);
        }
        ASSERT_TRUE(noDups);
        m_funcs.push_back(make_pair(opin, func));
    }

    string
    LibCell::getSingleFunc() const {
        ASSERT_TRUE(hasSingleFunc());
        return m_funcs.front().second;
    }

    string
    LibCell::getOpinFunction(const string &opin) const {
        string func;
        trc_funcByOpin fmap = getOpinFunctions();
        if (fmap.isNull()) {
            t_funcByOpin::const_iterator i = fmap->find(opin);
            if (i != fmap->end()) {
                func = i->second;
            }
        }
        return func;
    }

    LibCell::trc_funcByOpin
    LibCell::getOpinFunctions() const {
        trc_funcByOpin fmap;
        if (0 < getFuncCnt()) {
            fmap = new t_funcByOpin();
            for (t_opinFuncs::const_iterator i = m_funcs.begin();
                    i != m_funcs.end(); ++i) {
                fmap.asT()[i->first] = i->second;
            }
        }
        return fmap;
    }

    LibCell::~LibCell() {
    }

#undef UNCONST

};
