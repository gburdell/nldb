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
#if !defined(_vnl_libcell_hxx_)
#define  _vnl_libcell_hxx_

#include <list>
#include <utility>
#include <map>
#include "xyzzy/assert.hxx"
#include "vnl/vnl.hxx"
#include "vnl/unate.hxx"
#include "vnl/module.hxx"

namespace vnl {
    using std::list;
    using std::pair;
    using xyzzy::PTRcArray;

    class LibCell : virtual public Object, public Module {
    public:
        /* Few cells have multiple outs, and function associated w/ out.
         * ideally, need map of function by out; but a map starts
         * at 3x size of list.  So, we'll tradeoff speed for mem and just
         * use list.
         */
        typedef pair<string, string>    t_opinFunc;
        typedef list<t_opinFunc>        t_opinFuncs;
        typedef map<string,string>      t_funcByOpin;
        typedef PTRcPtr<t_funcByOpin>   trc_funcByOpin;

        explicit LibCell(const string &name);

        double getArea() const {
            return m_area;
        }

        /**
         * Set output pin function.
         * @param opin output pin to set function for.
         * @param func function to set.
         */
        void setOpinFunction(const string &opin, const string &func);

        unsigned getFuncCnt() const {
            return m_funcs.size();
        }
        
        bool hasSingleFunc() const {
            return (1 == getFuncCnt());
        }
        
        string getSingleFunc() const;
        
        /**
         * Get output pin's function.
         * @param opin output pin to get function for.
         * @return function of output pin.
         */
        string getOpinFunction(const string &opin) const;
        
        /**
         * Get all output pin functions as map by opin name.
         * @return map of function by opin name.
         */
        trc_funcByOpin getOpinFunctions() const;
        
        virtual bool isSequential() const = 0;

        virtual bool isBlackBox() const {
            return m_funcs.empty();
        }

        void setArea(double ar) {
            m_area = ar;
        }

        virtual ~LibCell();

    protected:
        t_opinFuncs m_funcs;
        double m_area;
        Unate m_unate;

    private:
        //Not allowed
        COPY_CONSTRUCTOR_DECL(LibCell);

        virtual unsigned getTypeId() const {
            return stTypeId;
        }

        static const unsigned stTypeId;

    };

    inline const TRcObject upcast(const TRcLibCell &p) {
        return xyzzy::upcast<Object, LibCell > (p);
    }

    inline TRcObject upcast(TRcLibCell &p) {
        return xyzzy::upcast<Object, LibCell > (p);
    }

    inline const TRcModule asModule(const TRcLibCell &p) {
        return xyzzy::upcast<Module, LibCell > (p);
    }

    inline TRcModule asModule(TRcLibCell &p) {
        return xyzzy::upcast<Module, LibCell > (p);
    }

}

#endif
