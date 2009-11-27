#include "stdafx.h"

#include <kcore/corebase.h>

#include <knet/socket/IpAddress.h>
#include <kcore/util/StringUtil.h>

namespace gk {

IpAddress::IpAddress()
: ip_(_T("0.0.0.0")), 
  port_(0)
{
    ::memset( (void*)&inaddr_, 0, sizeof( sockaddr_in ) );
}

IpAddress::~IpAddress()
{
	ip_.clear();
}

bool 
IpAddress::Init( const tstring& addr )
{
    std::vector<tstring> rv;

    StringUtil::Split( addr, _T(":"), rv );

    if ( rv.size() < 2 )  // parse error
    {
        return false; 
    }

    StringUtil::Trim( rv[0], ip_ );

    tstring sport;

    StringUtil::Trim( rv[1], sport );

    int port = StringUtil::ToInteger( sport );

    if ( port > ( 1 << 16 ) )
    {
        return false; // port too big
    }

    port_ = (ushort)( port & 0xFFFF );

    int len = sizeof( SOCKADDR );

    int rc = WSAStringToAddress( 
                (LPTSTR)ip_.c_str(), 
                AF_INET, 
                NULL, 
                (LPSOCKADDR)&inaddr_, 
                &len );

    inaddr_.sin_port = ::htons(port_);

    return ( rc == 0 );
}

bool
IpAddress::Init( const tstring& ip, ushort port )
{
    K_ASSERT( ip.length() > 0 );
    K_ASSERT( port > 0 );    

    ip_     = ip;
    port_   = port;

    int len = sizeof( SOCKADDR );

    int rc = WSAStringToAddress( 
        (LPTSTR)ip.c_str(), 
        AF_INET, 
        NULL, 
        (LPSOCKADDR)&inaddr_, 
        &len );

    inaddr_.sin_port = ::htons(port_);

    return ( rc == 0 );
}

bool 
IpAddress::Init( SOCKADDR* addr )
{
    K_ASSERT( addr != 0 );

    TCHAR buf[128];

    ::memset( buf, 0, 128*sizeof(TCHAR) );

    int len = 128;

    int rc = WSAAddressToString( 
                (LPSOCKADDR)addr, 
                sizeof( SOCKADDR ), 
                NULL, 
                (LPTSTR)buf, 
                (LPDWORD)&len );
                
    sockaddr_in* in = reinterpret_cast<sockaddr_in*>( addr );

    std::vector<tstring> rv;

    StringUtil::Split( buf, _T(":"), rv );

    if ( rv.size() < 2 )  // parse error
    {
        ip_ = buf;
    }
    else
    {
        StringUtil::Trim( rv[0], ip_ );
    }

	rv.clear();

    port_   = ::ntohs(in->sin_port);

    ::memcpy( (void*)&inaddr_, (void*)addr, sizeof( sockaddr_in ) );

    return ( rc ==  0 );
}

IpAddress::IpAddress( const IpAddress& rhs )
{
    ip_     = rhs.ip_;
    port_   = rhs.port_;
    
    ::memcpy( (void*)&inaddr_, (void*)&rhs.inaddr_, sizeof( sockaddr_in ) );
}

const IpAddress& 
IpAddress::operator = ( const IpAddress& rhs )
{
    ip_     = rhs.ip_;
    port_   = rhs.port_;
    
    ::memcpy( (void*)&inaddr_, (void*)&rhs.inaddr_, sizeof( sockaddr_in ) );

    return *this;
}

const tstring 
IpAddress::ToString() const
{
    TCHAR w[128];

	::memset( w, 0, sizeof(TCHAR)*128 );

    _sntprintf_s( w, 128, _T("%s:%d"), ip_.c_str(), port_ );

    return w;
}

} // gk
