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
#include "xyzzy/bitvec.hxx"
#include "xyzzy/assert.hxx"
#include "xyzzy/exception.hxx"

namespace xyzzy
{

unsigned
TBvBase::length(unsigned l, unsigned r)
{
	return 1 + (isDescending(l, r) ? (l - r) : (r - l));
}

bool
TBvBase::checkBounds(unsigned l, unsigned r) const
{
	bool rval = false;
	if (isDescending())
	{
		rval = (l >= r) && (l <= lb) && (r >= rb);
	}
	else 
	{
		rval = (r >= l) && (l >= lb) && (r <= rb);
	}
	invariant(rval);
	return rval;
}

unsigned 
TBvBase::offset(unsigned right) const
{
	unsigned rval = 0;
	rval = isDescending() ? (right - rb) : (rb - right);
	return rval;
}

//BEGIN: TUint32 specialization
//static
template<>
const TUint32
PTBitVecMethods<TUint32>::stAllOnes = ~0;

template<>
const unsigned
PTBitVecMethods<TUint32>::stMaxBits = 32;	//verified in portable.cxx

template<>
const TUint64
PTBitVecMethods<TUint64>::stAllOnes = ~0;

template<>
const unsigned
PTBitVecMethods<TUint64>::stMaxBits = 64;	//verified in portable.cxx

};

