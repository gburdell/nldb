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

#include "xyzzy/array.hxx"

namespace xyzzy
{
	TArray::TArray()
	:	m_len(0)
	{}

	TArray::TArray(len_t len)
	:	m_len(len)
	{}

	TArray::len_t
	TArray::checkIx(len_t ix) const throw(IndexOutOfBoundsException)
	{
		if (ix >= m_len)
		{
			throw IndexOutOfBoundsException(m_len, ix);
		}
		return ix;
	}

	TArray::IndexOutOfBoundsException::IndexOutOfBoundsException(len_t max, len_t ix)
    :	TException("Index out of bounds."),
		m_max(max), m_ix(ix)
	{}
}
