#pragma once 

#include <knet/group/NetGroupMember.h>
#include <knet/message/Message.h>
#include <knet/message/MessageSerialization.h>
#include <knet/message/net/NetMessageTypes.h>

namespace gk {

/**
 * @struct NmGroupJoin 
 *
 * Sent to group members when a server receives NmGroupPrepared
 */
struct NmGroupJoin : public Message 
{
	typedef std::vector<NetGroupMember> MemberList;

	uint 			groupId;
	uint 			connectionId; 		// who joined
	MemberList  	members;   	// the whole members including me

	bool Pack( BitStream& bs )
	{
		Message::Pack( bs );

		bs.Write( groupId );
		bs.Write( connectionId );

		uint count = members.size();

		bs.WriteInt( count, 11 );

		MemberList::iterator i( members.begin() );
		MemberList::iterator iEnd( members.end() );

		for ( ; i != iEnd; ++i )
		{
			NetGroupMember& m = *i;

			bs.Write( m.connectionId );

			gk::Pack( bs, m.in ); 
			gk::Pack( bs, m.ex ); 

			bs.Write( m.extra );
		}	

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		Message::Unpack( bs );

		bs.Read( groupId );
		bs.Read( connectionId );

		uint count = members.size();

		bs.ReadInt( count, 11 );

		for ( uint i=0; i<count; ++i )
		{
			NetGroupMember m;

			bs.Read( m.connectionId );
			gk::Unpack( bs, m.in ); 
			gk::Unpack( bs, m.ex ); 

			bs.Read( m.extra );

			members.push_back( m );
		}

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new NmGroupJoin;
	}

	NmGroupJoin()
	: groupId( 0 )
	, connectionId( 0 )
	, members()
	{
		type = NET_GROUP_JOIN;
	}
};

} // gk

