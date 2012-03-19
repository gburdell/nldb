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

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include "xyzzy/assert.hxx"
#include "xyzzy/shmalloc.hxx"

namespace xyzzy
{
#if !defined(CYGWIN)

using namespace std;

/*static*/ TShmAlloc *TShmAlloc::m_stTheOne = 0;

TShmAlloc::TShmAlloc(const char *key, size_t size, bool create)
{
    init(key, size, create);
}

TShmAlloc::TShmAlloc(const char *keyAndSize, bool create)
{
    static const char *const delim = ",";
    char buf[strlen(keyAndSize)+1];
    strcpy(buf, keyAndSize);
    const char *key = strtok(buf, delim);
    int sz = atoi(strtok(NULL, delim));
    init(key, sz, create);
}

void
TShmAlloc::init(const char *key, size_t size, bool create)
{
    ASSERT_TRUE(0 == m_stTheOne);
    int m_schmid = shm_open(key, O_RDWR | ((create) ? O_CREAT : 0), S_IRWXU);
    if (-1 == m_schmid)
        die("shmget");
    if (-1 == ftruncate(m_schmid, size))
        die("ftruncate");
    void *pm = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, m_schmid, 0);
    if (MAP_FAILED == pm)
        die("mmap");
    m_maxSize = size;
    m_used = 0;
    m_shmBase = pm;
    m_stTheOne = this;
    m_key = key;
}

ptrdiff_t
TShmAlloc::alloc(size_t sz)
{
    ptrdiff_t r = m_used;
    m_used += sz;
    ASSERT_TRUE(m_maxSize >= m_used);
    return r;
}

void
TShmAlloc::destroy()
{
    if (-1 == shm_unlink(m_key.c_str()))
    {
        if (ENOENT != errno)
            die("shm_unlink");
    }
#ifdef DEBUG
    cout << "DEBUG: shm_unlink(\"" << m_key << "\")" << endl;
#endif
}

#endif  //CYGWIN
};

