#pragma once 

#include <knet/aio/IoService.h>
#include <knet/message/Message.h>
#include <knet/message/MessageListener.h>
#include <knet/tcp/impl/TcpConnection.h>
#include <knet/tcp/impl/Acceptor.h>
#include <knet/tcp/impl/Connector.h>
#include <knet/socket/Socket.h>
#include <knet/NetSecurity.h>

#include <map>

namespace gk
{
/**
 * @class TcpCommunicator
 *
 * Maintains a list of connected connections
 * and communicates with those connections.
 */
class TcpCommunicator 
{
public:
    TcpCommunicator();
    virtual ~TcpCommunicator();

	/**
	 * Initialize with upper layer and io service
	 *
	 * @param listener The listener to notify messages
	 * @param ios IoService for this communicator for underlying IO
	 * @return true if successful
	 */
    bool Init( MessageListener* listener, IoService* ios );

	/**
	 * Start listen on addr 
	 *
	 * @param addr The ip:port to listen on
	 * @param sl The security level. 
	 * @return true if successful
	 */
	bool Listen( const IpAddress& addr, SecurityLevel sl );

	/**
	 * Stop listen on addr
	 *
	 * @param addr The ip:port to stop listen
	 */
	void StopListen( const IpAddress& addr ); 

	/**
	 * Connect to remote. The result is notified with a NetStateMessage
	 *
	 * @param remote The ip:port to connect to
	 */
	void Connect( const IpAddress& remote );

	/**
	 * Close a connection 
	 *
	 * @param connectionId The connection id to close.
	 */
	void Close( uint connectionId );

    /**
     * Send a message to the connection. 
	 *
	 * @param connectionId The tcp connection id
	 * @param m The message to send
     */
    void Send( uint connectionId, MessagePtr m );

	/**
	 * Send a message to several connections 
	 *
	 * @param connections The list of connection ids to send to 
	 * @param m The message to send
	 */
    void Send( const std::vector<uint> connections, MessagePtr m );

    /**
     * Tick function. Not a thread function.
     */
    void Run();

	/**
	 * Called when to notify message. 
	 *
	 * Used by connections to notify message to communicator
	 *
	 * @param m The message to notify 
	 */
	void Notify( MessagePtr m );

    /**
     * Clean up 
     */
    void Fini();

	/**
	 * Find connection. Used only by NetClient or NetServer
	 *
	 * @param id The connection id to find
	 * @return The TcpConnection found. 0 if not found.
	 */
	TcpConnection* FindById( uint id );


private:
	typedef std::map<uint, TcpConnection*> ConnectionMap;
	typedef std::map<uint, Acceptor*> AcceptorMap;

	void processConnections();
	void processMessages();

	void onStateMessage( MessagePtr m );
	void onRelayMessage( MessagePtr m );

	void onNewConnection( Socket* s, SecurityLevel sl, bool accepted );

	void cleanupMessages();
	void cleanupConnections();
	void cleanupAcceptors();

private:
	MessageListener* m_listener;
	IoService* 		 m_ios;

    ConnectionMap   m_connections;      // normal connections including udp
	AcceptorMap 	m_acceptors;
	Connector 		m_connector;		// A threaded connector

	MessageQ 		m_messages;
	uint 			m_nextConnectionId;
};

} // gk

