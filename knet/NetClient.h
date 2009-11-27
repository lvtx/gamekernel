#pragma once 

#include <kcore/sys/Thread.h>
#include <knet/message/MessageListener.h>
#include <knet/tcp/TcpCommunicator.h>
#include <knet/udp/UdpCommunicator.h>

namespace gk 
{

struct UdpSend 
{
	MessagePtr 	m;
	int 		qos;
	bool 		broadcast;

	UdpSend()
		: qos( Message::RELIABLE )
		, broadcast( false )
	{
	}
};

/**
 * @class NetClient
 *
 * NetClient has TcpCommunicator and UdpCommunicator for TCP and UDP communication
 *
 * [1] Send and receive messages over TCP connections 
 * [2] Processes UDP group management and initiates P2P communication. 
 * [3] Processes UDP messages over UDP connections or relay connection.
 */
class NetClient : public Thread, public MessageListener
{
public:
	NetClient();
	~NetClient();

	/**
	 * Init with a MessageListener
	 *
	 * @param listener The MessageListener to notify
	 * @return true if successful
	 */
	bool Init( MessageListener* listener );

	/**
	 * Connect to NetServer
	 *
	 * @param addr The ip:port to connect
	 */
	void Connect( const IpAddress& addr );

	/**
	 * Close a TCP connection 
	 *
	 * @param connectionId The connection id to close
	 */
	void Close( uint connectionId );

	/**
	 * Send message over tcp
	 *
	 * @param m Message to send
	 */
	void Send( MessagePtr m );

	/**
	 * Send message over udp. There can be only one Udp group
	 *
	 * @param m Message to send
	 * @param qos The quality of service level
	 */
	void SendUdp( MessagePtr m, int qos = Message::RELIABLE );
	void BroadcastUdp( MessagePtr m, int qos = Message::RELIABLE );

	/**
	 * MessageListener::Notify to get notified
	 */
	void Notify( MessagePtr m );

	/**
	 * Thread::Run
	 */
	int Run();

	/**
	 * Clean up
	 */
	void Fini();

private:
	typedef Queue<UdpSend, Mutex> UdpSendQ;

	void processRecvQ();
	void processSendQ();
	void processUdpSendQ();

	void onControl( MessagePtr m );
	void onGroupPrepare( MessagePtr m );
	void onGroupJoin( MessagePtr m );
	void onGroupLeave( MessagePtr m );
	void onGroupDestroy( MessagePtr m );

private:
	IoService 		m_ios;
	TcpCommunicator m_tcp;
	UdpCommunicator m_udp;

	MessageListener*	m_listener;
	MessageQ			m_recvQ; 		// from notify
	MessageQ			m_sendQ; 		// from send

	UdpSendQ 			m_udpSendQ; 	// from send udp
	uint 				m_groupId; 		// current udp group
	uint 				m_selfTag; 		// my tag when joined
	uint 				m_processedCount;
};

} // gk 

