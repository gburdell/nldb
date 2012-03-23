//The MIT License
//
//slf - Synopsys Library File
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
#if !defined(_slf_token_hxx_)
#define  _slf_token_hxx_

#include "xyzzy/array.hxx"
#include "vnl/location.hxx"
#include "slf/slf.hxx"

namespace slf {
    using xyzzy::PTArray;
	using vnl::Location;
	using vnl::TRcLocation;

    class Token {
    public:

        enum EType {
        	eLibrary,
        	eCell,
        	eTrue,
        	eFalse,
        	eColon,
        	eComma,
        	eLCurly,
        	eRCurly,
        	eLParen,
        	eRParen,
        	eLBracket,
        	eRBracket,
        	eSemi,
        	eStar,
        	ePlus,
        	eIdent,
        	eInteger,
        	eFloat,
        	eString,
            eEOF,
            eInvalid
        };

        explicit Token(EType type, string text, TRcLocation &loc)
        : m_text(text), m_loc(loc), m_type(type) {
        }

        string getText() const {
            return m_text;
        }

        const TRcLocation& getLocation() const {
            return m_loc;
        }

        EType getType() const {
            return m_type;
        }

        bool isType(EType type) const {
            return (getType() == type);
        }
        
        bool isEOF() const {
            return isType(eEOF);
        }

        string toString() const;
        
        //Allow default constructor

    private:
        friend class Lexer;

        string m_text;
        TRcLocation m_loc;
        EType m_type;
    };
}

#endif
