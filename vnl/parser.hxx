//The MIT License
//
//vnl - verilog netlist
//Copyright (c) 2006-2010  Karl W. Pfalzer
//Copyright (c) 2012-      George P. Burdell
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.
#if !defined(_vnl_parser_hxx_)
#define  _vnl_parser_hxx_

#include <queue>
#include <map>
#include <vector>
#include "vnl/vnl.hxx"
#include "vnl/lexer.hxx"
#include "vnl/library.hxx"
#include "vnl/module.hxx"
#include "vnl/bus.hxx"
#include "vnl/constref.hxx"

namespace vnl {
    using std::deque;
    using std::map;
    using std::vector;

    class Parser {
    public:
        typedef Token::EType EType;

        explicit Parser(TRcLexer &lexer);

        void start(TRcLibrary &lib);
        
        unsigned getErrorCnt() const {
            return m_errCnt;
        }

        virtual ~Parser();

    private:
        unsigned m_errCnt;
        TRcLexer m_lexer;

        typedef deque<TRcToken> t_tokStack;
        t_tokStack m_toks;

        /**
         * Get lookahead token.
         * @param lib
         * @return next token.
         */
        TRcToken la(unsigned i);

        /**
         * Accept accumulated tokens.
         * @param n number of tokens to accept.
         * @return last token (accepted).
         */
        TRcToken accept(unsigned n = 1);

        /**
         * Check token type at la(i).
         * @param type check if this type.
         * @return true if la(i) is same as type.
         */
        bool test(EType type, unsigned i = 0);

        /**
         * Expect token type at la(i).
         * @param type check if this type.
         * @return true if la(i) is same as type.
         *         Generate error message on false;
         */
        bool expect(EType type, unsigned i = 0);

        TRcToken expectAccept(EType type) throw (unsigned);

        /**
         * Used for error recovery.  Slurp until type is found (or EOF).
         * @param type slurp until (and consume).
         */
        void slurpUntil(EType type = Token::eSemi);

        TRcModule moduleDeclaration(TRcLibrary &lib);

        void portDeclarations(TRcModule &mod) throw (unsigned);

        void moduleItems(TRcModule &mod) throw (unsigned);

        void wireDefinition(TRcModule &mod) throw (unsigned);

        void assignStatement(TRcModule &mod) throw (unsigned);

        void moduleInstantiation(TRcModule &mod) throw (unsigned);

        void moduleInstantiation(TRcModule &mod, TRcToken &refNm,
                TRcToken &instNm) throw (unsigned);

        TRcConnList connections(TRcModule &mod) throw (unsigned);

        TRcConnList expression(TRcModule &mod) throw (unsigned);

        TRcConnList primary(TRcModule &mod) throw (unsigned);

        TRcConnList concatentation(TRcModule &mod) throw (unsigned);

        TRcBus range() throw (unsigned);

        unsigned toUnsigned(TRcToken&) throw (unsigned);

        /**
         * Convert based number to list of const wires.
         * @param size number of bits (could be null).
         * @param based based number token.
         * @param mod module containing const wire 0/1.
         * @return list of const wire (begin() is MSB)
         */
        TRcConnList toBits(TRcToken &size, TRcToken &based, TRcModule &mod) throw (unsigned);

        COPY_CONSTRUCTOR_DECL(Parser);
    };
}

#endif
