#pragma once 

#include <knet/cipher/Cipher.h>
#include <knet/message/Message.h>
#include <knet/message/MessageSerialization.h>
#include <knet/message/net/NetMessageTypes.h>
#include <knet/NetSecurity.h>

namespace gk {

/**
 * @struct NmGroupPrepare 
 *
 * Sent when a join requested on server to the client.
 */
struct NmGroupPrepare : public Message 
{
	uint 			groupId;
	uint 			connectionId;
	IpAddress 		ex;
	SecurityLevel 	sl;
	byte 			challenge[Cipher::LEN_CHALLENGE];

	bool Pack( BitStream& bs )
	{
		Message::Pack( bs );

		bs.WriteInt( groupId, 32 );
		bs.WriteInt( connectionId, 32 );

		gk::Pack( bs, ex );

		bs.WriteInt( sl, 16 );
		bs.Write( Cipher::LEN_CHALLENGE, challenge );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		Message::Unpack( bs );

		bs.ReadInt( groupId, 32 );
		bs.ReadInt( connectionId, 32 );

		gk::Unpack( bs, ex );

		uint lsl = SECURITY0;

		bs.ReadInt( lsl, 16 );
		sl = (SecurityLevel)lsl;

		bs.Read( Cipher::LEN_CHALLENGE, challenge );

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new NmGroupPrepare;
	}

	NmGroupPrepare()
	: groupId( 0 )
	, connectionId( 0 )
	, ex()
	, sl( SECURITY0 )
	{
		type = NET_GROUP_PREPARE;

		::memset( challenge, 0, Cipher::LEN_CHALLENGE );
	}
};

} // gk
