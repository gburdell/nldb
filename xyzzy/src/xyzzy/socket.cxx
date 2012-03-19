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
#include <string.h>
#include <cstdio>
#include <errno.h>
#include "xyzzy/socket.hxx"

namespace xyzzy
{
#if !defined(CYGWIN)
TSocket::Exception::Exception(const char *from)
:   TException(mkMessage(from)),
    m_errno(errno)
{}

const char*
TSocket::Exception::mkMessage(const char *from)
{
    static char *buf = 0;
    const char *m = strerror(errno);
    size_t len = strlen(m) + strlen(from) + 8;
    delete buf;
    buf = new char[len];
    sprintf(buf, "%s: %s", from, m);
    return buf;
}

TSocket::Exception::~Exception()
{
    free(const_cast<char*>(TException::m_reason));
}

TSocket::TSocket(u_int16_t port, string naddr) throw(Exception)
:   m_port(port),
    m_addr(naddr)
{
    init();
}

void
TSocket::init() throw(Exception)
{
    m_sockId = socket(PF_INET, SOCK_STREAM, 0);
    if (-1 == m_sockId)
    {
        throw Exception("socket");
    }
    int opt = 1;
    if (-1 == setsockopt(m_sockId, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        throw Exception("setsockopt");
    }
    int rval = inet_aton(m_addr.c_str(), &m_sockAddr.sin_addr);
    if (0 == rval)
    {
        throw Exception("inet_aton");
    }
    m_sockAddr.sin_family = AF_INET;
    m_sockAddr.sin_port   = htons(m_port);  //network byte order
}

TSocket::~TSocket()
{
    close(m_sockId);
}

TSocket::Connection::Connection(int id, size_t sz)
:   m_cBufSz(sz),
    m_id(id)
{
    mp_buf = new char[m_cBufSz];
}

const char*
TSocket::Connection::recv(ssize_t &nrd, bool nonBlocking)
    throw(Exception)
{
    ssize_t rsz = ::recv(m_id, mp_buf, m_cBufSz-1,
                         nonBlocking?MSG_DONTWAIT:0);
    if (-1 == rsz)
    {
        if (nonBlocking && (EAGAIN == errno))
        {
            nrd = 0;
            return 0;
        }
        throw Exception("recv");
    }
    mp_buf[rsz] = '\0';//nul terminate
    nrd = rsz;
    return mp_buf;
}

bool
TSocket::Connection::send(const void *buf, size_t len, bool nonBlocking)
    throw(Exception)
{
    if (0 == len)
    {
        len = 1 + strlen((const char*)buf);
    }
    ssize_t ssz = ::send(m_id, buf, len, nonBlocking?MSG_DONTWAIT:0);
    if (-1 == ssz)
    {
        if (nonBlocking && (EAGAIN == errno))
            return false;
        throw Exception("send");
    }
    return true;
}

TSocket::Connection::~Connection()
{
    ::close(m_id);
    delete mp_buf;
}

TServerSocket::TServerSocket(u_int16_t port, size_t bufSz) throw(Exception)
:   TSocket(port),
    m_cBufSz(bufSz)
{
    struct sockaddr *paddr = (struct sockaddr *) &m_sockAddr;
    m_sid = ::bind(m_sockId, paddr, sizeof(struct sockaddr_in));
    if (-1 == m_sid)
    {
        throw Exception("bind");
    }
}

TServerSocket::~TServerSocket()
{
    ::close(m_sid);
}

TSocket::TRcConnection
TServerSocket::accept() throw(Exception)
{
    if (-1 == listen(m_sockId, 1))
    {
        throw Exception("listen");
    }
    struct sockaddr_in saddr;
    socklen_t slen = sizeof(saddr);
    memset(&saddr, 0, slen);
    int sid = ::accept(m_sockId, (struct sockaddr*)&saddr, &slen);
    if (-1 == sid)
    {
        throw Exception("accept");
    }
    return new Connection(sid, m_cBufSz);
}

TClientSocket::TClientSocket(u_int16_t port, string naddr, size_t bufSz)
        throw(Exception)
:   TSocket(port, naddr)
{
    init(bufSz);
}

void
TClientSocket::init(size_t bufSz) throw(Exception)
{
    struct sockaddr *paddr = (struct sockaddr *) &m_sockAddr;

    int bindId = connect(m_sockId, paddr, sizeof(struct sockaddr_in));
    if (-1 == bindId)
    {
        throw Exception("connect");
    }
    m_conn = new Connection(m_sockId, bufSz);
}

TClientSocket::~TClientSocket()
{}

#endif  //CYGWIN
}

