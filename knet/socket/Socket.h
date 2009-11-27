#pragma once 

#include <kcore/base/Noncopyable.h>
#include <kcore/sys/Lock.h>
#include <kcore/sys/Atomic.h>
#include <knet/aio/IoBlock.h>
#include <knet/socket/IpAddress.h>

#include <string>

namespace gk {
/// used to pass data to Async functions

/**
 * @class Socket
 *
 * Wraps around windows socket 
 * 
 * - Normal socket operations
 * - Overlapped IOCP operations
 *
 * Note
 *  Socket strictly depends on Winsock2 functionality.
 *  GameKernel supposes Windows 2000 or higher to decrease platform 
 *  compatibility issues.
 */
class Socket 
{
public:
    Socket();

    ~Socket();

    /**
     * Startup winsock2
     */
    static bool Startup();

    /**
     * Clean up winsock2
     */
    static void Cleanup();

    /**
     * Create tcp socket with WSA_OVERLAPPED mode 
     */
    bool CreateTcpSocket();

    /**
     * Create udp socket with WSA_OVERLAPPED mode
     */
    bool CreateUdpSocket();

    /**
     * Bind tcp/udp socket 
     */
    bool Bind( const IpAddress& addr );

    /**
     * Listen on socket and return true when connected, false when failed
     */
    bool Listen();

    /**
     * Accepts a new socket
     */
    Socket* Accept();

    /**
     * Connect to address
     */
    bool Connect( const IpAddress& remote );

    /**
     * Recv from socket 
     */
    int Recv( byte* v, int count );

    /**
     * Send to socket 
     */
    int Send( byte* v, size_t count );
    
    /**
     * Receive with WSARecv. 
     */
    int AsyncRecv( IoBlock* buf );

    /**
     * Send with WSASend
     */
    int AsyncSend( IoBlock* buf );

    /**
     * Receive from remote (address filled) with recvfrom 
     */
    int RecvFrom( byte* v, int count, IpAddress& remote );

    /**
     * Send to remote with sendto
     */
    int SendTo( byte* v, int count, const IpAddress& remote );

    /**
     * Receive from remote ( address filled ) with WSARecvFrom
     */
    int AsyncRecvFrom( IoBlock* buf );

    /**
     * Send to remote with WSASendTo
     */
    int AsyncSendTo( IoBlock* buf );

    /**
     * Set non-blocking mode
     */
    bool SetNonblocking();

    /**
     * Set blocking mode
     */
    bool SetBlocking();

	/**
	 * Set ignore connection reset
	 */
	bool SetIgnoreUdpReset();
    
    /**
     * Close socket 
     */
    void Close();

    /**
     * Get socket
     */
    SOCKET GetSystemSocket();

    /**
     * Return address 
     */
    const IpAddress& GetAddress() const;

    /**
     * Return peer address when socket is valid. TCP only.
     */
    const IpAddress& GetPeerAddress();

    /**
     * Returns error string
     */
    const tstring& GetErrorDesc( int errorCode ); 

    /**
     * Returns last error 
     */
    int GetLastError();

private:
    enum Protocol
    {
        PT_UNKNOWN, 
        PT_TCP, 
        PT_UDP
    };

	static Atomic<int> started_;

    Protocol protocol_;
    SOCKET socket_;

    IpAddress addr_;
    IpAddress peerAddr_;

    tstring errorDesc_;
};

} // gk 

