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

#if	!defined(_xyzzy_message_hxx_)
#	 define  _xyzzy_message_hxx_

#include <iostream>
#include <string>
#include <map>

namespace xyzzy
{
	using namespace std;

	/// General message class.
	class TMessage
	{
	public:
		enum ESeverity {eInfo = 0, eWarn, eError, eFatal};

		static void msg(ESeverity svr, unsigned code, ...);

		static void addMsgs(unsigned code, string msg);

		/// Add messages starting at code.  Stop at null ("") message.
		static void addMsgs(string msgs[], unsigned code = 1);

		static void setStream(ESeverity strm, ostream *os);

	private:
		typedef map<unsigned, string> TMsgMap;

		static TMsgMap	m_msgMap;

		/// Streams in order respecting ESeverity
		static ostream*	m_strms[];
	};
};

#endif	//_xyzzy_message_hxx_
