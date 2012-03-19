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
#include <cstdarg>
#include <cstdio>
#include "xyzzy/message.hxx"
#include "xyzzy/assert.hxx"

namespace xyzzy
{
	using namespace std;

	/*static*/ TMessage::TMsgMap TMessage::m_msgMap;

	/*static*/ ostream* TMessage::m_strms[] =
	{
		&cout,	//eInfo
		&cerr,	//eWarn
		&cerr,	//eError
		&cerr,	//eFatal
	};

	//static
	void
	TMessage::msg(ESeverity svr, unsigned code, ...)
	{
		static string pfxs[] = 
		{
			//Pad strings to match longest
			"Info", "Warning", "Error", "Fatal"
		};
		static char buf[1024];

		TMsgMap::const_iterator fmt = m_msgMap.find(code);
		ASSERT_TRUE(m_msgMap.end() != fmt);
		va_list ap;
		va_start(ap, code);
		vsprintf(buf, fmt->second.c_str(), ap);
		va_end(ap);
		*m_strms[svr] << pfxs[svr] << ": " << buf << endl;
	}

	void
	TMessage::addMsgs(unsigned code, string msg)
	{
        typedef pair<TMsgMap::iterator,bool> rtype;
        TMsgMap::value_type val(code, msg);
        rtype rval = m_msgMap.insert(val);
        ASSERT_TRUE(rval.second);
	}

	void 
	TMessage::addMsgs(string msgs[], unsigned code)
	{
		for (int i = 0; 0 != msgs[i].size(); )
		{
			addMsgs(code++, msgs[i++]);
		}
	}

	void 
	TMessage::setStream(ESeverity strm, ostream *os)
	{
		m_strms[strm] = os;
	}
};
