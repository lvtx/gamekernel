#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/socket/Socket.h>
#include <kcore/base/PanicError.h>
#include <kcore/util/StringUtil.h>

namespace gk { 

Atomic<int> Socket::started_ = Atomic<int>( 0 );

bool 
Socket::Startup()
{
	if ( started_ > 0 )
	{
		started_.Inc();

		return true;
	}

	WSAData wd;

	WORD version = MAKEWORD( 2, 2 );

	if ( WSAStartup( version, &wd ) == 0 )
	{
		started_.Dec();
	}
	else
	{
		throw PanicError(_T("Cannot intialize winsock") );
	}

	return started_ > 0;
}

void 
Socket::Cleanup()
{
	if ( started_ == 0 )
	{
		// we allow unmatched cleanup 

		return;
	}

	started_.Dec();

	if ( started_ == 0 )
	{
		WSACleanup();
	}
}

Socket::Socket()
: protocol_(PT_UNKNOWN), 
  socket_(0)
{
}

Socket::~Socket()
{
    if ( socket_ )
    {
        Close();
    }
}

bool 
Socket::CreateTcpSocket()
{
    socket_ = ::WSASocket( 
                    AF_INET, 
                    SOCK_STREAM, 
                    IPPROTO_TCP, 
                    NULL, 
                    0, 
                    WSA_FLAG_OVERLAPPED );

    protocol_ = PT_TCP;

    return ( socket_ != INVALID_SOCKET );
}

bool 
Socket::CreateUdpSocket()
{
    socket_ = ::WSASocket( 
                    AF_INET, 
                    SOCK_DGRAM, 
                    IPPROTO_UDP, 
                    NULL, 
                    0, 
                    WSA_FLAG_OVERLAPPED );

    protocol_ = PT_UDP;

    return ( socket_ != INVALID_SOCKET );
}

bool 
Socket::Bind( const IpAddress& addr )
{
    K_ASSERT( socket_ != INVALID_SOCKET );

    if ( socket_ == INVALID_SOCKET )
	{
		return false;
	}

    addr_ = addr;

#if 0 // setting socket to reuse address is not that good.
    uint yes = 1;

    (void)::setsockopt( 
                socket_, 
                SOL_SOCKET, 
                SO_REUSEADDR, 
                (const char*)&yes, 
                sizeof(yes) );
#endif 

    int rc = ::bind( 
                socket_, 
                (SOCKADDR *)addr_.GetInetAddr(), 
                sizeof(sockaddr_in) );

    return ( rc == 0 );
}

bool 
Socket::Listen()
{
    K_ASSERT( socket_ != INVALID_SOCKET );

    int rc = ::listen( socket_, SOMAXCONN );

    return ( rc == 0 );
}

Socket* 
Socket::Accept()
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_TCP );

    sockaddr_in clientAddr;

    int clientAddrLen = sizeof( clientAddr );

    SOCKET acceptedSocket = ::accept( 
                                socket_, 
                                (SOCKADDR *)&clientAddr, 
                                &clientAddrLen );

    if ( acceptedSocket == 0 )
	{
		return (Socket*)0;
	}

    IpAddress addr;
    (void)addr.Init( (SOCKADDR*)&clientAddr );

    Socket* ns      = new Socket();

    // Init socket 
    ns->protocol_   = PT_TCP;         
    ns->socket_     = acceptedSocket;
    ns->addr_       = addr_;            // make accepted address as socket addr
    (void)ns->GetPeerAddress();

    return ns;
}

bool
Socket::Connect( const IpAddress& remote )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_TCP );

    int rc = ::connect(
                    socket_, 
                    (SOCKADDR *)remote.GetInetAddr(), 
                    sizeof(sockaddr_in) );

    if ( rc != SOCKET_ERROR )
    {
        sockaddr_in addrin;

        int len = sizeof( sockaddr_in );

        ::getsockname( socket_, (SOCKADDR*)&addrin, &len );

        (void)addr_.Init( (SOCKADDR*)&addrin );

        // Init peer address here.
        (void)this->GetPeerAddress();
    }

    return ( rc == 0 );
}

int 
Socket::Recv( byte* v, int count )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_TCP );

    K_ASSERT( v != 0 );
    K_ASSERT( count > 0 );

    return ::recv( socket_, (char*)v, count, 0 );
}

int 
Socket::Send( byte* v, size_t count )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_TCP );

    K_ASSERT( v != 0 );
    K_ASSERT( count > 0 );

    return ::send( socket_, (const char*)v, (int)count, 0 );
}

