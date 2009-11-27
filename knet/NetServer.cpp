#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/NetServer.h>

#include <kcore/sys/Logger.h>
#include <knet/group/NmGroupPrepare.h>
#include <knet/group/NmGroupPrepared.h>
#include <knet/group/NmGroupJoin.h>
#include <knet/group/NmGroupJoined.h>
#include <knet/group/NmGroupLeave.h>
#include <knet/group/NmGroupDestroy.h>
#include <knet/group/NmGroupRelay.h>
#include <knet/message/MessageFactory.h>
#include <knet/message/net/NetControlMessage.h>
#include <knet/message/net/NetStateMessage.h>
#include <knet/socket/Socket.h>

namespace gk { 

NetServer::NetServer()
: m_ios()
, m_tcp()
, m_listener( 0 )
, m_recvQ()
, m_sendQ()
, m_groups()
, m_nextGroupId( 1 )
, m_processedCount( 0 )
{
}

NetServer::~NetServer()
{
}

bool 
NetServer::Init( MessageListener* listener )
{
	K_ASSERT( listener != 0 );

	Socket::Startup();

	m_listener = listener;

	bool rc = m_ios.Init();

	if ( !rc )
	{
		LOG( FT_ERROR, _T("NetServer::Init> IoService init failed") );

		return false;
	}

	rc = m_tcp.Init( this, &m_ios );

	if ( !rc )
	{
		LOG( FT_ERROR, _T("NetServer::Init> TcpCommunicator init failed") );

		return false;
	}

	MessageFactory::Instance()->Register( new NmGroupPrepared );
	MessageFactory::Instance()->Register( new NmGroupRelay );

	Start();

	return true;
}

void 
NetServer::Listen( const IpAddress& addr, SecurityLevel sl )
{
	NetControlMessage* ncm = new NetControlMessage;

	ncm->control = NetControlMessage::TCP_LISTEN;
	ncm->remote  = addr;
	ncm->sl		 = sl;

	this->Notify( MessagePtr( ncm ) );
}

void 
NetServer::Connect( const IpAddress& addr )
{
	NetControlMessage* ncm = new NetControlMessage;

	ncm->control = NetControlMessage::TCP_CONNECT;
	ncm->remote  = addr;

	this->Notify( MessagePtr( ncm ) );
}

void 
NetServer::Close( uint connectionId )
{
	NetControlMessage* ncm = new NetControlMessage;

	ncm->control 		= NetControlMessage::TCP_CLOSE;
	ncm->connectionId 	= connectionId;

	this->Notify( MessagePtr( ncm ) );
}

void 
NetServer::Send( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	m_sendQ.Put( m );
}

void 
NetServer::Notify( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );
	
	switch ( m->type )
	{
	case NET_CONTROL_MESSAGE:
	case NET_GROUP_PREPARED:
	case NET_STATE_MESSAGE:
		m_recvQ.Put( m );
		break;
	default:
		m_listener->Notify( m );
		break;
	}
}

uint 
NetServer::CreateUdpGroup( SecurityLevel sl )
{
	NetGroupOp op;

	op.groupId 	= m_nextGroupId++;
	op.sl 		= sl;
	op.op 		= NetGroupOp::CREATE;

	m_groupQ.Put( op );

	return op.groupId;
}

void 
NetServer::JoinUdpGroup( uint groupId, uint connectionId, const tstring& extra )
{
	NetGroupOp op;

	op.groupId 		= groupId;
	op.connectionId = connectionId;
	op.op 			= NetGroupOp::JOIN;
	op.extra		= extra;
	
	m_groupQ.Put( op );
}

void 
NetServer::LeaveUdpGroup( uint groupId, uint connectionId )
{
	NetGroupOp op;

	op.groupId 		= groupId;
	op.connectionId = connectionId;
	op.op 			= NetGroupOp::LEAVE;
	
	m_groupQ.Put( op );
}

