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

#if	!defined(_xyzzy_shmalloc_hxx_)
#	 define  _xyzzy_shmalloc_hxx_

#include <cstdlib>	//ptrdiff_t
#include <string>
#include "xyzzy/assert.hxx"
#include "xyzzy/portable.hxx"

using namespace std;

namespace xyzzy
{

/**
 *	A singleton class of shared memory allocator.
 */
class TShmAlloc
{
public:
	// key is form "/name" (as in shm_open)
	explicit TShmAlloc(const char *key, size_t size, bool create = true);

	// keyAndSize is string: "key,size"
	explicit TShmAlloc(const char *keyAndSize, bool create = true);

	// Return pointer (offset) to alloc requested chunk.
	ptrdiff_t alloc(size_t);

	// Return pointer: base + offset
	void* ptr(ptrdiff_t offset) const  
	{
		return (TUint8*)m_shmBase + offset;
	}

	void* base() const { return m_shmBase; }

	static TShmAlloc* theOne() { return m_stTheOne; }

	void destroy();

	const char* getKey() const {return m_key.c_str();}

	size_t getMaxSize() const  {return m_maxSize;}

	~TShmAlloc() {destroy();}

private:
	int		m_schmid;
	void	*m_shmBase;
	size_t	m_maxSize;
	size_t	m_used;

	std::string	m_key;

	static TShmAlloc	*m_stTheOne;

	void init(const char *key, size_t size, bool create);
};

///All things related to shared memory in portable library
namespace shm
{
	inline
	ptrdiff_t alloc(size_t sz)
	{
		return TShmAlloc::theOne()->alloc(sz);
	}

	inline
	void* allocp(size_t sz)
	{
		return TShmAlloc::theOne()->ptr(alloc(sz));
	}

	inline
	ptrdiff_t offset(void* p)
	{
		return reinterpret_cast<TUint8*>(p) -
		       reinterpret_cast<TUint8*>(TShmAlloc::theOne()->base());
	}

	inline
	void destroy()
	{
		TShmAlloc::theOne()->destroy();
	}

	inline
	const char* getKey()
	{
		return TShmAlloc::theOne()->getKey();
	}

	inline
	size_t getMaxSize()
	{
		return TShmAlloc::theOne()->getMaxSize();
	}

};

template<typename T, typename Alloc>
T
castTo(ptrdiff_t offset)
{
	return reinterpret_cast<T>(Alloc::theOne()->ptr(offset));
}

template<typename T, typename Alloc = TShmAlloc>
class PTShmObj
{
public:
	explicit PTShmObj()
	:	m_offset(0)
	{}

	PTShmObj(T p)
	:	m_offset(shm::offset(p))
	{}

	PTShmObj(ptrdiff_t offset)
	:	m_offset(offset)
	{}

	const PTShmObj& operator=(ptrdiff_t offset)
	{
		const_cast<ptrdiff_t&>(m_offset) = offset;
		return *this;
	}

	const PTShmObj& operator=(const PTShmObj &r)
	{
		const_cast<ptrdiff_t&>(m_offset) = r.m_offset;
		return *this;
	}

	T operator()()
	{
		return castTo<T, Alloc>(m_offset);
	}

	const T operator()() const
	{
		return castTo<T, Alloc>(m_offset);
	}

	operator T()	{ return operator()(); }

	operator const T() const	{ return operator()(); }

private:
	const ptrdiff_t m_offset;
};

template<typename T, typename Alloc = TShmAlloc>
class PTShmObjAr
{
public:
	explicit PTShmObjAr(TUint32 num = 0)
	:	m_numObjs(0)
	{
		init(num);
	}

	const PTShmObjAr& operator=(TUint32 num)
	{
		init(num);
		return *this;
	}

	TUint32 length() const {return m_numObjs;}

	T& operator[](TUint32 ix) {return at(ix);}

	//const T& operator[](TUint32 ix) const {return at(ix);}

private:
	TUint32			m_numObjs;
	PTShmObj<T*>    m_pobjsAr;

	void init(TUint32 num)
	{
		ASSERT_TRUE(0 == m_numObjs);	//can't reallocate
		if (0 == num) 
			return;
		m_numObjs = num;
		m_pobjsAr = shm::alloc(sizeof(T) * m_numObjs);
	}

	T& at(TUint32 ix)
	{
		ASSERT_TRUE(m_numObjs > ix);
		T *arv = m_pobjsAr();
		return arv[ix];
	}
};

};

#endif	//_xyzzy_shmalloc_hxx_
