#pragma once 

#include <knet/message/Message.h>
#include <knet/message/MessageSerialization.h>
#include <knet/message/net/NetMessageTypes.h>

namespace gk {

/**
 * @struct NmGroupPrepared 
 *
 * Sent when a client received NmGroupPrepare after figuring out internal address
 */
struct NmGroupPrepared : public Message 
{
	uint 		groupId;
	uint 		connectionId;
	IpAddress 	in;

	bool Pack( BitStream& bs )
	{
		Message::Pack( bs );

		bs.Write( groupId );
		bs.Write( connectionId );

		gk::Pack( bs, in );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		Message::Unpack( bs );

		bs.Read( groupId );
		bs.Read( connectionId );

		gk::Unpack( bs, in );

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new NmGroupPrepared;
	}

	NmGroupPrepared()
	: groupId( 0 )
	, connectionId( 0 )
	, in()
	{
		type = NET_GROUP_PREPARED;
	}
};

} // gk


