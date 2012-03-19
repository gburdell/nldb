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
#if !defined(_xyzzy_delta_hxx_)
#    define  _xyzzy_delta_hxx_

#include <utility>
#include "xyzzy/slist.hxx"
#include "xyzzy/array.hxx"

namespace xyzzy
{

using std::pair;

class TDeltaDouble
{
public:
	static void setTrace(bool on);

	static void dumpTrace(const char *fname);

protected:
	static void addDelta(double d);

	static bool	m_stDoTrace;

private:
	static PTSlist<double>	m_stList;	
};

template<typename TR>
class PTDelta : private TDeltaDouble
{
public:
	explicit PTDelta()
	{}

	PTDelta(double gold, TR rev)
	:	m_dat(gold, rev)
	{
		delta(*this);
	}

	PTDelta(double gold)
	:	m_dat(gold, gold)
	{}

	const PTDelta&
	operator =(double gold)
	{
		m_dat.first = gold;
		m_dat.second = gold;
		return *this;
	}

	const PTDelta&
	operator =(const PTDelta& r)
	{
		if (this != &r)
		{
			m_dat.first = r.gold();
			m_dat.second = r.rev();
			delta(*this);
		}
		return *this;
	}

	double gold() const
	{
		return m_dat.first;
	}

	TR rev() const
	{
		return m_dat.second;
	}

	static void toDouble(const PTArray<PTDelta<TR> > &r, PTArray<double> &dbl, 
				         bool rev = true)
	{
		const int N = r.length();
		for (int i = 0; i < N; i++)
		{
			const PTDelta<TR> &ri = r[i];
			dbl[i] = rev ? (double)ri.rev() : (double)ri.gold();
			if (rev && m_stDoTrace)
			{
				delta(ri);
			}
		}
	}

	const PTDelta& operator +=(const PTDelta& r);

	const PTDelta& operator *=(const PTDelta& r);

private:
	pair<double, TR>	m_dat;

	static void delta(const PTDelta &r)
	{
		if (m_stDoTrace)
			addDelta(r.rev() - r.gold());
	}
};

template<typename TR>
PTDelta<TR>
operator +(const PTDelta<TR> &a, const PTDelta<TR> &b)
{
	double y2 = a.gold() + b.gold();
	TR y1 = a.rev() + b.rev();
	return PTDelta<TR>(y2, y1);
}

template<typename TR>
PTDelta<TR>
operator *(const PTDelta<TR> &a, const PTDelta<TR> &b)
{
	double y2 = a.gold() * b.gold();
	TR y1 = a.rev() * b.rev();
	return PTDelta<TR>(y2, y1);
}

template<typename TR>
const PTDelta<TR>& 
PTDelta<TR>::operator +=(const PTDelta& r)
{
	this->operator=(*this + r);
	return *this;
}

template<typename TR>
const PTDelta<TR>& 
PTDelta<TR>::operator *=(const PTDelta& r)
{
	this->operator=(*this * r);
	return *this;
}

}
#endif	//_xyzzy_delta_hxx_
