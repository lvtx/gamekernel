#pragma once 

#include <kcore/sys/Lock.h>
#include <knet/aio/IoAgent.h>
#include <knet/aio/IoService.h>
#include <knet/cipher/Cipher.h>
#include <knet/message/Message.h>
#include <knet/message/MessageListener.h>
#include <knet/NetSecurity.h>
#include <knet/socket/Socket.h>

#include <hash_map>
#include <bitset>

namespace gk {

class UdpConnection;
class TcpConnection;

/**
 * @class UdpCommunicator
 *
 * Opens one UDP port and communicates on that connection
 *
 * Locking:
 *  m_connLock is for accessing connection containers.
 *  OnRecvCompleted competes on connection containers.
 */
class UdpCommunicator : public IoAgent 
{
public:
	UdpCommunicator();
	~UdpCommunicator();

	/**
	 * Initialize udp communication on a single port.
	 * UdpCommunicator is created for a single port.
	 *
	 * @param listener - message listener
	 * @param ios   - IoService 
	 * @param addr  - Listening udp port
	 * @param sl    - security level 
	 * @param challenge - challenge text to initialize cipher
	 * @return true - if successful
	 */
	bool Init( MessageListener* listener, 
			   IoService* ios, 
			   const IpAddress& addr,
			   uint selfTag, 
			   SecurityLevel sl = SECURITY0, 
			   byte* challenge = 0, 
			   TcpConnection* relay = 0 );

	/**
	 * Open a new connection to ep 
	 *
	 * @param tag - application provided unique tag to identify udp node
	 * @param in - internal target address to connect to
	 * @param ex - external target address to connect to
	 * @return internal connection id 
	 */
	uint Connect( uint remoteTag, const IpAddress& in, const IpAddress& ex );

	/**
	 * Close a connection. local to UdpCommunicator. 
	 *
	 * @param connectionId Internal connection id
	 */
	void Close( uint tag );

	/**
	 * Close a connection. Local to UdpCommunicator
	 *
	 * @param tag The tag given by server
	 */
	void CloseByTag( uint tag );

	/**
	 * Called from outside. NetClient or NetServer
	 */
	void Run();

	/**
	 * Send a message to a connection
	 *
	 * @param localId : connection to send
	 * @param m       : message to send
	 */
	void Send( uint tag, MessagePtr m, int qos = Message::RELIABLE );
	void SendByTag( MessagePtr m, int qos = Message::RELIABLE );

	/**
	 * Broadcast message to all connected ones 
	 *
	 * @param m The message to broadcast
	 * @param qos The Quality of Service level
	 */
	void Broadcast( MessagePtr m, int qos = Message::RELIABLE );

	/**
	 * Called from UdpConnection when sending any packet 
	 */
	void SendTo( byte* data, uint len, const IpAddress& to );

	/**
	 * Called from UdpConnection or Reliable when segment arrives
	 */
	void OnRecv( uint remoteTag, void* data, uint len );

	/**
	 * Notify message to upper layer
	 */
	void Notify( MessagePtr m );

	/** 
	 * Check relay connection when a TCP connection is closed
	 */
	void CheckRelay( uint connectionId );

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
	 * Close all connections and cleanup
	 */
	void Fini();

	/**
	 * UdpConnection can use this.
	 *
	 * @return Underlying Socket
	 */
	Socket& GetSocket();

	/** 
	 * Set loss rate to a connection 
	 */
	void SetLossy( uint tag, uint rate );

	/**
	 * Find by key
	 *
	 * @param addrKey The key get by Socket::GetKey()
	 * @return UdpConnection found
	 */
	UdpConnection* FindByAddress( ulong addrKey );

	/**
	 * Find by udp connection by tag
	 *
	 * @param The tag given by server
	 * @return UdpConnection found
	 */
	UdpConnection* FindByTag( uint tag );

private:
	typedef stdext::hash_map<uint, UdpConnection*> ConnectionMap;

	enum 
	{
		MAX_CONNECTION_COUNT = 256	
	};

	void processConnections();
	void processErrorConnections();
	void processTickConnections();

	void send( UdpConnection* c, MessagePtr m, int qos );
	void multicast( const Message::RemoteList& tags, MessagePtr m, int qos );	

private:
	Socket 				m_socket;
	IoService* 			m_ios;
	MessageListener*	m_listener;
	TcpConnection* 		m_relay;
	uint 				m_selfTag;

	SecurityLevel 		m_sl;
	Cipher 				m_cipher;

	IoBlock 			m_recvBlock;
	BitStream 			m_recvBuffer;
	uint				m_recvCount;

	ConnectionMap 		m_connections;

	Mutex 				m_connLock; 
	Mutex				m_ioLock;

	std::bitset<MAX_CONNECTION_COUNT> m_ids;
};

inline
Socket& 
UdpCommunicator::GetSocket()
{
	return m_socket;
}

} // gk 
