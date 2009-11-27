#pragma once 

#include <knet/message/net/NetMessageTypes.h>

namespace gk {

/** 
 * From Cell and above, all messages are cell messages.
 * This can be checked by message->type > CELL_MESSAGE_BEGIN 
 */
enum 
{
	  CELL_MESSAGE_BEGIN = NET_MESSAGE_END
	, CM_NODE_STATE 							///< Node state message
	, CM_CELL_STATE  							///< Cell state message
	, CM_LUA_LAX_MESSAGE
	, CELL_MESSAGE_END 
};

} // gk
