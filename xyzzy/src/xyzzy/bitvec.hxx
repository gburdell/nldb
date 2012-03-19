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
#if !defined(_xyzzy_bitvec_hxx_)
#    define  _xyzzy_bitvec_hxx_

#include "xyzzy/portable.hxx"
#include "xyzzy/exception.hxx"
#include "xyzzy/assert.hxx"
#include "xyzzy/util.hxx"

namespace xyzzy
{

//	Base class
class TBvBase
{
public:
	unsigned length() const
	{
		return length(lb, rb);
	}

	bool isDescending() const
	{
		return isDescending(lb, rb);
	}

	unsigned left() const
	{
		return lb;
	}

	unsigned right() const
	{
		return rb;
	}

	/**
	 *	Return offset of "right" from rb.
	 */
	unsigned offset(unsigned right) const;

	/**
	 *	Return true if valid bounds.
	 */
	bool checkBounds(unsigned left, unsigned right) const;

	static unsigned length(unsigned left, unsigned right);

	static bool isDescending(unsigned left, unsigned right)
	{
		return (left >= right);
	}

protected:
	explicit TBvBase(unsigned lb, unsigned rb)
	:	lb(lb), rb(rb)
	{}

	const unsigned 	lb, rb;

private:
};

/**
 *	A template is used since the "runtime/area cost" of virtual functions
 *	will be too much.
 *	
 */

/**
 *	This template defines methods which must be implemented by
 *  underlying types.
 */
template<typename T>
class PTBitVecMethods
{
public:
	/**
	 *	Initialize lhs to default for type T.
	 */
	static const T& assign(T& lhs);
	/**
	 *	Assign v to lhs subject to boundary constraints in bref.
	 */
	static const T& assign(T& lhs, const TBvBase& bref, const T v);
	/**
	 *	Assign v to lhs[rb:lb] subject to boundary constraints in bref.
	 */
	static T assign(T& lhs, const TBvBase& bref, const T& v,
			    	unsigned lb, unsigned rb);
	/**
	 *	Return subrange[lb:rb] subject to boundary constraints in bref.
	 */
	static T valueAt(const T& v, const TBvBase& bref,
				     unsigned lb, unsigned rb);
	/**
	 *	Return subrange[ix] subject to boundary constraints in bref.
	 */
	static bool valueAt(const T& v, const TBvBase& bref, unsigned ix);
	/**
	 * Return value to set stMaxBits.
	 */
	static unsigned getMaxBits();
	
	static const unsigned stMaxBits;

	static const T stAllOnes;

private:
	static T onesMask(unsigned lb, unsigned rb);
};

template<typename T>
class PTBitVec : public TBvBase
{
public:
	explicit PTBitVec(unsigned lb, unsigned rb)
	:	TBvBase(lb, rb)
	{
		invariant(PTBitVecMethods<T>::stMaxBits >= length());
		PTBitVecMethods<T>::assign(val);
	}

	explicit PTBitVec(unsigned len)
	:	TBvBase(len - 1, 0)
	{
		invariant(PTBitVecMethods<T>::stMaxBits >= length());
		PTBitVecMethods<T>::assign(val);
	}

	explicit PTBitVec(const PTBitVec& r)
	:	TBvBase(r.lb, r.rb)
	{
		invariant(PTBitVecMethods<T>::stMaxBits >= length());
		PTBitVecMethods<T>::assign(val, *this, r.val);
	}
	
	const PTBitVec& operator =(const PTBitVec& r)
	{
		//TODO: range check !
		val = r.val;
		return *this;
	}
	
	const PTBitVec& operator =(const T& v)
	{
		//TODO: valid range check
		PTBitVecMethods<T>::assign(val, *this, v);
		return *this;
	}
	
	/**
	 *	TAccessor allows type to be used as both lhs and rhs
	 *	of expressions.
	 */
	class TAccessor
	{
	private:
		explicit TAccessor(PTBitVec &r, unsigned lb, unsigned rb)
		:   ref(r), lb(lb), rb(rb)
		{}

	public:
		operator T() const
		{
			return PTBitVecMethods<T>::valueAt(ref.val, ref, lb, rb);
		}

		T operator =(const TAccessor& v)
		{
			//T vt = v;
			return PTBitVecMethods<T>::assign(ref.val, ref, v, lb, rb);
		}

