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

#if	!defined(_xyzzy_autoptr_hxx_)
#	 define  _xyzzy_autoptr_hxx_

namespace xyzzy
{
	template<class T>
	class PTAutoPtr
	{
	public:
		explicit PTAutoPtr(T* p, bool owns = true)
		:	m_p(p), m_owns(owns)
		{}

		PTAutoPtr(const PTAutoPtr<T> &r)
		{
			init(r);
		}

		const PTAutoPtr& operator=(const PTAutoPtr &r)
		{
			destroy();
			init(r);
			return *this;
		}

		T* get() const
		{
			return m_p;
		}

		T* operator->()
		{
			return get();
		}

		T& operator*() const
		{
			return *m_p;
		}

		~PTAutoPtr()
		{
			destroy();
		}

	private:
		void init(const PTAutoPtr &r)
		{
			m_p = r.m_p;
			m_owns = r.m_owns;
			PTAutoPtr &rr = const_cast<PTAutoPtr&>(r);
			rr.m_owns = false;
			rr.m_p = 0;
		}

		void destroy()
		{
			if (m_owns)
			{
				delete m_p;
				m_p = 0;
				m_owns = false;
			}
		}

		T		*m_p;
		bool	m_owns;
	};
};

#endif	//_xyzzy_autoptr_hxx_
