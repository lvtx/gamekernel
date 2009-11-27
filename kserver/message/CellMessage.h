#pragma once 

#include <knet/message/Message.h>
#include <kserver/cell/CellId.h>
#include <kserver/message/CellMessageTypes.h>

namespace gk {

/**
 * @struct CellMessage 
 *
 * @brief Base class for all cell messages 
 */
struct CellMessage : public Message 
{
	CellId src;
	CellId dst;

	bool Pack( BitStream& bs )
	{
		K_ASSERT( type > CELL_MESSAGE_BEGIN );

		Message::Pack( bs );

		src.Pack( bs );
		dst.Pack( bs );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		K_ASSERT( type > CELL_MESSAGE_BEGIN );

		Message::Unpack( bs );

		src.Unpack( bs );
		dst.Unpack( bs );

		return bs.IsValid();
	}

}; 

} // 
