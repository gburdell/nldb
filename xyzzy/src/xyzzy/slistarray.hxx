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

#if	!defined(_xyzzy_slistarray_hxx_)
#	 define  _xyzzy_slistarray_hxx_

#include "xyzzy/slist.hxx"
#include "xyzzy/array.hxx"
#include "xyzzy/assert.hxx"

namespace xyzzy
{
	template<typename T>
	class PTSlistArray
	{
	public:
		explicit PTSlistArray()
		{
			m_udat.mp_list = new PTSlist<T>();
			m_isList = true;
		}

		PTSlist<T>& asList()
		{
			ASSERT_TRUE(isList());
			return m_udat->mp_list;
		}

		PTArray<T>& asArray()
		{
			if (isList())
			{
				PTSlist<T> *from = m_udat.mp_list;
				m_udat.mp_ar = new PTArray<T>(from->length());
				PTArray<T>::copy(from, m_udat.mp_ar);
				delete from;
				m_isList = false;
			}
			return *m_udat.mp_ar;
		}

		bool isList() const
		{
			return m_isList;
		}

		void append(T &dat)
		{
			ASSERT_TRUE(isList());
			m_udat.mp_list->append(dat);
		}

		~PTSlistArray()
		{
			if (m_isList)
				delete m_udat.mp_list;
			else
				delete m_udat.mp_ar;
		}

	private:
		union
		{
			PTSlist<T>	*mp_list;
			PTArray<T>	*mp_ar;
		}	m_udat;
		bool m_isList;
	};
};

#endif	//_xyzzy_slistarray_hxx_
