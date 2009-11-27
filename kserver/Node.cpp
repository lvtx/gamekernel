#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/Node.h>

#include <kserver/cell/CellFactory.h>
#include <kserver/message/CmCellState.h>
#include <kserver/message/CmNodeState.h>
#ifdef USE_SQLAPI
#include <kserver/db/sqlapi/SaDbPool.h>
#endif 
#include <knet/message/MessageFactory.h>
#include <knet/message/net/NetStateMessage.h>
#include <kcore/xml/tinyxml.h>
#include <kcore/util/StringUtil.h>

namespace gk {

Node::Node()
: m_net()
, m_celltree()
, m_cells()
{
}

Node::~Node()
{
}

bool 
Node::Init( TiXmlElement* xe, bool luaMode )
{
	K_ASSERT( xe != 0 );

	m_luaMode = luaMode;

	bool rc = m_net.Init( this );

	if ( !rc )
	{
		LOG( FT_ERROR, _T("Node::Init> Cannot init NetServer") );

		return false;
	}

	(void)m_asyncPool.Init( this, m_luaMode ); // order befor loadCells.

	loadNode( xe );
	loadDb( xe );
	loadMembers( xe );
	loadCells( xe );

	REGISTER_MESSAGE( new CmNodeState );
	REGISTER_MESSAGE( new CmCellState );

	m_net.Listen( m_self.addr, SECURITY1 );

	m_recvCount = 0;
	m_sendCount = 0;

	Start();

	return IsRunning();
}

int 
Node::Run()
{
	m_tickStatus.Reset();

	while ( IsRunning() )
	{
		m_processCount = 0;

		processCells();
		processCluster();
		processRecvQ();
		processSendQ();

		processTrans();

		if ( m_processCount == (uint)0 )
		{
			::Sleep( 1 );
		}

		if ( m_tickStatus.Elapsed() > 1000 )
		{
			LOG( FT_SERVICE, _T("Node::Run> %d SPS %d RPS"), 
				 m_sendCount, m_recvCount );

			m_tickStatus.Reset();

			m_sendCount = 0;
			m_recvCount = 0;
		}
	}

	return 0;
}

void 
Node::Send( MessagePtr m )
{
	if ( m->remote > 0 || !m->remotes.empty() )
	{
		m_net.Send( m ); // send to directly to the known connection

		m_processCount.Inc();
	}
	else
	{
		m_sendQ.Put( m );
	}
}

void 
Node::Notify( MessagePtr m )
{
	m_recvQ.Put( m );
}

void 
Node::Request( Transaction* trans )
{
	K_ASSERT( trans != 0 );

	m_transServer.Prepare( trans );
	m_transServer.Request( trans );
}

void 
Node::Completed( Transaction* trans )
{
	m_transQ.Put( trans );
}

uint 
Node::CreateUdpGroup( SecurityLevel sl )
{
	return m_net.CreateUdpGroup( sl );
}

void 
Node::JoinUdpGroup( uint groupId, uint connectionId, const tstring& extra )
{
	m_net.JoinUdpGroup( groupId, connectionId, extra );
}

void 
Node::LeaveUdpGroup( uint groupId, uint connectionId )
{
	m_net.LeaveUdpGroup( groupId, connectionId );
}

void 
Node::DestroyUdpGroup( uint groupId )
{
	m_net.DestroyUdpGroup( groupId );
}

void 
Node::Fini()
{
	Stop();	

	LOG( FT_INFO, _T("Node::Fini> Node thread stopped") );

	CellList::iterator i( m_cells.begin() );	
	CellList::iterator iEnd( m_cells.end() );	

	for ( ; i != iEnd; ++i )
	{
		Cell* cell = i->second;
		K_ASSERT( cell != 0 );

		cell->Fini();

		LOG(FT_INFO, _T("Node::Fini> Cell %s finished"), cell->GetId().ToString().c_str() );

		if ( !m_luaMode )
		{
			delete cell;
		}
	}

	m_cells.clear();
	m_celltree.Fini();

	m_transServer.Fini();
	LOG(FT_INFO, _T("Node::Fini> Transaction server finished"));

	m_asyncPool.Fini();
	LOG(FT_INFO, _T("Node::Fini> AsyncPool finished"));

	m_net.Fini(); 

	LOG(FT_INFO, _T("Node::Fini> NetServer finished"));

	m_nodes.clear();

	Logger::Instance()->Fini();
}


void 
Node::processCells()
{
	CellList::iterator i( m_cells.begin() );	
	CellList::iterator iEnd( m_cells.end() );	

	for ( ; i != iEnd; ++i )
	{
		Cell* cell = i->second;
		K_ASSERT( cell != 0 );

		if ( !cell->IsActive() )
		{
			cell->Run(); // tick passive cells	
		}
	}
}

void 
Node::processCluster()
{
	if ( m_tickState.Elapsed() < 1000 )
	{
		return;
	}

	m_tickState.Reset();

	NodeStateList::iterator i( m_nodes.begin() );
	NodeStateList::iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		NodeState& ns = i->second;

		switch ( ns.state ) 
		{
		case NodeState::INIT:
			{
				if ( ns.nodeId < m_self.nodeId )
				{
					ns.state = NodeState::CONNECTING;

					m_net.Connect( ns.addr );
				}
			}
			break;
		case NodeState::CONNECTED:
		case NodeState::UP:
			{
				CmNodeState* m = new CmNodeState;

				m->remote = ns.connectionId;
				m->nodeId = m_self.nodeId;
				m->name   = m_self.name;
				m->addr	  = m_self.addr;

				m_net.Send( MessagePtr( m ) ); 

				LOG( FT_DEBUG, 
					 _T("Node::processCluster> Sending state to %d"), 
					 ns.nodeId );
			}
			break;
		}
	}
}

