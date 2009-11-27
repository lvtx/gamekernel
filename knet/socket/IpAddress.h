#pragma once 

#include <string>

namespace gk 
{
/**
 * @class IpAddress
 *
 * Wraps ip address
 */
class IpAddress 
{
public:
    IpAddress();
	~IpAddress();

    /**
     * Initialize with ip:port format 
	 *
	 * @param addr The string of ip:port
	 * @return true if successful
     */
    bool Init( const tstring& addr );

    /**
     * Initialize with ip and port
	 *
	 * @param ip The ip address
	 * @param port The port 
     */
    bool Init( const tstring& ip, ushort port );

    /**
     * Initialize with SOCKADDR 
	 *
	 * @param addr The socket address 
     */
    bool Init( SOCKADDR* addr );

    /**
     * Copy constructor 
     */
    IpAddress( const IpAddress& rhs );
    
    /**
     * Assignment operator 
     */
    const IpAddress& operator = ( const IpAddress& rhs );

    /**
     * Get ip only
     */
    const tstring& GetIp() const;
    
    /**
     * Get port only
     */
    ushort GetPort() const;

    /**
     * Convert to a string
	 *
	 * @return ip:port string
     */
    const tstring ToString() const;

    /**
     * Gets inet address structure 
     */
    SOCKADDR* GetInetAddr() const;

	/**
	 * Gets ulong representation of sockaddr_in 
	 *
	 * NOTE: This is unique on one machine for a quite useful time period.
	 *       Use this only on client.
	 */
	ulong GetKey() const;

private:
    tstring ip_;
    ushort port_;

    sockaddr_in inaddr_; // must be valid after Initialized
};

inline
const tstring& 
IpAddress::GetIp() const
{
    return ip_;
}

inline
ushort 
IpAddress::GetPort() const
{
    // port_ can be zero when not initialized.
    return port_;
}

inline
SOCKADDR*
IpAddress::GetInetAddr() const
{
    return (SOCKADDR*)&inaddr_;
}

inline
ulong 
IpAddress::GetKey() const
{
	return inaddr_.sin_addr.S_un.S_addr;
}

inline
bool 
operator==( const IpAddress& l, const IpAddress& r )
{
    return ( l.GetIp() == r.GetIp() && l.GetPort() == r.GetPort() );
}

inline
bool 
operator!=( const IpAddress& l, const IpAddress& r )
{
    return !operator==( l, r );
}

} // gk 

