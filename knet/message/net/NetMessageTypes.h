#pragma once 

#include <knet/message/Message.h>

namespace gk {

enum 
{
	  NET_STATE_MESSAGE = Message::MESSAGE_TYPE_SYSTEM_BEGIN
	, NET_CONTROL_MESSAGE
	, NET_GROUP_PREPARE
	, NET_GROUP_PREPARED
	, NET_GROUP_JOIN
	, NET_GROUP_JOINED
	, NET_GROUP_LEAVE
	, NET_GROUP_DESTROY
	, NET_GROUP_RELAY
	, NET_HANDSHAKE
	, NET_MESSAGE_END
};

} // gk