void 
Node::processRecvQ()
{
	MessagePtr m;

	while ( m_recvQ.Get( m ) )
	{
		m_processCount.Inc();

		switch ( m->type )
		{
		case NET_STATE_MESSAGE:
			onStateMessage( m );
			break;
		case CM_NODE_STATE:
			onNodeState( m );
			break;
		case CM_CELL_STATE:
			onCellState( m );
			break;
		default:
			onCellMessage( m );
			break;
		}

		++m_recvCount;
	}
}

void 
Node::processSendQ()
{
	MessagePtr m;

	while ( m_sendQ.Get( m ) )
	{
		m_processCount.Inc();

		if ( m->type <= CELL_MESSAGE_BEGIN )
		{
			LOG( FT_WARN, 
				_T("Node::processSendQ> Delayed message %d which is not cell message"), 
				m->type );

			return;
		}

		CellMessage* cm = static_cast<CellMessage*>( m.Get() );

		K_ASSERT( cm != 0 );

		if ( cm->dst.N == m_self.nodeId )
		{
			// local message 
			m_recvQ.Put( m );
		}
		else
		{
			if ( cm->dst.N == CellId::BROADCAST )
			{
				broadcast( m );
			}
			else
			{
				unicast( cm->dst.N, m );
			}
		}

		++m_sendCount;
	}
}

void 
Node::processTrans()
{
	Transaction* trans = 0;

	while ( m_transQ.Get( trans ) )
	{
		K_ASSERT( trans != 0 );

		const CellId& id = trans->GetCellId();	

		CellList::iterator i( m_cells.begin() );
		CellList::iterator iEnd( m_cells.end() );

		for ( ; i != iEnd; ++i )
		{
			Cell* cell = i->second;

			if ( cell->GetId().L0 == id.L0  )
			{
				cell->Completed( trans );
			}
		}
	}
}

void 
Node::broadcast( MessagePtr m )
{
	NodeStateList::iterator i( m_nodes.begin() );
	NodeStateList::iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		NodeState& ns = i->second;

		if ( ns.state == NodeState::UP )
		{
			m->remotes.push_back( ns.connectionId );
		}
	}

	if ( m->remotes.empty() )
	{
		LOG( FT_WARN, _T("Node::broadcast> Dest empty") );

		return;
	}

	m_net.Send( m );
}

void 
Node::unicast( ushort node, MessagePtr m )
{
	NodeStateList::iterator i( m_nodes.find( node ) );

	if ( i == m_nodes.end() )
	{
		LOG( FT_WARN, 
			 _T("Node::processSendQ> Dest %d not found"), 
			 node );

		return;
	}

	NodeState& ns = i->second;

	m->remote = ns.connectionId;

	if ( ns.state != NodeState::UP )
	{
		LOG( FT_WARN, 
			 _T("Node::processSendQ> Node %d is not up"), 
			 ns.nodeId );
	}
	else
	{
		m_net.Send( m );
	}
}

void 
Node::onStateMessage( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	NetStateMessage* sm = static_cast<NetStateMessage*>( m.Get() );

	switch ( sm->state )
	{
	case NetStateMessage::TCP_OPEN:
		{
			onTcpOpen( sm->connectionId, sm->addr );
		}
		break;
	case NetStateMessage::TCP_CONNECT_FAIL:
		{
			onTcpConnectFail( sm->addr );
		}
		break;
	case NetStateMessage::TCP_CLOSED:
		{
			onTcpClosed( sm->connectionId );

			// propagate to cells to handle clients
			CellList::iterator i( m_cells.begin() );
			CellList::iterator iEnd( m_cells.end() );

			for ( ; i != iEnd; ++i )
			{
				Cell* cell = i->second;

				cell->Notify( m );
			}
		}
		break;
	}
}

