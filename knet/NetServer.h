#pragma once 

#include <kcore/sys/Thread.h>
#include <knet/message/MessageListener.h>
#include <knet/group/NetGroup.h>
#include <knet/group/NetGroupOp.h>
#include <knet/tcp/TcpCommunicator.h>
#include <knet/udp/UdpCommunicator.h>
#include <knet/NetSecurity.h>

namespace gk 
{
/**
 * @class NetServer 
 *
 * NetServer has TcpCommunicator to communicate over TCP connections. 
 *
 * [1] Send and receive messages over TCP connections 
 * [2] Processes UDP group management and initiates P2P communication. 
 * [3] Processes UDP relay messages
 *
 */
class NetServer : public Thread, public MessageListener
{
public:
	NetServer();
	~NetServer();

	/**
	 * Initialize NetServer 
	 *
	 * @param listener The listener to notify 
	 * @return true if successful
	 */
	bool Init( MessageListener* listener );

	/**
	 * Start listen on addr 
	 *
	 * @param addr The ip:port to listen 
	 * @param sl The security level 
	 */
	void Listen( const IpAddress& addr, SecurityLevel sl = SECURITY0 );

	/**
	 * Connect to other NetServer 
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
	 * @param 
	 */
	void Send( MessagePtr m );

	/**
	 * MessageListener::Notify 
	 */
	void Notify( MessagePtr m );

	/**
	 * Create an empty UDP group
	 */
	uint CreateUdpGroup( SecurityLevel sl = SECURITY0 );

	/**
	 * Join a UDP group. Messages will be delivered to connectionId
	 * for further UDP setup processing including hole punching and conneciton setup
	 */
	void JoinUdpGroup( uint groupId, uint connectionId, const tstring& extra );

	/**
	 * Leave a UDP group. Messages will be delivered to connectionId
	 * for futher UDP cleanup processing including connection teardown
	 */
	void LeaveUdpGroup( uint groupId, uint connectionId );

	/**
	 * Destroy a UDP group
	 */
	void DestroyUdpGroup( uint groupId );

	/**
	 * Thread::Run
	 */
	int Run();

	/**
	 * Clean up server completely 
	 */
	void Fini();

private:
	typedef stdext::hash_map<uint, NetGroup*> NetGroupMap;
	typedef Queue<NetGroupOp, Mutex> GroupOpQueue;

	void processRecvQ();
	void processSendQ();
	void processOpQ();

	void onControl( MessagePtr m );
	void onStateMessage( MessagePtr m );
	void onGroupPrepared( MessagePtr m );

	// group queue handlers {
	void onCreateUdpGroup( const NetGroupOp& op );
	void onJoinUdpGroup( const NetGroupOp& op );
	void onLeaveUdpGroup( const NetGroupOp& op );
	void onDestroyUdpGroup( const NetGroupOp& op );
	// }

	void cleanupGroups();

private:
	IoService 			m_ios;
	TcpCommunicator 	m_tcp;

	MessageListener* 	m_listener;
	MessageQ 			m_recvQ; 		// from notify
	MessageQ 			m_sendQ; 		// from send

	NetGroupMap 		m_groups;
	uint 				m_nextGroupId;
	GroupOpQueue 		m_groupQ;

	uint 				m_processedCount;
};

} // gk 
