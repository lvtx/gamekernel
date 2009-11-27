#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/group/NetGroup.h>

#include <kcore/sys/Logger.h>
#include <knet/group/NmGroupPrepare.h>
#include <knet/group/NmGroupPrepared.h>
#include <knet/group/NmGroupJoin.h>
#include <knet/group/NmGroupJoined.h>
#include <knet/group/NmGroupLeave.h>
#include <knet/group/NmGroupDestroy.h>
#include <knet/tcp/TcpCommunicator.h>

namespace gk {

NetGroup::NetGroup()
: m_communicator( 0 )
, m_id( 0 )
, m_securityLevel()
, m_members()
, m_remotes()
{
}

NetGroup::~NetGroup()
{
}

bool 
NetGroup::Init( TcpCommunicator* communicator, uint id, SecurityLevel sl )
{
	K_ASSERT( communicator != 0 );
	K_ASSERT( id > 0 );

	m_communicator  = communicator;
	m_id 			= id;
	m_securityLevel = sl;

	K_ASSERT( m_members.empty() );
	K_ASSERT( m_remotes.empty() );

	if ( m_securityLevel > SECURITY0 )
	{
		m_cipher.Init();
	}

	return true;
}

bool 
NetGroup::Join( uint connectionId, const IpAddress& ex, const tstring& extra )
{
	MemberMap::iterator i( m_members.find( connectionId ) );

	if ( i != m_members.end() )
	{
		LOG( FT_WARN, 
			 _T("NetGroup::Join> Duplicate join group %d conn %d"), 
			 m_id, 
			 connectionId );

		return true;
	}

	NetGroupMember m;

	m.connectionId  = connectionId;
	m.ex 			= ex;
	m.state 		= NetGroupMember::INIT;
	m.extra			= extra;

	m_members.insert( MemberMap::value_type( connectionId, m ) );
	m_remotes.push_back( connectionId );

	LOG( FT_DEBUG, 
		_T("NetGroup::Join> Group %d Connection %d Ex %s"), 
		m_id, 
		connectionId, 
		m.ex.ToString().c_str() );

	sendPrepare( m );

	return true;
}

void 
NetGroup::Leave( uint connectionId )
{
	MemberMap::iterator i( m_members.find( connectionId ) );

	if ( i == m_members.end() )
	{
		LOG( FT_WARN, 
			 _T("NetGroup::Leave> Member %d in group %d not found"), 
			 connectionId, 
			 m_id );

		return;
	}

	LOG( FT_DEBUG, 
		 _T("NetGroup::Leave> Group %d Connection %d"), 
		 m_id, 
		 connectionId );

	sendLeave( i->second ); // send leave first to all members

	m_members.erase( connectionId );

	// then erase 
	
	std::vector<uint>::iterator r( m_remotes.begin() );
	std::vector<uint>::iterator rEnd( m_remotes.end() );

	for ( ; r != rEnd; ++r )
	{
		if ( *r == connectionId )
		{
			m_remotes.erase( r );

			return;
		}
	}
}

void 
NetGroup::OnPrepared( uint connectionId, const IpAddress& in )
{
	MemberMap::iterator i( m_members.find( connectionId ) );

	if ( i == m_members.end() )
	{
		LOG( FT_WARN, 
			 _T("NetGroup::OnPrepared> Member %d in group %d not found"), 
			 connectionId, 
			 m_id );

		return;
	}

	NetGroupMember& m = i->second;

	m.in 	= in;
	m.state = NetGroupMember::PREPARED;

	LOG( FT_WARN, 
		 _T("NetGroup::OnPrepared> Member %d in group %d prepared"), 
		 connectionId, 
		 m_id );


	sendJoin( m );
}

void 
NetGroup::Fini()
{
	sendDestroy();

	m_members.clear();
	m_remotes.clear();
}

void 
NetGroup::sendPrepare( const NetGroupMember& m )
{
	// when server join and to figure out internal address 
	
	NmGroupPrepare* p = new NmGroupPrepare;

	p->remote  	= m.connectionId;
	p->groupId 	= m_id;
	p->connectionId = m.connectionId;	
	p->ex     	= m.ex;
	p->sl 		= m_securityLevel;
	::memcpy( p->challenge, m_cipher.GetChallenge(), Cipher::LEN_CHALLENGE );

	m_communicator->Send( p->connectionId, MessagePtr( p ) );
}

void 
NetGroup::sendJoin( const NetGroupMember& m )
{
	// when prepared and to connect to other members
	
	NmGroupJoin* p = new NmGroupJoin;

	p->remotes = m_remotes;
	p->remote  = 0;
	p->groupId = m_id;
	p->connectionId = m.connectionId;	

	MemberMap::iterator i( m_members.begin() );
	MemberMap::iterator iEnd( m_members.end() );

	for ( ; i != iEnd; ++i )
	{
		p->members.push_back( i->second );
	}

	K_ASSERT( p->remotes.size() == p->members.size() );

	m_communicator->Send( p->remotes, MessagePtr( p ) );
}

void 
NetGroup::sendLeave( const NetGroupMember& m )
{
	// when left on server and to disconnect 
	
	NmGroupLeave* l = new NmGroupLeave;

	l->remotes = m_remotes;
	l->groupId = m_id;
	l->connectionId = m.connectionId;	

	m_communicator->Send( l->remotes, MessagePtr( l ) );
}

void 
NetGroup::sendDestroy()
{
	// when group is destroyed and to clear up on client
	
	NmGroupLeave* m = new NmGroupLeave;

	m->remotes 	= m_remotes;
	m->groupId 	= m_id;

	m_communicator->Send( m_remotes, MessagePtr( m ) );
}

} // gk 