void 
Node::onNodeState( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	CmNodeState* cns = static_cast<CmNodeState*>( m.Get() );

	LOG( FT_INFO, 
		 _T("Node::onNodeState> Node %d is up"), 
		 cns->nodeId );

	NodeStateList::iterator i( m_nodes.begin() );
	NodeStateList::iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		NodeState& ns = i->second;

		if ( ns.nodeId == cns->nodeId )
		{
			ns.connectionId = m->remote; // update connection 
			ns.state = NodeState::UP;	

			return;
		}
	}
	
	NodeState nns;

	nns.nodeId 		 = cns->nodeId;
	nns.connectionId = cns->remote;
	nns.name 		 = cns->name;
	nns.state 		 = NodeState::UP;
	nns.addr 		 = cns->addr;

	LOG( FT_INFO, 
		 _T("Node::onNodeMessage> Remote node %s up"), 
		 nns.name.c_str() );

	m_nodes.insert( NodeStateList::value_type( nns.nodeId, nns ) );
}

void 
Node::onCellState( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	CmCellState* cs = static_cast<CmCellState*>( m.Get() );

	CellTree::CellNode n;

	n.id = cs->src;
	n.up = cs->up;

	// XXX: good for debugging, but do we really need this?
	m_celltree.Update( n );	 

	if ( cs->dst.L0 > 0 )
	{
		CellList::iterator i( m_cells.begin() );
		CellList::iterator iEnd( m_cells.end() );

		for ( ; i != iEnd; ++i )
		{
			Cell* cell = i->second;

			if ( cell->GetId().L0 == cs->dst.L0 || cs->dst.L0 == CellId::BROADCAST )
			{
				cell->Notify( m );
			}
		}
	}
}

void 
Node::onCellMessage( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	if ( m->type > CELL_MESSAGE_BEGIN )
	{
		CellMessage* cm = static_cast<CellMessage*>( m.Get() );

		K_ASSERT( cm != 0 );
		K_ASSERT( cm->dst.N == m_self.nodeId );

		bool handled = false;

		CellList::iterator i( m_cells.begin() );
		CellList::iterator iEnd( m_cells.end() );

		for ( ; i != iEnd; ++i )
		{
			Cell* cell = i->second;

			if ( cell->GetId().L0 == cm->dst.L0 || cm->dst.L0 == CellId::BROADCAST )
			{
				cell->Notify( m ); // slow propagation. fairness with active logic

				handled = true;
			}
		}

		if ( !handled )
		{
			LOG( FT_WARN, 
				 _T("Node::onCellMessage> Unhandled type %d dst L0 %d"), 
			     m->type, cm->dst.L0 );
		}
	}
	else
	{
		LOG( FT_WARN, _T("Node::onCellMessage> Unknown %d"), m->type );
	}
}

void 
Node::onTcpOpen( uint connectionId, const IpAddress& addr )
{
	K_ASSERT( connectionId > 0 );

	NodeStateList::iterator i( m_nodes.begin() );
	NodeStateList::iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		NodeState& ns = i->second;

		if ( ns.addr == addr )
		{
			LOG( FT_INFO, 
				 _T("Node::onTcpOpen> Node %s connected"), 
				 addr.ToString().c_str() );

			ns.connectionId = connectionId;
			ns.state 		= NodeState::CONNECTED;

			return;
		}
	}
}

void 
Node::onTcpConnectFail( const IpAddress& addr )
{
	NodeStateList::iterator i( m_nodes.begin() );
	NodeStateList::iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		NodeState& ns = i->second;

		if ( ns.addr == addr )
		{
			LOG( FT_INFO, 
				 _T("Node::onTcpConnectFail> Node %s failed to connect"), 
				 addr.ToString().c_str() );

			ns.state = NodeState::INIT;

			return;
		}
	}
}

void 
Node::onTcpClosed( uint connectionId )
{
	K_ASSERT( connectionId > 0 );

	NodeStateList::iterator i( m_nodes.begin() );
	NodeStateList::iterator iEnd( m_nodes.end() );

	for ( ; i != iEnd; ++i )
	{
		NodeState& ns = i->second;

		if ( ns.connectionId == connectionId )
		{
			LOG( FT_INFO, 
				 _T("Node::onTcpClosed> Node %s closed"), 
				 ns.addr.ToString().c_str() );

			ns.state = NodeState::INIT;

			return;
		}
	}
}

