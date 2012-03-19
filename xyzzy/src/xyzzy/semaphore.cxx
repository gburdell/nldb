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

#include "xyzzy/assert.hxx"
#include "xyzzy/semaphore.hxx"

namespace xyzzy
{

TSemaphore::TSemaphore(int val, bool shared)
{
	sem_t *p = &m_sem;
	if (-1 == sem_init(p, shared?1:0, val))
	{
		die("sem_init");
	}
}

int 
TSemaphore::getValue() const
{
	sem_t *p = const_cast<sem_t*>(&m_sem);
	int rval = 0;
	if (-1 == sem_getvalue(p, &rval))
	{
		die("sem_getvalue");
	}
	return rval;
}

void 
TSemaphore::post()
{
	sem_t *p = &m_sem;
	if (-1 == sem_post(p))
	{
		die("sem_post");
	}
}

void 
TSemaphore::wait()
{
	sem_t *p = &m_sem;
	if (-1 == sem_wait(p))
	{
		die("sem_wait");
	}
}

};