void 
NetServer::DestroyUdpGroup( uint groupId )
{
	NetGroupOp op;

	op.groupId 		= groupId;
	op.op 			= NetGroupOp::DESTROY;
	
	m_groupQ.Put( op );
}

int 
NetServer::Run()
{
	LOG( FT_INFO, _T("NetServer::Run> Started") );

	while ( IsRunning() )
	{
		m_processedCount = 0;

		m_tcp.Run();				// tick 

		processRecvQ();				
		processSendQ();
		processOpQ();

		if ( m_processedCount == 0 )
		{
			::Sleep( 1 );
		}
	}

	LOG( FT_INFO, _T("NetServer::Run> Stopped") );

	return 0;
}

void 
NetServer::Fini()
{	
	m_ios.Fini();  // TcpConnection::Fini requires IOCP to finish 

	LOG( FT_DEBUG, _T("NetServer::Fini> IoService finished") );

	m_tcp.Fini();

	LOG( FT_DEBUG, _T("NetServer::Fini> TcpCommunicator finished") );

	Stop();

	LOG( FT_DEBUG, _T("NetServer::Fini> Thread finished") );

	cleanupGroups();

	Socket::Cleanup();
}
	
void 
NetServer::processRecvQ()
{
	MessagePtr m;

	while ( m_recvQ.Get( m ) )
	{
		switch ( m->type )
		{
		case NET_CONTROL_MESSAGE:
			onControl( m );
			break;
		case NET_STATE_MESSAGE:
			onStateMessage( m );
			break;
		case NET_GROUP_PREPARED:
			onGroupPrepared( m );
			break;
		default:
			m_listener->Notify( m );
			break;
		}

		++m_processedCount;
	}
}

void 
NetServer::processSendQ()
{
	MessagePtr m;

	while ( m_sendQ.Get( m ) )
	{
		if ( m->remote > 0 )
		{
			m_tcp.Send( m->remote, m );
		}

		if ( !m->remotes.empty() )
		{
			m_tcp.Send( m->remotes, m );
		}

		++m_processedCount;
	}
}

void
NetServer::processOpQ() 
{
	NetGroupOp op;

	while ( m_groupQ.Get( op ) )
	{
		switch ( op.op )
		{
		case NetGroupOp::CREATE:
			onCreateUdpGroup( op );
			break;
		case NetGroupOp::JOIN:
			onJoinUdpGroup( op );
			break;
		case NetGroupOp::LEAVE:
			onLeaveUdpGroup( op );
			break;
		case NetGroupOp::DESTROY:
			onDestroyUdpGroup( op );
			break;
		}
	}
}

void 
NetServer::onControl( MessagePtr m )
{
	NetControlMessage* cm = static_cast<NetControlMessage*>( m.Get() );

	switch ( cm->control )
	{
	case NetControlMessage::TCP_CONNECT:
		{
			m_tcp.Connect( cm->remote );
		}
		break;
	case NetControlMessage::TCP_LISTEN:
		{
			m_tcp.Listen( cm->remote, cm->sl );
		}
		break;
	case NetControlMessage::TCP_CLOSE:
		{
			m_tcp.Close( cm->connectionId );
		}
		break;
	}
}

void 
NetServer::onStateMessage( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	NetStateMessage* nsm = static_cast<NetStateMessage*>( m.Get() );

	switch ( nsm->state )
	{
	case NetStateMessage::TCP_CLOSED:
		{
			NetGroupMap::iterator i = m_groups.find( nsm->groupId );

			if ( i != m_groups.end() )
			{
				NetGroup* group = i->second;

				if ( group != 0 )
				{
					group->Leave( nsm->connectionId );
				}
			}	
		}
		break;
	}

	m_listener->Notify( m );
}

