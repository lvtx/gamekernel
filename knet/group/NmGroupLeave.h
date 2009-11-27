#pragma once 

#include <knet/message/Message.h>
#include <knet/message/MessageSerialization.h>
#include <knet/message/net/NetMessageTypes.h>

namespace gk { 

/**
 * @struct NmGroupLeave
 *
 * Sent to client to disconnect udp connection 
 */
struct NmGroupLeave : public Message 
{
	uint groupId;
	uint connectionId; // UDP connection to send

	bool Pack( BitStream& bs )
	{
		Message::Pack( bs );

		bs.Write( groupId );
		bs.Write( connectionId );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		Message::Unpack( bs );

		bs.Read( groupId );
		bs.Read( connectionId );

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new NmGroupLeave;
	}

	NmGroupLeave()
	: groupId( 0 ) 
	, connectionId( 0 ) 
	{
		type = NET_GROUP_LEAVE;
	}
};

} // gk
