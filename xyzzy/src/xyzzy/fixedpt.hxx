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

#if	!defined(_xyzzy_fixedpt_hxx_)
#	 define  _xyzzy_fixedpt_hxx_

#include <string>
#include "xyzzy/portable.hxx"
#include "xyzzy/assert.hxx"

namespace xyzzy
{

template<typename T>
T asr(T x, unsigned shift)
{
	if (((T)(-64)>>2 == (T)(-16)) &&((T)(64)>>2 == (T)(16)))
	{
		x >>= shift;
	}
	else
	{
		bool sign = (x < 0);
		x >>= shift;
		if (sign)
			x |= (T)~0 << shift;
		else
			x &= ~((T)~0 << shift);
	}
	return x;
}

template<typename T>
class PTFixedPtBase
{
public:
	static T toFixedPoint(double d, unsigned intN, unsigned fractN);

	static double toDouble(T x, unsigned intN, unsigned fractN);

	static std::string toBinary(T x, unsigned intN, unsigned fractN);

	explicit PTFixedPtBase(unsigned intN, unsigned fractN)
	:	cIntN(intN), cFractN(fractN)
	{
		check();
	}

	explicit PTFixedPtBase(unsigned intN, unsigned fractN, T d)
	:	cIntN(intN), cFractN(fractN), m_val(d)
	{
		check();
	}

	explicit PTFixedPtBase(unsigned intN, unsigned fractN, double d)
	:	cIntN(intN), cFractN(fractN)
	{
		check();
		m_val = toFixedPoint(d, cIntN, cFractN);
	}

	explicit PTFixedPtBase(unsigned intN, unsigned fractN, 
						   const PTFixedPtBase &r)
	:	cIntN(intN), cFractN(fractN)
	{
		check();
		init(r);
	}

	const PTFixedPtBase&
	operator =(const PTFixedPtBase &r)
	{
		return init(r);
	}

	std::string toBinary() const
	{
		return toBinary(m_val, cIntN, cFractN);
	}

	double toDouble() const
	{
		return toDouble(m_val, cIntN, cFractN);
	}

	operator double() const
	{
		return toDouble();
	}

	unsigned getNumBits() const {return cIntN + cFractN;}

	static T getValidMask(unsigned n)
	{
		return (n == cNumBits) ? ~(T)0 : ~((T)~0 << n);
	}

	T getValidMask() const
	{
		return getValidMask(getNumBits());
	}

public:
	T getVal() const
	{
		return m_val;
	}

	const unsigned	cIntN, cFractN;

	const static unsigned cNumBits = sizeof(T) * 8;

protected:
	void setVal(T v)
	{
		ASSERT_TRUE(0 == v);
		m_val = v;
	}

private:
	T	m_val;	

	const PTFixedPtBase& init(const PTFixedPtBase &r);

	void check()
	{
		if (cNumBits < (cIntN + cFractN))
		{
			const_cast<unsigned&>(cFractN) = cNumBits - cIntN;
		}
		ASSERT_TRUE(cNumBits >= (cIntN + cFractN));
	}
};

// To convert from 64 to 32
const PTFixedPtBase<TInt32>
to32(const PTFixedPtBase<TInt64>& r);

// Fwd declare
template<unsigned INTN, unsigned FRACTN>
class TFixedPt32;

template<unsigned INTN, unsigned FRACTN>
class TFixedPt64;

template<unsigned INTN, unsigned FRACTN>
class TFixedPt32 : public PTFixedPtBase<TInt32>
{
public:
	explicit TFixedPt32()
	:	PTFixedPtBase<TInt32>(INTN, FRACTN)
	{}

	TFixedPt32(double d)
	:	PTFixedPtBase<TInt32>(INTN, FRACTN, d)
	{}

	explicit TFixedPt32(TInt32 v)
	:	PTFixedPtBase<TInt32>(INTN, FRACTN, v)
	{}

	TFixedPt32(const PTFixedPtBase<TInt32> &r)
	:	PTFixedPtBase<TInt32>(INTN, FRACTN, r)
	{}

	const PTFixedPtBase<TInt32>&
	operator =(const PTFixedPtBase<TInt32> &r)
	{
		if (this != &r)
		{
			PTFixedPtBase<TInt32>::operator=(r);
		}
		return *this;
	}

	const PTFixedPtBase<TInt32>& operator +=(const PTFixedPtBase<TInt32> &r);

	TFixedPt32(const PTFixedPtBase<TInt64> &r);

	const PTFixedPtBase<TInt32>& operator =(const PTFixedPtBase<TInt64> &r);
};

template<unsigned INTN, unsigned FRACTN>
class TFixedPt64 : public PTFixedPtBase<TInt64>
{
public:
	explicit TFixedPt64()
	:	PTFixedPtBase<TInt64>(INTN, FRACTN)
	{}

	TFixedPt64(double d)
	:	PTFixedPtBase<TInt64>(INTN, FRACTN, d)
	{}

	explicit TFixedPt64(TInt64 v)
	:	PTFixedPtBase<TInt64>(INTN, FRACTN, v)
	{}