int 
Socket::AsyncRecv( IoBlock* buf )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_TCP );

    K_ASSERT( buf != 0 );
    K_ASSERT( buf->buf.buf >= 0 ); // we use 0 buffer recv 
    K_ASSERT( buf->buf.len >= 0 ); // we use 0 buffer recv
    K_ASSERT( buf->op == IoBlock::OP_READ );

    DWORD bytes = 0;
    DWORD flag = 0;

    ::memset( (void*)buf, 0, sizeof(OVERLAPPED) ); // clear!!!
    
    // if we don't clear, then ERROR_INVALID_HANDLE returns. 
    // This is Microsoft way of reporting error. 
    
    int rc = ::WSARecv( 
                    socket_,                    // socket handle
                    (LPWSABUF)&buf->buf,        // buffer
                    1, 
                    &bytes, 
                    &flag,                      // same as ::send flag
                    (LPOVERLAPPED)buf,          // 
                    0 );                        // do not use completion routine

    if ( rc != SOCKET_ERROR )
    {
        return 0;
    }

    int errorCode = WSAGetLastError();

    if ( errorCode == WSA_IO_PENDING )
    {
        return 0;
    }

    // let owner handles error

    return errorCode; 
}

int 
Socket::AsyncSend( IoBlock* buf )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_TCP );

    K_ASSERT( buf != 0 );
    K_ASSERT( buf->buf.buf != 0  );
    K_ASSERT( buf->buf.len > 0 ); // send must have data
    K_ASSERT( buf->op == IoBlock::OP_WRITE );

    DWORD bytes = buf->buf.len;
    DWORD flag = 0;

    ::memset( (void*)buf, 0, sizeof(OVERLAPPED) ); // clear!!!

    // if we don't clear, then ERROR_INVALID_HANDLE returns. 
    // This is Microsoft way of reporting error. 
    
    int rc = ::WSASend( 
                    socket_, 
                    (LPWSABUF)&buf->buf, 
                    1, 
                    &bytes, 
                    flag,                       // same as ::send flag
                    (LPOVERLAPPED)buf, 
                    0 );                        // do not use completion routine

    if ( rc != SOCKET_ERROR )
    {
        return 0;
    }

    int errorCode = WSAGetLastError();

    if ( errorCode == WSA_IO_PENDING )
    {
        return 0;
    }

    // let owner handle error

    return errorCode;    
}

int 
Socket::RecvFrom( byte* v, int count, IpAddress& remote )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( v != 0 );
    K_ASSERT( count > 0 );
    K_ASSERT( protocol_ == PT_UDP );

    sockaddr_in remoteAddr;

    int remoteAddrLen = sizeof( sockaddr_in );

    int rc = ::recvfrom( 
                    socket_, 
                    (char*)v, 
                    count, 
                    0, 
                    (SOCKADDR*)&remoteAddr, 
                    &remoteAddrLen );

    (void)remote.Init( (SOCKADDR*)&remoteAddr );

    return rc;
}

int 
Socket::SendTo( byte* v, int count, const IpAddress& remote )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( v != 0 );
    K_ASSERT( count > 0 );
    K_ASSERT( remote.GetPort() > 0 );
    K_ASSERT( protocol_ == PT_UDP );

    int rc = ::sendto( 
                    socket_, 
                    (const char*)v, 
                    count, 
                    0, 
                    (SOCKADDR*)remote.GetInetAddr(), 
                    sizeof( sockaddr_in ) );

    return rc;
}

int 
Socket::AsyncRecvFrom( IoBlock* buf )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_UDP );

    K_ASSERT( buf != 0 );
    K_ASSERT( buf->buf.buf >= 0 ); // we can use 0 buffer recv
    K_ASSERT( buf->buf.len >= 0 ); // we can use 0 buffer recv
    K_ASSERT( buf->op == IoBlock::OP_READ );

    DWORD bytes = 0;
    DWORD flag = 0;

    ::memset( (void*)buf, 0, sizeof(OVERLAPPED) ); // clear!!!

    // if we don't clear, then ERROR_INVALID_HANDLE returns. 
    // This is Microsoft way of reporting error. 
    
    int rc = ::WSARecvFrom( 
                    socket_, 
                    (LPWSABUF)&buf->buf, 
                    1, 
                    &bytes, 
                    &flag,          // for MSG_PARTIAL
                    (SOCKADDR*)&buf->remote, 
                    (LPINT)&buf->remoteLen,                 
                    (LPOVERLAPPED)buf, 
                    0 );            // do not use completion routine

    if ( rc != SOCKET_ERROR )
    {
        return 0;
    }

    int errorCode = WSAGetLastError();

    if ( errorCode == WSA_IO_PENDING )
    {
        return 0;
    }

    // let owner handles error

    return errorCode; 
}

