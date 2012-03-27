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
#if !defined(_vnl_unate_hxx_)
#define  _vnl_unate_hxx_

// Unateness (helper) for LibCell

#include <map>
#include <utility>
#include <list>
#include "xyzzy/refcnt.hxx"
#include "vnl/vnl.hxx"

namespace vnl {
    using std::map;
    using std::pair;
    using std::list;
    using xyzzy::PTRcPtr;

    class Unate {
    public:
        enum EUnate {
            ePositive = 1, eNegative = 2, eBoth = (ePositive | eNegative)
        };

        explicit Unate() {}
        
        typedef pair<string, EUnate> t_unate;
        typedef list<t_unate>        t_unateList;
        typedef PTRcPtr<t_unateList> trc_unateList;

        /**
         * Add unateness relation.
         * @param in input (of cell).
         * @param out affected output (by in).
         * @param unate relation of in->out.
         */
        void addRelation(string in, string out, EUnate unate);
        
        /**
         * Get affected outputs (and unate) by in.
         * @param in input.
         * @return list of affected outputs.
         */
        trc_unateList getAffectedOutputs(string in);
           
        virtual ~Unate();

    private:
        //Map of affected outputs by input.
        typedef map<string, trc_unateList> t_outsByIn;
        typedef PTRcPtr<t_outsByIn>        trc_outsByIn;
        
        trc_outsByIn m_outsByIn;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(Unate);
    };
}

#endif