	TFixedPt64(const PTFixedPtBase<TInt64> &r)
	:	PTFixedPtBase<TInt64>(INTN, FRACTN, r)
	{}

	const PTFixedPtBase<TInt64>&
	operator =(const PTFixedPtBase<TInt64> &r)
	{
		if (this != &r)
		{
			PTFixedPtBase<TInt64>::operator=(r);
		}
		return *this;
	}

	const PTFixedPtBase<TInt64>& operator +=(const PTFixedPtBase<TInt64> &r);
};

template<unsigned INTN, unsigned FRACTN>
TFixedPt32<INTN, FRACTN>
operator +(const TFixedPt32<INTN, FRACTN> &a, const TFixedPt32<INTN, FRACTN> &b)
{
	TInt32 r = a.getVal() + b.getVal();
	return TFixedPt32<INTN, FRACTN>(r);
}

template<unsigned INTN, unsigned FRACTN>
const PTFixedPtBase<TInt32>& 
TFixedPt32<INTN, FRACTN>::operator +=(const PTFixedPtBase<TInt32> &r)
{
	this->operator=(*this + r);
	return *this;
}

template<unsigned INTN, unsigned FRACTN>
TFixedPt64<INTN, FRACTN>
operator +(const TFixedPt64<INTN, FRACTN> &a, const TFixedPt64<INTN, FRACTN> &b)
{
	TInt64 r = a.getVal() + b.getVal();
	return TFixedPt64<INTN, FRACTN>(r);
}

template<unsigned INTN, unsigned FRACTN>
const PTFixedPtBase<TInt64>& 
TFixedPt64<INTN, FRACTN>::operator +=(const PTFixedPtBase<TInt64> &r)
{
	this->operator=(*this + r);
	return *this;
}

#if 0	//ambiguous w/ 64-bit
template<unsigned INTN, unsigned FRACTN>
PTFixedPtBase<TInt32>
operator *(const TFixedPt32<INTN, FRACTN> &a, const TFixedPt32<INTN, FRACTN> &b)
{
	TInt32 r = a.getVal() * b.getVal();
	return PTFixedPtBase<TInt32>(2*INTN, 2*FRACTN, r);
}
#endif

template<unsigned INTN, unsigned FRACTN>
PTFixedPtBase<TInt64>
operator *(const TFixedPt32<INTN, FRACTN> &a, const TFixedPt32<INTN, FRACTN> &b)
{
	TInt64 al = a.getVal(), bl = b.getVal();
	TInt64 r = al * bl;
	return PTFixedPtBase<TInt64>(2*INTN, 2*FRACTN, r);
}

template<unsigned INTN, unsigned FRACTN>
PTFixedPtBase<TInt64>
operator *(const TFixedPt64<INTN, FRACTN> &a, const TFixedPt64<INTN, FRACTN> &b)
{
	TInt64 al = a.getVal(), bl = b.getVal();
	TInt64 r = al * bl;
	return PTFixedPtBase<TInt64>(2*INTN, 2*FRACTN, r);
}

template<typename T>
std::string 
PTFixedPtBase<T>::toBinary(T x, unsigned intN, unsigned fractN)
{
	std::string rval = "";
	T mask = (T)1 << (intN + fractN - 1);
	for (int i = intN + fractN; i > 0; i--, mask >>= 1)
	{
		rval += (x & mask) ? "1" : "0";
	}
	return rval;
}

template<typename T>
const PTFixedPtBase<T>&
PTFixedPtBase<T>::init(const PTFixedPtBase &r)
{
	if (this != &r)
	{
		m_val = r.m_val;
		if (0 == m_val)
			return *this;
		// align to decimal point
		int shiftBy = cFractN - r.cFractN;
		if (0 < shiftBy)
		{
			// e.g: [?.2]:*.ff -> [?.4]:*.ff00
			m_val <<= shiftBy;
		}
		else if (0 > shiftBy)
		{
			// e.g: [?.4]:*.ffff -> [?.2]:*.ff
			m_val = asr(m_val, -shiftBy);	
			//TODO: report loss of precision
		}
		// Sign-extend
		//TODO: check/report loss of intp precision
		T mask = getValidMask();
		if (0 > m_val)
			m_val |= ~mask;
		else
			m_val &= mask;
	}
	return *this;
}

template<unsigned INTN, unsigned FRACTN>
TFixedPt32<INTN, FRACTN>::TFixedPt32(const PTFixedPtBase<TInt64> &r)
:	PTFixedPtBase<TInt32>(INTN, FRACTN, to32(r))
{}

template<unsigned INTN, unsigned FRACTN>
const PTFixedPtBase<TInt32>&
TFixedPt32<INTN, FRACTN>::operator =(const PTFixedPtBase<TInt64> &r)
{
	PTFixedPtBase<TInt32> v32(to32(r));
	PTFixedPtBase<TInt32>::operator=(v32);
	return *this;
}

};

#endif	//_xyzzy_fixedpt_hxx_
