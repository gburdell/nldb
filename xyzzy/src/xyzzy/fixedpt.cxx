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

#include "xyzzy/fixedpt.hxx"
#include "xyzzy/assert.hxx"
#include "xyzzy/bitvec.hxx"

namespace xyzzy
{

template<>
TInt64
PTFixedPtBase<TInt64>::toFixedPoint(double d, unsigned intN, unsigned fractN)
{
  	// Double precision word
  	// S EEEEEEEEEEE FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
  	// 0 1        11 12                                                63
	compileAssertTrue<sizeof(TUint64) == sizeof(double)>();
	ASSERT_TRUE((intN + fractN) <= cNumBits);

	TBitVec64<0,63> fp;
	{
		double *pd = &d;
		TUint64 x = *reinterpret_cast<TUint64*>(pd);
		fp = x;
	}

	bool sign = fp[0];
	ASSERT_TRUE((sign && (d < 0)) || (!sign && (d >= 0)));
	TBitVec64<53> fract = (TUint64)fp(12,63);
	ASSERT_TRUE(!fract[52]);
	int exp = fp(1,11);
	ASSERT_TRUE(0 <= exp);
	if ((0 == fract) && (0 == exp))
	{
		return 0;
	}
	exp -= 1023;

	//account for implicit 1.f
	fract[52] = 1;
	exp += 1;

	// Parts of result: intp.fractp
	TBitVec64<64> intp = 0;
	TBitVec64<64> fractp = 0;

	if (0 < exp)
	{
		int rb = fract.left() - exp + 1;
		intp(exp-1,0) = fract(fract.left(), rb);
		unsigned nfract = fract.length() - exp;
		int rb2 = (fractN < nfract) 
					? rb - fractN
					: fract.right();
		fractp = fract(rb-1, rb2);
	}
	else
	{
		exp *= -1;	//now exp>=0
		if (exp >= (int)fractN)
		{
			fractp = 0;
			ASSERT_TRUE(0 == intp);
			return 0;
		}
		else
		{
			ASSERT_TRUE((0 <= exp) && ((unsigned)exp < fractN));
			int rb = fract.left() - fractN + 1;
			rb = (0 > rb) ? 0 : rb;
			fractp = fract(fract.left(), rb);
			if (fractN > fract.length())
			{
				unsigned amt = fractN - fract.length();
				fractp = fractp << amt;	//add more digits
			}
			fractp = fractp >> exp;
		}
	}
	TInt64 result = intp;
	result <<= fractN;
	result |= fractp;
	result *= (sign) ? -1 : 1;
	return result;
}

template<>
TInt32
PTFixedPtBase<TInt32>::toFixedPoint(double d, unsigned intN, unsigned fractN)
{
	ASSERT_TRUE((intN + fractN) <= cNumBits);
	TInt64 x = PTFixedPtBase<TInt64>::toFixedPoint(d, intN, fractN);
	int shiftBy = intN + fractN - cNumBits;
	TInt32 rval = x;
#ifdef FEB_22_BAD
	if (0 < shiftBy)
	{
		TInt64 x2 = (0 > x) ? -x : x;
		x2 >>= shiftBy;
		rval = x2;
	}
	else
	{
		rval = (0 > x) ? -x : x;
	}
	rval *= (0 > x) ? -1 : 1;
#else
	if (0 < shiftBy)
		rval = asr(x, shiftBy);//PTFixedPtBase<TInt64>::asr(x, shiftBy);
#endif
	return rval;
}

template<>
double
PTFixedPtBase<TInt64>::toDouble(TInt64 x, unsigned intN, unsigned fractN)
{
	const bool sign = x < 0;
	if (sign)
		x = -x;	//x is now positive
	TBitVec64<64> xbits(x);
	ASSERT_TRUE(false == xbits[63]);
	TUint64 intp = xbits(intN + fractN - 1, fractN);
	TBitVec64<64> fract(xbits(fractN - 1, 0));
	ASSERT_TRUE(intp >= 0);
	double rval = intp;
	const double cLSB = 1.0 / (1ul << fractN);
	double fractp = fract * cLSB;
	rval += fractp;
	rval *= (sign) ? -1 : 1;
	return rval;
}

template<>
double
PTFixedPtBase<TInt32>::toDouble(TInt32 x, unsigned intN, unsigned fractN)
{
	double rval = PTFixedPtBase<TInt64>::toDouble(x, intN, fractN);
	return rval;
}

const PTFixedPtBase<TInt32>
to32(const PTFixedPtBase<TInt64>& r)
{
	TInt64 val = r.getVal();
	if (0 == val)
	{
		return PTFixedPtBase<TInt32>(r.cIntN, r.cFractN, 0);
	}
	/* If we "TInt32 x = (TInt32)val"
	 * (empirically observed) x is simply lower 32-bit LSBs
	 * and all sense of sign is lost.
	 * So, we'll do it the more accurate/harder way.
	 */
	const unsigned cNumBits = PTFixedPtBase<TInt32>::cNumBits; 
	int shiftBy = r.cIntN + r.cFractN - cNumBits;
	int fractN = r.cFractN;
	// Make sure we dont lose all of the intp
	if (0 < shiftBy)
	{
		// [35.1] -> >>4
		ASSERT_TRUE(r.cIntN < cNumBits);
		val = asr(val, shiftBy);//PTFixedPtBase<TInt64>::asr(val, shiftBy);
		ASSERT_TRUE(shiftBy < fractN);
		fractN -= shiftBy;
	}
	//TODO: if this ASSERT triggers, have issue where r too big
	ASSERT_TRUE(0 == ((~(TInt64)0 << cNumBits) & val));	//only using low 32 bits

	TInt32 val32 = val;
	PTFixedPtBase<TInt32> rval(r.cIntN, fractN, val32);
	return rval;
}

}
