/* The MIT License
 * 
 * vnl - verilog netlist
 * Copyright (c) 2006-2010  Karl W. Pfalzer
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
#if !defined(_vnl_writer_hxx_)
#define  _vnl_writer_hxx_

#include <fstream>
#include "vnl/vnl.hxx"

namespace vnl {
    using std::ofstream;

    class Writer {
    public:

        /**
         * Verilog writer configuration.
         */
        struct Config {
            explicit Config(bool ansiPortDecl = false,
                    bool topModuleLast = true,
                    bool instTopological = false,
                    unsigned portDeclIndent = 2,
                    unsigned instDeclIndent = 2,
                    unsigned instDeclBusConnIndent = 4,
                    unsigned wireDefnIndent = 2,
                    unsigned compressBusConnExpr = 2,
                    bool libCellStub = true,
                    unsigned maxCharsPerLine = 0,
                    bool allowUnresolved = true
                    );

            const bool m_ansiPortDecl;
            const bool m_topModuleLast; //else first
            const bool m_instTopological; //else leaf, hier, alphabetically.
            const bool m_libCellStub; //write libcell interface/stub.
            const unsigned m_portDeclIndent;
            const unsigned m_instDeclIndent;
            const unsigned m_instDeclBusConnIndent;
            const unsigned m_wireDefnIndent;
            const unsigned m_compressBusConnExpr;
            const unsigned m_maxCharsPerLine; //0 for 1 term per line
            const bool m_allowUnresolved;
        };

        typedef PTRcPtr<Config> TRcConfig;
        typedef unsigned long   t_lineCnt;

        /**
         * Create verilog netlist writer.
         * @param fname output filename.
         * @param cfg writer configuration.
         * @throw true on error.
         */
        explicit Writer(const string &fname, const TRcConfig &cfg = stDefaultConfig)
        throw (bool);

        /**
         * Write top-level module using config specified with object constructor.
         * @param top top-level module.
         * @param doHier set true to write hierarchical netlist.
         * @return number of lines output.
         * @throw true on error.
         */
        t_lineCnt write(const TRcModule &top, bool doHier=true) throw (bool);

        virtual ~Writer();

    private:
        static TRcConfig stDefaultConfig;

        //Not allowed
        COPY_CONSTRUCTOR_DECL(Writer);

        class Impl;
        friend class Impl;
        Impl *mp_impl;
    };
}

#endif