void 
NetServer::onGroupPrepared( MessagePtr m )
{
	NmGroupPrepared* gp = static_cast<NmGroupPrepared*>( m.Get() );

	NetGroupMap::iterator i = m_groups.find( gp->groupId );

	if ( i == m_groups.end() )
	{
		LOG( FT_WARN, _T("NetServer::onGroupPrepared> %d not found"), gp->groupId );

		// TODO: error report

		return;
	}	

	NetGroup* group = i->second;
	K_ASSERT( group != 0 );

	group->OnPrepared( gp->connectionId, gp->in );
}

void 
NetServer::onCreateUdpGroup( const NetGroupOp& op )
{
	NetGroup* group = new NetGroup;

	group->Init( &m_tcp, op.groupId, op.sl );

	m_groups.insert( NetGroupMap::value_type( group->GetId(), group ) );

	LOG( FT_DEBUG, _T("NetServer::onCreateUdpGroup> %d created"), group->GetId() );
}

void
NetServer::onJoinUdpGroup( const NetGroupOp& op )
{
	NetGroupMap::iterator i = m_groups.find( op.groupId );

	if ( i == m_groups.end() )
	{
		LOG( FT_WARN, _T("NetServer::onJoinUdpGroup> %d not found"), op.groupId );

		// TODO: error report

		return;
	}

	TcpConnection* c = m_tcp.FindById( op.connectionId );

	if ( c == 0 )
	{
		LOG( FT_WARN, 
			 _T("NetServer::onJoinUdpGroup> Connection %d not found"), 
			 op.connectionId );

		return;
	}

	NetGroup* group = i->second;
	K_ASSERT( group != 0 );

	c->SetGroup( op.groupId );

	(void)group->Join( op.connectionId, c->GetSocket()->GetPeerAddress(), op.extra );

	LOG( FT_DEBUG, 
		 _T("NetServer::onJoinUdpGroup> %d joined to %d"), 
		 op.connectionId, group->GetId() );
}

void 
NetServer::onLeaveUdpGroup( const NetGroupOp& op )
{
	NetGroupMap::iterator i = m_groups.find( op.groupId );

	if ( i == m_groups.end() )
	{
		LOG( FT_WARN, _T("NetServer::onLeaveUdpGroup> %d not found"), op.groupId );

		return;
	}

	TcpConnection* c = m_tcp.FindById( op.connectionId );
	
	if ( c != 0 )
	{
		c->SetGroup( 0 );
	}

	NetGroup* group = i->second;
	K_ASSERT( group != 0 );

	group->Leave( op.connectionId );

	LOG( FT_DEBUG, 
		 _T("NetServer::onLeaveUdpGroup> %d left from %d"), 
		 op.connectionId, group->GetId() );
}

void 
NetServer::onDestroyUdpGroup( const NetGroupOp& op )
{
	NetGroupMap::iterator i = m_groups.find( op.groupId );

	if ( i == m_groups.end() )
	{
		LOG( FT_WARN, _T("NetServer::onDestroyUdpGroup> %d not found"), op.groupId );

		return;
	}

	NetGroup* group = i->second;
	K_ASSERT( group != 0 );

	// make TcpConnection to forget about group
	const std::vector<uint>& remotes = group->GetRemotes();

	std::vector<uint>::const_iterator ci( remotes.begin() );
	std::vector<uint>::const_iterator ciEnd( remotes.end() );

	for ( ; ci != ciEnd; ++ci )
	{
		TcpConnection* c = m_tcp.FindById( *ci );

		if ( c != 0 )
		{
			c->SetGroup( 0 );
		}
	}

	group->Fini();

	LOG( FT_DEBUG, 
		 _T("NetServer::onDestroyUdpGroup> %d destroyed"), 
		 group->GetId() );

	delete group;

	m_groups.erase( i );
}

void 
NetServer::cleanupGroups() 
{
	NetGroupMap::iterator i( m_groups.begin() );
	NetGroupMap::iterator iEnd( m_groups.end() );

	for ( ; i != iEnd; ++i )
	{
		NetGroup* group = i->second;
		K_ASSERT( group != 0 );

		group->Fini();

		delete group;
	}

	m_groups.clear();
}

} // gk