int 
Socket::AsyncSendTo( IoBlock* buf )
{
    K_ASSERT( socket_ != INVALID_SOCKET );
    K_ASSERT( protocol_ == PT_UDP );
    
    K_ASSERT( buf != 0 );
    K_ASSERT( buf->buf.buf > 0 );
    K_ASSERT( buf->buf.len > 0 ); // send must have data
    K_ASSERT( buf->op == IoBlock::OP_WRITE );

    DWORD bytes = buf->buf.len;
    DWORD flag = 0;

    ::memset( (void*)buf, 0, sizeof(OVERLAPPED) ); // clear!!!

    // if we don't clear, then ERROR_INVALID_HANDLE returns. 
    // This is Microsoft way of reporting error. 

    int rc = ::WSASendTo( 
                    socket_, 
                    (LPWSABUF)&buf->buf, 
                    1, 
                    &bytes, 
                    flag,  
                    (SOCKADDR*)&buf->remote, 
                    buf->remoteLen,                   
                    (LPOVERLAPPED)buf, 
                    0 );        // do not use completion routine

    if ( rc != SOCKET_ERROR )
    {
        return 0;
    }

    int errorCode = WSAGetLastError();

    if ( errorCode == WSA_IO_PENDING )
    {
        return 0;
    }

    // let owner handles error

    return errorCode; 
}

bool 
Socket::SetNonblocking()
{
    K_ASSERT( socket_ != INVALID_SOCKET );

    if ( socket_ == 0 )
	{
		return false;
	}

    ulong ul = 1;

    int rc = ::ioctlsocket(socket_, FIONBIO, (unsigned long *) &ul);

    return ( rc == SOCKET_ERROR );
}

bool 
Socket::SetBlocking()
{
    K_ASSERT( socket_ != INVALID_SOCKET );

    if ( socket_ == 0 )
	{
		return false;
	}

    ulong ul = 0;

    int rc = ::ioctlsocket( socket_, FIONBIO, (unsigned long *) &ul );

    return ( rc == SOCKET_ERROR );
}

bool 
Socket::SetIgnoreUdpReset()
{
		int  bytes    = 0;
		bool behavior = false;
		
		// disable  new behavior using
		// IOCTL: SIO_UDP_CONNRESET
		//
		int rc = ::WSAIoctl( socket_, 
				           	 SIO_UDP_CONNRESET,
						   	 &behavior, 
							 sizeof(behavior),
							 NULL, 
							 0, 
							 (LPDWORD)&bytes,
							 NULL, 
							 NULL );

		return ( rc != SOCKET_ERROR ); 
};

void 
Socket::Close()
{
    if ( socket_ )
    {
        ::shutdown( socket_, SD_BOTH );

        ::closesocket( socket_ );

        socket_ = 0;
    }
}

SOCKET 
Socket::GetSystemSocket()
{
    return socket_;
}

const IpAddress& 
Socket::GetAddress() const
{
    return addr_;
}

const IpAddress& 
Socket::GetPeerAddress()
{
    K_ASSERT( protocol_ == PT_TCP );

    if ( peerAddr_.GetPort() > 0 ) // assume peerAddr_ is Initd.
    {
        return peerAddr_;
    }

    sockaddr_in addrin;

    int len = sizeof( sockaddr_in );

    if ( ::getpeername( socket_, (SOCKADDR*)&addrin, &len ) == 0 )
    {
        (void)peerAddr_.Init( (SOCKADDR*)&addrin );
    }

    return peerAddr_;
}

const tstring&
Socket::GetErrorDesc( int errorCode )
{
    LPTSTR msgBuf;

    errorDesc_.clear();

    ::FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode, 
            0,                      // Language. Not supported. 
            (LPTSTR)&msgBuf,
            0,
            NULL );

	if ( msgBuf != 0 )
	{
		errorDesc_.append( msgBuf );
	}

    StringUtil::Trim( errorDesc_ );

    LocalFree( msgBuf );

    return errorDesc_;
}

int 
Socket::GetLastError()
{
    return ::WSAGetLastError();
}

} // gk
