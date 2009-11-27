#pragma once 

#include <kcore/sys/Tick.h>
#include <knet/NetServer.h>
#include <knet/message/MessageListener.h>
#include <knet/NetSecurity.h>
#include <kserver/cell/Cell.h>
#include <kserver/db/TransactionServer.h>
#include <kserver/async/AsyncPool.h>

class TiXmlElement;

namespace gk {

struct NodeState
{
	enum State 
	{
		  INIT 			// initial state
		, CONNECTING 	// connecting from self
		, CONNECTED  	// connected from self or from remote
		, UP 			// up by heartbeat (CmNodeState)
	};

	ushort 		nodeId;
	tstring 	name;
	uint   		connectionId;
	IpAddress 	addr;
	State 		state;

	NodeState()
	: nodeId( 0 )
	, name()
	, connectionId( 0 )
	, addr()
	, state( INIT )
	{
	}
};

typedef stdext::hash_map<uint, NodeState> NodeStateList;

/**
 * @class Node 
 * 
 * Responsibility:
 *  [1] Initialize NetServer 
 *  [2] Listen for communication 
 *  [3] Connects to other Nodes 
 *  [4] Sends node information to other nodes (Heart beat)
 *  [5] Starts local level 0 Cells
 *  [6] Monitors level 0 Cells
 *  [7] Give processing power to level 0 passive Cells
 *  [8] Sends cell messages to other nodes 
 *  [9] Delivers cell messages to local cells 
 */
class Node : public Thread, public MessageListener
{
public:
	Node();
	~Node();

	/**
	 * Initialize. xe points to node. 
	 * 
	 */
	bool Init( TiXmlElement* xe, bool luaMode = false );

	/**
	 * Thread::Run
	 */
	int Run();

	/**
	 * Send to other node with cell address
	 */
	void Send( MessagePtr m ); 

	/**
	 * MessageListener::Notify
	 *
	 * Called from NetServer
	 */
	void Notify( MessagePtr m );

	/**
	 * Request a transaction to transaction server
	 * @param trans A Transaction
	 */
	void Request( Transaction* trans );

	/**
	 * Called back from TransactionRunner
	 */
	void Completed( Transaction* trans );

	/// Udp { 
	uint CreateUdpGroup( SecurityLevel sl = SECURITY0 );
	void JoinUdpGroup( uint groupId, uint connectionId, const tstring& extra );
	void LeaveUdpGroup( uint groupId, uint connectionId );
	void DestroyUdpGroup( uint groupId );
	/// }

	/**
	 * Cleans up 
	 */
	void Fini();

	/** 
	 * Has node with name
	 */
	bool HasNode( const tstring& name ) const;

	/** 
	 * Get a node state 
	 */
	const NodeState& GetNode( const tstring& name ) const;
	const NodeState& GetSelf() const;

	/// Get async pool
	AsyncPool* GetAsyncPool();

private:
	typedef Queue<Transaction*, Mutex> TransQ;

	void processCells();
	void processCluster(); 	// process state (reconnect, heartbeat)
	void processRecvQ(); 	// process received messages
	void processSendQ(); 	// process sent messages
	void processTrans(); 	// process completed transactions

	void broadcast( MessagePtr m );
	void unicast( ushort node, MessagePtr m );

	void onStateMessage( MessagePtr m );
	void onNodeState( MessagePtr m );
	void onCellState( MessagePtr m );
	void onCellMessage( MessagePtr m );

	void onTcpOpen( uint connectionId, const IpAddress& addr );
	void onTcpConnectFail( const IpAddress& addr );
	void onTcpClosed( uint connectionId );

	bool loadNode( TiXmlElement* xe );
	bool loadDb( TiXmlElement* xe );
	bool loadMembers( TiXmlElement* xe );
	bool loadMember( TiXmlElement* xe );
	bool loadCells( TiXmlElement* xe );
	bool loadCell( TiXmlElement* xe );

private:
	bool			m_luaMode;
	NetServer 		m_net;
	NodeState 		m_self;
	NodeStateList  	m_nodes;

	CellTree  		m_celltree;
	CellList  		m_cells;		// level 0 cells

	MessageQ 		m_recvQ;
	MessageQ 		m_sendQ;

	uint			m_recvCount;
	uint			m_sendCount;

	Tick 			m_tickState;
	Tick			m_tickStatus;
	Atomic<uint> 	m_processCount;

	TransactionServer m_transServer;
	TransQ 			  m_transQ;

	AsyncPool 		m_asyncPool;
};

inline
bool 
Node::HasNode( const tstring& name ) const
{
	NodeStateList::const_iterator i( m_nodes.begin() );
	NodeStateList::const_iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		const NodeState& ns = i->second;

		if ( ns.name == name )
		{
			return true;
		}
	}

	return false;
}

inline
const NodeState&
Node::GetNode( const tstring& name ) const
{
	static NodeState null;

	NodeStateList::const_iterator i( m_nodes.begin() );
	NodeStateList::const_iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		const NodeState& ns = i->second;

		if ( ns.name == name )
		{
			return ns;
		}
	}

	return null;
}

inline
const NodeState& 
Node::GetSelf() const
{
	return m_self;
}

inline
AsyncPool* 
Node::GetAsyncPool()
{
	return &m_asyncPool;
}

} // gk
