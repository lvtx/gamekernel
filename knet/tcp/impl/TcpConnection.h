#pragma once 

#include <kcore/sys/Lock.h>
#include <knet/aio/IoAgent.h>
#include <knet/cipher/Cipher.h>
#include <knet/message/BitStream.h>
#include <knet/message/Message.h>
#include <knet/socket/Socket.h>
#include <knet/NetSecurity.h>

#include <vector>

namespace gk
{

class TcpCommunicator;

/**
 * @class TcpConnection
 *
 * Wraps around Socket for buffered network IO.
 *
 * Protocol 
 *   LEN{16} CONTROL{8} 
 *  
 */
class TcpConnection : private Noncopyable, public IoAgent
{
public:
    TcpConnection();
    ~TcpConnection();
   
    /**
     * Initialize with transport. 
	 *
	 * @param communicator The TcpCommunicator to notify 
	 * @param id The connection id of this connection 
	 * @param socket The socket accepted or connected
	 * @param sl The security level
	 * @param accepted The flag for accepted or connected
     */
    bool Init( TcpCommunicator* communicator, uint id, Socket* socket, SecurityLevel sl, bool accepted );

	/**
	 * Start negotiating for security and other protocol parameters 
	 */
	void StartHandshake();

	/**
	 * Wait for accepted connection to negotiate protocol parameters
	 */
	void WaitHandshake();

    /**
     * Send message to remote
	 *
	 * @param m The message to send
     */
    void Send( MessagePtr m );

	/**
	 * Send alreay packed message. 
	 *
	 * NOTE: Only used for relay message.
	 *
	 * @param data The raw bytes to send
	 * @param len The bytes to send
	 */
	void Send( void* data, uint len );

    /**
     * Get tick for protocol processing
     */
    void Run();

	// IoAgent { 
	HANDLE RequestHandle();
	bool RequestSend(); 
	bool RequestRecv();
	void OnSendCompleted( IoBlock* io );
	void OnSendCompleted( IoBlock* io, uint bytesSent );
	void OnRecvCompleted( IoBlock* io );
	void OnIoError( int ec, IoBlock* io );
	// } 
	
	/**
	 * Close underlying socket
	 */
	void Close();
	
    /**
     * Cleanup 
     */
    void Fini();

    /**
     * Check error
     */
    bool HasError() const;

    /**
     * Get TcpConnection id
     */
    uint GetId() const;

    /**
     * Get reference count with send/recv request count
     */
    uint GetRequestCount();

    /**
     * Get socket
     */
    Socket* GetSocket();

	/**
	 * Set group of this connection 
	 *
	 * @param groupId The P2P group id.
	 */
	void SetGroup( uint groupId );

	/**
	 * Get group of this connection 
	 *
	 * @return The P2P group id 
	 */
	uint GetGroup() const;

private:
	enum 
	{
		  HEADER_LEN = 3 		 // 2 bytes len, 1 byte control
		, MAX_PACKET_LEN = 8192
	};

	MessagePtr buildMessage();
	void sendHandshake();

private:
	TcpCommunicator* 	m_communicator;
    uint             	m_id;
    Socket*         	m_socket;
	Atomic<int> 		m_error;
	SecurityLevel 		m_sl;
	bool 				m_accepted;

	Cipher				m_cipher;
	bool 				m_handshaking;

	Mutex				m_lockRecv;
	Mutex 				m_lockSend;
    Atomic<uint>		m_sendRequestCount;
    Atomic<uint>		m_recvRequestCount;
    BitStream 			m_sendBuffer1;
	BitStream 			m_sendBuffer2;
    BitStream 			m_recvBuffer;
	BitStream* 			m_accBuffer; 			// accumulation buffer
	IoBlock 			m_sendBlock;
	IoBlock 			m_recvBlock;

	uint 				m_groupId;
};

inline
uint 
TcpConnection::GetId() const
{
    return m_id;
}

inline
uint 
TcpConnection::GetRequestCount()
{
    return ( m_sendRequestCount + m_recvRequestCount );
}

inline
Socket* 
TcpConnection::GetSocket()
{
    return m_socket;
}

inline
bool 
TcpConnection::HasError() const
{
	return (m_error != 0);
}

inline
void 
TcpConnection::SetGroup( uint groupId )
{
	m_groupId = groupId;
}

inline
uint 
TcpConnection::GetGroup() const
{
	return m_groupId;
}

} // gk

