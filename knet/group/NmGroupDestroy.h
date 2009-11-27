#pragma once 

#include <knet/message/Message.h>
#include <knet/message/MessageSerialization.h>
#include <knet/message/net/NetMessageTypes.h>

namespace gk {

/**
 * @struct NmGroupDestroy 
 *
 * Sent when a server destroys a group. 
 * 
 * This is used on client too to report group destroy
 */
struct NmGroupDestroy : public Message 
{
	uint 		groupId;

	bool Pack( BitStream& bs )
	{
		Message::Pack( bs );

		bs.Write( groupId );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		Message::Unpack( bs );

		bs.Read( groupId );

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new NmGroupDestroy;
	}

	NmGroupDestroy()
	: groupId( 0 )
	{
		type = NET_GROUP_DESTROY;
	}
};

} // gk

