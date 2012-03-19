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

#if	!defined(_xyzzy_assert_hxx_)
#	 define  _xyzzy_assert_hxx_

#include <string>

#define ASSERT_TRUE(_cond) xyzzy::assertTrue(_cond, #_cond, __FILE__, __LINE__)

#define ASSERT_NEVER ASSERT_TRUE(false)

namespace xyzzy
{
	bool assertTrue(const bool cond);

	bool assertTrue(const bool cond, const char* cexpr,
				    const char* fname, unsigned lnum);
	
	// call perror, exit w/ failure
	void die(const char *pfx);

	/**
	 *	Use compileAssertTrue for compile time check; i.e.,
	 *	the "cond" clause must be static/compile-time 
	 * 	evaluated.
	 * 	Example usage:
	 * 		compileAssertTrue<sizeof(char) < sizeof(int)>();
	 */
	template<bool cond> struct compileAssertTrue;

	// Specialize just the true version.
	template<> struct compileAssertTrue<true> {};
};

#endif	//_xyzzy_assert_hxx_