		T operator =(const T& v)
		{
			return PTBitVecMethods<T>::assign(ref.val, ref, v, lb, rb);
		}

	private:
	    PTBitVec&	ref;
		unsigned    lb, rb;

	    friend class PTBitVec;
	};

	TAccessor operator ()(unsigned l, unsigned r)
	{
		return TAccessor(*this, l, r);
	}

	TAccessor operator ()(unsigned ix)
	{
		return TAccessor(*this, ix, ix);
	}

	TAccessor operator [](unsigned ix)
	{
		return operator()(ix);
	}

	operator T () const
	{
		return val;
	}

private:
	T	val;

	friend class TAccessor;
};

#define GEN_BOUNDS(lb_, rb_) (rb_ != ~0u) ? lb_ : (lb_-1), (rb_ != ~0u) ? rb_ : 0

/**
 *	Specialize for T=TUint32
 */
template<unsigned LB, unsigned RB = ~0>
class TBitVec : public PTBitVec<TUint32>
{
public:
	explicit TBitVec()
	:	PTBitVec<TUint32>(GEN_BOUNDS(LB,RB))
	{}

	/**
	 *	Conversion constructor, so can write
	 *		TBitVec<> a = 4;
	 */
	TBitVec(TUint32 n)
	:	PTBitVec<TUint32>(GEN_BOUNDS(LB,RB))
	{
		PTBitVec<TUint32>::operator =(n);
	}
	
	const TBitVec& operator =(TUint32 v)
	{
		PTBitVec<TUint32>::operator =(v);
		return *this;
	}
};

/**
 *	Specialize for T=TUint64
 */
template<unsigned LB, unsigned RB = ~0>
class TBitVec64 : public PTBitVec<TUint64>
{
public:
	explicit TBitVec64()
	:	PTBitVec<TUint64>(GEN_BOUNDS(LB,RB))
	{}

	/**
	 *	Conversion constructor, so can write
	 *		TBitVec64<> a = 4;
	 */
	TBitVec64(TUint64 n)
	:	PTBitVec<TUint64>(GEN_BOUNDS(LB,RB))
	{
		PTBitVec<TUint64>::operator =(n);
	}
	
	const TBitVec64& operator =(TUint64 v)
	{
		PTBitVec<TUint64>::operator =(v);
		return *this;
	}
};

#undef GEN_BOUNDS

// BEGIN: template definitions
template<typename T>
const T&
PTBitVecMethods<T>::assign(T& lhs)
{
	lhs = 0xdeadbeef;
	return lhs;
}

template<typename T>
T 
PTBitVecMethods<T>::onesMask(unsigned lb, unsigned rb)
{
	const unsigned n = TBvBase::length(lb, rb);
	return ~((stMaxBits == n) ? 0 : (stAllOnes << n));
}

template<typename T>
const T&
PTBitVecMethods<T>::assign(T& lhs, const TBvBase& bref, 
                           const T v)
{
	lhs = v & onesMask(bref.left(), bref.right());
	return lhs;
}

template<typename T>
T
PTBitVecMethods<T>::valueAt(const T& v, const TBvBase& bref,
				     	    unsigned lb, unsigned rb)
{
	bref.checkBounds(lb, rb);
	T rval = (v >> bref.offset(rb)) & onesMask(lb, rb);
	return rval;
}

template<typename T>
T
PTBitVecMethods<T>::assign(T& lhs, const TBvBase& bref, 
						   const T& v,
						   unsigned lb, unsigned rb)
{
	bref.checkBounds(lb, rb);
	T val = lhs;
	// zero out bit slots we will set
	unsigned offset = bref.offset(rb);
	ASSERT_TRUE(offset < stMaxBits);
	T mask = onesMask(lb, rb) << offset; // mask <= 1s covering [lb,rb]
	val &= ~mask;    // val <= zero out [lb,rb]
	T v2 = v;
	v2 <<= offset;  // ...v000
	v2 &= mask;
	val |= v2;
	lhs = val;
	return valueAt(lhs, bref, lb, rb);
}

template<typename T>
bool
PTBitVecMethods<T>::valueAt(const T& v, const TBvBase& bref,
	     		            unsigned ix)
{
	return (0 != valueAt(v, bref, ix, ix)) ? true : false;
}
// END: template definitions

};

#endif //defined(_xyzzy_bitvec_hxx_)
