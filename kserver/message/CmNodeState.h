#pragma once 

#include <knet/message/MessageSerialization.h>
#include <kserver/message/CellMessage.h>

namespace gk {

/**
 * @struct CmNodeState 
 *
 * @brief Node state message
 */
struct CmNodeState : public CellMessage 
{
	ushort 		nodeId;
	tstring 	name;
	IpAddress 	addr;
	
	CmNodeState()
	: nodeId( 0 )
	, name()
	{
		type = CM_NODE_STATE;
	}

	bool Pack( BitStream& bs )
	{
		CellMessage::Pack( bs );

		bs.WriteInt( nodeId, 12 );
		bs.Write( name );

		gk::Pack( bs, addr );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		CellMessage::Unpack( bs );

		uint s = 0;
		bs.ReadInt( s, 12 );
		nodeId = (ushort)s;

		bs.Read( name );

		gk::Unpack( bs, addr );

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new CmNodeState;
	}
};

} // gk
