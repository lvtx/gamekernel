#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/NetClient.h>

#include <kcore/sys/Logger.h>
#include <knet/group/NmGroupPrepare.h>
#include <knet/group/NmGroupPrepared.h>
#include <knet/group/NmGroupJoin.h>
#include <knet/group/NmGroupJoined.h>
#include <knet/group/NmGroupLeave.h>
#include <knet/group/NmGroupDestroy.h>
#include <knet/message/MessageFactory.h>
#include <knet/message/net/NetControlMessage.h>
#include <knet/socket/Socket.h>

namespace gk {

NetClient::NetClient()
: m_ios()
, m_tcp()
, m_udp()
, m_listener( 0 )
, m_recvQ()
, m_sendQ()
, m_groupId( 0 )
, m_selfTag( 0 )
, m_processedCount( 0 )
{
}

NetClient::~NetClient()
{
}

bool 
NetClient::Init( MessageListener* listener )
{
	K_ASSERT( listener != 0 );

	Socket::Startup();

	m_listener = listener;

	bool rc = m_ios.Init();

	if ( !rc )
	{
		LOG( FT_ERROR, _T("NetClient::Init> IoService init failed") );

		return false;
	}

	rc = m_tcp.Init( this, &m_ios );

	if ( !rc )
	{
		LOG( FT_ERROR, _T("NetClient::Init> TcpCommunicator init failed") );

		return false;
	}

	m_groupId = 0;
	m_selfTag = 0;

	MessageFactory::Instance()->Register( new NmGroupPrepare );
	MessageFactory::Instance()->Register( new NmGroupJoin );
	MessageFactory::Instance()->Register( new NmGroupLeave );
	MessageFactory::Instance()->Register( new NmGroupDestroy );

	Start();

	return true;
}

void 
NetClient::Connect( const IpAddress& addr )
{
	NetControlMessage* ncm = new NetControlMessage;

	ncm->control = NetControlMessage::TCP_CONNECT;
	ncm->remote  = addr;

	this->Notify( MessagePtr( ncm ) );
}

void 
NetClient::Close( uint connectionId )
{
	NetControlMessage* ncm = new NetControlMessage;

	ncm->control = NetControlMessage::TCP_CLOSE;
	ncm->connectionId = connectionId;

	this->Notify( MessagePtr( ncm ) );
}

void 
NetClient::Send( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	m_sendQ.Put( m );
}

void 
NetClient::SendUdp( MessagePtr m, int qos )
{
	UdpSend s;
	s.m 	= m;
	s.qos 	= qos;

	m_udpSendQ.Put( s );
}

void 
NetClient::BroadcastUdp( MessagePtr m, int qos )
{
	UdpSend s;

	s.m 		= m;
	s.qos 		= qos;
	s.broadcast = true;

	m_udpSendQ.Put( s );
}

void 
NetClient::Notify( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	m_recvQ.Put( m );
}

int 
NetClient::Run()
{
	LOG( FT_INFO, _T("NetClient::Run> Started") );

	while ( IsRunning() )
	{
		m_processedCount = 0;

		m_tcp.Run(); 
		m_udp.Run();
		
		processRecvQ();
		processSendQ();
		processUdpSendQ();

		if ( m_processedCount == 0 )
		{
			::Sleep( 1 );
		}
	}

	LOG( FT_INFO, _T("NetClient::Run> Stopped") );

	return 0;

}

void 
NetClient::Fini()
{
	Stop();

	MessagePtr mp;
	
	while ( m_recvQ.Get( mp ) )	{}
	while ( m_sendQ.Get( mp ) )	{}

	m_tcp.Fini();
	m_ios.Fini();
	m_udp.Fini();

	Socket::Cleanup();
}

void 
NetClient::processRecvQ()
{
	MessagePtr m;

	while ( m_recvQ.Get( m ) )
	{
		switch ( m->type )
		{
		case NET_CONTROL_MESSAGE:
			onControl( m );
			break;
		case NET_GROUP_PREPARE:
			onGroupPrepare( m );
			break;
		case NET_GROUP_JOIN:
			onGroupJoin( m );
			break;
		case NET_GROUP_LEAVE:
			onGroupLeave( m );
			break;
		case NET_GROUP_DESTROY:
			onGroupDestroy( m );
			break;
		default:
			m_listener->Notify( m );
			break;
		}

		++m_processedCount;
	}
}

void 
NetClient::processSendQ()
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
NetClient::processUdpSendQ()
{
	UdpSend s;

	while ( m_udpSendQ.Get( s ) )
	{
		if ( s.broadcast )
		{
			m_udp.Broadcast( s.m, s.qos );
		}
		else
		{
			m_udp.SendByTag( s.m, s.qos );
		}	

		++m_processedCount;
	}
}

void 
NetClient::onControl( MessagePtr m )
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
			m_udp.CheckRelay( cm->connectionId );
		}
		break;
	}
}

