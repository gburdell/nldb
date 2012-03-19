//The MIT License
//
//xyzzy - c++ library
//Copyright (c) 2007-2010  Karl W. Pfalzer
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

#if	!defined(_xyzzy_indent_hxx_)
#    define  _xyzzy_indent_hxx_

#include <string>

namespace xyzzy
{
	class TIndent
	{
	public:
		TIndent(int indentAmt = 4);
		
		TIndent(const TIndent &s, bool indent = true);

		operator const char*() const
		{
			return m_indent.c_str();
		}

	private:
		static int	st_indentAmt;
		static bool st_notUsedYet;

		std::string	m_indent;
	};
};

/*
  Following macros used as:
 
	TIndent start;
	...
	// "indent" will be declared as "const char*" by BEGIN_INDENT() macro
	// for subsequent used.
	//
 	BEGIN_INDENT(indent, start);
	...
	cerr << indent << ...;
	...
	END_INDENT(start);
 */

#define BEGIN_INDENT(_indent,_indentRef) 					\
	xyzzy::TIndent __indentSaved(_indentRef, false);\
	_indentRef = xyzzy::TIndent(__indentSaved);     \
	const char *_indent = _indentRef

#define END_INDENT(_indentRef) _indentRef = xyzzy::TIndent(__indentSaved,false)

#endif	//#    define  _xyzzy_indent_hxx_
