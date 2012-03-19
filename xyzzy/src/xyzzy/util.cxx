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

#include <cstdio>
#include "xyzzy/util.hxx"

#if defined(_MSC_VER)
// sscanf deprecated
#	pragma warning( disable : 4996 )
#endif

namespace xyzzy
{
	using namespace std;

    unsigned int toUnsignedInt(const char* s)
	{
		unsigned int r;
		sscanf(s, "%u", &r);
		return r;
	}

	int toInt(const char* s)
	{
		int r;
		sscanf(s, "%d", &r);
		return r;
	}

	unsigned long int toUnsignedLongInt(const char* s)
	{
		unsigned long int r;
		sscanf(s, "%lu", &r);
		return r;
	}


	long int toLongInt(const char* s)
	{
		long int r;
		sscanf(s, "%ld", &r);
		return r;
	}

	TTicker::TTicker(unsigned tickIncr, 
					 unsigned tickPcnt,
					 ostream &os)
	: m_tickIncr(tickIncr),
	  m_tickPcnt(tickPcnt),
	  m_os(os),
	  m_pcnt(0),
	  m_tick(0)
	{}

	void 
	TTicker::tick()
	{
		if (m_tickIncr == ++m_tick)
		{
			m_pcnt += m_tickPcnt;
			m_os << m_pcnt << "% ";
			m_os.flush();
			m_tick = 0;
		}
	}

	static
	bool
	isLittleEndian(void)
	{
		const int sz = sizeof(unsigned);
		char buf[sz];
		buf[0] = 0x01;
		buf[1] = 0x23;
		for (int i = 2; i < sz; i++)
			buf[i] = 0;
		unsigned u = *(unsigned*)&buf;
		return (u == ((0x23 << 8) + 0x01));
	}

	static const bool stcIslittleEndian = isLittleEndian();

	int
	toLittleEndianHex(double d, char *buf)
	{
		static const char cHex[] = 
		{
			'0','1','2','3','4','5','6','7','8','9',
			'A','B','C','D','E','F'
		};
		const int sz = sizeof(d);
		const int sz2= sz*2;
		char endBuf[sz2];
		const char *p = (char*)&d;
		int ix;
		for (int i = 0, j = 0, k = sz-1; i < sz; i++, k--, j += 2)
		{
			ix = stcIslittleEndian ? i : k;
			endBuf[j] = cHex[0x0f & p[ix]];
			endBuf[j+1] = cHex[0x0f & (p[ix]>>4)];
		}
		char *q = endBuf;
		int i;
		for (i = 0; i < sz2; i++)
			buf[i] = q[i];
		buf[i] = '\0';
		return sz2+1;
	}
}
