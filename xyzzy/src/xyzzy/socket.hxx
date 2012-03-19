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
#if !defined(_xyzzy_socket_hxx_)
#    define  _xyzzy_socket_hxx_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string>
#include "xyzzy/exception.hxx"
#include "xyzzy/refcnt.hxx"

namespace xyzzy
{

using namespace xyzzy;
using std::string;

class TSocket
{
public:
	static const size_t cDfltSz = 1024;

	class Exception : public TException
	{
	public:
		explicit Exception(const char *from);

		const int m_errno;

		virtual ~Exception();

	private:
		const char* mkMessage(const char *from);
	};

	class Connection : public TRcObj
	{
	public:
		explicit Connection(int id, size_t sz= cDfltSz);

		/**
	 	 *	Receive data from server.
	 	 *	@return pointer to buffer on success and fill nrd 
		 *	with number of chars read.  A terminating nul is written.
	 	 */
		const char* recv(ssize_t &nrd, bool nonBlocking = true) 
			throw(Exception);

		/**
	 	 *	Send data to server.
	 	 *	Use buf and len as buffer (to send) and length, respectively.
	 	 *  If len==0, treat buf as null terminated string.
	 	 *  @return true on success, false if non-blocking and op would block.
	 	 */
		bool send(const void *buf, size_t len = 0, bool nonBlocking = true) 
			throw(Exception);

		virtual ~Connection();

	private:
		const size_t	m_cBufSz;
		char*			mp_buf;
		int				m_id;
	};

	typedef PTRcObjPtr<Connection>	TRcConnection;

	virtual ~TSocket();

protected:
	explicit TSocket(u_int16_t port, string naddr = "127.0.0.1") 
		throw(Exception);

	const u_int16_t		m_port;
	const string		m_addr;
	int					m_sockId;
	struct sockaddr_in	m_sockAddr;

private:
	void init() throw(Exception);
};

class TServerSocket : public TSocket
{
public:
	explicit TServerSocket(u_int16_t port, size_t bufSz = cDfltSz)
		throw(Exception);

	// Simply listen/block, accept and return Connection
	TRcConnection accept() throw(Exception);

	virtual ~TServerSocket();

private:
	const size_t	m_cBufSz;
	int				m_sid;
};

class TClientSocket : public TSocket
{
public:
	explicit TClientSocket(u_int16_t port, string naddr = "127.0.0.1", 
						   size_t bufSz = cDfltSz) 
		throw(Exception);

	/**
	 *	Receive data from server.
	 *	@return pointer to buffer on success and fill nrd with number of chars
	 *  read.  A terminating nul is written.
	 */
	const char* recv(ssize_t &nrd, bool nonBlocking = true) 
		throw(Exception)
	{
		return m_conn->recv(nrd, nonBlocking);
	}

	/**
	 *	Send data to server.
	 *	Use buf and len as buffer (to send) and length, respectively.
	 *  If len==0, treat buf as null terminated string.
	 *  @return true on success, false if non-blocking and op would block.
	 */
	bool send(const void *buf, size_t len = 0, bool nonBlocking = true) 
		throw(Exception)
	{
		return m_conn->send(buf, len, nonBlocking);
	}

	virtual ~TClientSocket();

private:
	TRcConnection	m_conn;

	void init(size_t bufSz) throw(Exception);
};

}

#endif