void 
NetClient::onGroupPrepare( MessagePtr m )
{
	K_ASSERT( m_groupId == 0 );
	K_ASSERT( m_selfTag == 0 );

	m_groupId = 0;
	m_selfTag = 0;

	NmGroupPrepare* gp = static_cast<NmGroupPrepare*>( m.Get() );

	TcpConnection* c = m_tcp.FindById( gp->remote );

	if ( c == 0 )
	{
		LOG( FT_WARN, 
			 _T("NetClient::onGroupPrepare> Connection %d not found"), 
			 gp->remote );

		return;
	}
	
	m_udp.Fini();

	m_groupId = gp->groupId;
	m_selfTag = gp->connectionId;

	// init udp with c as a relay connection 
	// use same ip:port as TCP connection
	bool rc = m_udp.Init( this, 
						  &m_ios, 
						  c->GetSocket()->GetAddress(), 
						  m_selfTag, 
						  gp->sl, 
						  gp->challenge, 
						  c );

	if ( !rc )
	{
		LOG( FT_WARN, _T("NetClient::onGroupPrepare> Failed to init udp") );

		return;
	}

	NmGroupPrepared* p = new NmGroupPrepared;

	p->remote 		= gp->remote;
	p->groupId 		= gp->groupId;
	p->connectionId = gp->connectionId;
	p->in 			= c->GetSocket()->GetAddress();

	m_tcp.Send( m->remote, MessagePtr( p ) );

	LOG( FT_DEBUG, _T("NetClient::onGroupPrepare> Prepared group %d connection %d"), 
		 gp->groupId, 
		 gp->connectionId );
}

void 
NetClient::onGroupJoin( MessagePtr m )
{
	K_ASSERT( m_groupId != 0 );

 	NmGroupJoin* join = static_cast<NmGroupJoin*>( m.Get() );

	K_ASSERT( join->groupId == m_groupId );

	LOG( FT_DEBUG, 
		 _T("NetClient::onGroupJoin> Group %d Members %d"), 
		 join->groupId, 
		 join->members.size() );

	NmGroupJoin::MemberList::iterator i( join->members.begin() );
	NmGroupJoin::MemberList::iterator iEnd( join->members.end() );

	for ( ; i != iEnd; ++i )
	{
		NetGroupMember& member = *i;

		if ( member.connectionId != m_selfTag )
		{
			UdpConnection* c = m_udp.FindByTag( member.connectionId );

			if ( c != 0 )
			{
				continue;
			}

			// tag is used for communication
			(void)m_udp.Connect( member.connectionId, member.in, member.ex ); 

			NmGroupJoined* joined = new NmGroupJoined;

			joined->groupId 	 = join->groupId;
			joined->connectionId = member.connectionId;
			joined->extra		 = member.extra;

			m_listener->Notify( MessagePtr( joined ) );

			LOG( FT_DEBUG, 
				_T("NetClient::onGroupJoin> Self %d Tag %d joined In %s Ex %s"), 
				m_selfTag,
				member.connectionId, 
				member.in.ToString().c_str(), 
				member.ex.ToString().c_str() );
		}
	}
}

void 
NetClient::onGroupLeave( MessagePtr m )
{
	K_ASSERT( m_groupId != 0 );

	NmGroupLeave* leave = static_cast<NmGroupLeave*>( m.Get() );

	m_udp.CloseByTag( leave->connectionId );			

	if ( leave->connectionId == m_selfTag )
	{
		m_groupId = 0;
		m_selfTag = 0;

		m_udp.Fini(); // finish by left the group by myself.
	}

	LOG( FT_DEBUG, 
		 _T("NetClient::onGroupLeave> Tag %d left"), 
		 leave->connectionId );

	m_listener->Notify( m );
}

void 
NetClient::onGroupDestroy( MessagePtr m )
{
	LOG( FT_DEBUG, 
		 _T("NetClient::onGroupDestroy> Group %d"), 
		 m_groupId );

	m_groupId = 0;
	m_selfTag = 0;

	m_udp.Fini();

	m_listener->Notify( m );
}
	
} // gk
