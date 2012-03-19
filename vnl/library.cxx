/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2006-2010  Karl W. Pfalzer
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

#include "vnl/library.hxx"

namespace vnl {

#define UNCONST const_cast<Library*>(this)

    static string stTypeName = "library";
    const unsigned Library::stTypeId = Object::getNextTypeId(stTypeName);

    bool Library::add(TRcModule &mod) {
        string nm = mod->getName();
        bool ok = !hasModule(nm);
        if (ok) {
            m_modulesByName[nm] = mod;
            if (m_firstAdded.isNull()) {
                m_firstAdded = mod;
            }
        }
        return ok;
    }

    TRcModule
    Library::getModule(string name) {
        TRcModule mod;
        if (hasModule(name)) {
            mod = m_modulesByName[name];
        }
        return mod;
    }

    const TRcModule
    Library::getModule(string name) const {
        return UNCONST->getModule(name);
    }

    Library::~Library() {
    }

#undef UNCONST

};