bool 
Node::loadNode( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	int id = 0;

	xe->Attribute( "id", &id );

	m_self.nodeId = (ushort)id;

	if ( id == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadNode> id %d is invalid"), id );

		return false;
	}

	const char* addr = xe->Attribute( "addr" );	

	if ( addr == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadNode> Address not found") );

		return false;
	}

	tstring waddr;

	StringUtil::ConvertToSystemCode( addr, waddr );	

	bool rc = m_self.addr.Init( waddr );

	if ( !rc )
	{
		LOG( FT_ERROR, 
			 _T("Node::loadNode> Address %s is invalid"), 
			 waddr.c_str() );

		return false;
	}

	const char* name = xe->Attribute( "name" );

	if ( name == 0 )
	{
		LOG( FT_ERROR, 
			_T("Node::loadNode> Name not found") );

		return false;
	}

	StringUtil::ConvertToSystemCode( name, m_self.name );

	return true;
}

bool 
Node::loadDb( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	// if we provide more db, then a factory is required
#ifndef USE_SQLAPI
	return true;
#else
	
	TiXmlElement* xpool = xe->FirstChildElement( "sadbpool" );

	if ( xpool == 0 )
	{
		return false;
	}

	const char* poolPath = xpool->Attribute( "cfg" );

	if ( poolPath == 0 )
	{
		return false;
	}

	tstring out;

	StringUtil::ConvertToSystemCode( poolPath, out );

	SaDbPool* pool = new SaDbPool();

	if ( !pool->Init( out ) )
	{
		delete pool;

		return false;
	}

	return m_transServer.Init( this, pool );
#endif
}

bool 
Node::loadMembers( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	TiXmlElement* xmembers = xe->FirstChildElement( "members" );

	if ( xmembers == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadMembers> Members not found") );

		return true;
	}

	TiXmlElement* xmember = xmembers->FirstChildElement( "node" );

	while ( xmember != 0 )
	{
		loadMember( xmember );

		xmember = xmember->NextSiblingElement( "node" );
	}

	return true;
}

bool 
Node::loadMember( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	NodeState node;

	int id = 0;

	xe->Attribute( "id", &id );

	node.nodeId = (ushort)id;

	if ( id == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadMember> id is invalid") );

		return false;
	}

	const char* addr = xe->Attribute( "addr" );	

	if ( addr == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadMember> addr not found") );

		return false;
	}

	tstring waddr;

	StringUtil::ConvertToSystemCode( addr, waddr );	

	node.addr.Init( waddr );

	const char* name = xe->Attribute( "name" );

	if ( name == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadMember> name not found") );

		return false;
	}

	StringUtil::ConvertToSystemCode( name, node.name );

	m_nodes.insert( NodeStateList::value_type( node.nodeId, node ) );

	LOG( FT_DEBUG, _T("Node::loadMember> %d loaded"), node.nodeId );

	return true;
}

bool 
Node::loadCells( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	TiXmlElement* xcells = xe->FirstChildElement( "cells" );

	if ( xcells == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadCells> Cells not found") );

		return true;
	}

	TiXmlElement* xcell = xcells->FirstChildElement( "cell" );

	while ( xcell != 0 )
	{
		loadCell( xcell );

		xcell = xcell->NextSiblingElement( "cell" );
	}

	return true;
}

bool 
Node::loadCell( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	const char* cls = xe->Attribute( "class" );

	if ( cls == 0 )
	{
		LOG( FT_ERROR, _T("Node::loadCell> class not found") );

		return false;
	}

	Cell* cell = CellFactory::Instance()->Create( cls );

	if ( cell == 0 )
	{
		tstring wcls;

		StringUtil::ConvertToSystemCode( cls, wcls );

		LOG( FT_ERROR, _T("Node::loadCell> class %s is invalid"), wcls.c_str() );

		return false;
	}

	int id = 0;

	xe->Attribute( "id", &id );

	if ( id <= 0 || id > 4096 )
	{
		LOG( FT_ERROR, _T("Node::loadCell> id %d is invalid"), id );

		return false;
	}

	const char* active = xe->Attribute( "active" );

	bool bactive = false;

	if ( active != 0 )
	{
		if ( _stricmp( active, "yes" ) == 0 )
		{
			bactive = true;
		}
	}

	CellId cellId;

	cellId.N  = m_self.nodeId;
	cellId.L0 = (ushort)id;

	bool rc = cell->Init( this, 0, cellId, xe, bactive );

	if ( !rc )
	{
		cell->Fini();

		LOG( FT_ERROR, _T("Node::loadCell> Cell init failed") );

		delete cell;

		return false;
	}

	m_cells.insert( CellList::value_type( cell->GetId(), cell ) );

	LOG( FT_DEBUG, 
		 _T("Node::loadCell> %d loaded"), 
		 cell->GetId().L0 );

	return true;
}

} // gk
