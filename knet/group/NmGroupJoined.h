#pragma once 

#include <knet/message/Message.h>
#include <knet/message/MessageSerialization.h>
#include <knet/message/net/NetMessageTypes.h>

namespace gk { 

/**
 * @struct NmGroupJoined
 *
 * Client internal message when NmGroupJoin is received. 
 *
 * [1] Start connecting to other members 
 * [2] Then report this immediately since TCP relay is available.
 */
struct NmGroupJoined : public Message 
{
	uint groupId; 		// udp group id 
	uint connectionId; 	// UDP connection to send
	tstring extra;

	bool Pack( BitStream& /* bs */ )
	{
		K_ASSERT( !_T("Internal message") );

		return false;
	}

	bool Unpack( BitStream& /* bs */ )
	{
		K_ASSERT( !_T("Internal message") );

		return false;
	}
	
	NmGroupJoined()
	: groupId( 0 ) 
	, connectionId( 0 ) 
	{
		type = NET_GROUP_JOINED;
	}
};

} // gk
