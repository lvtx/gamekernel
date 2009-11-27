#pragma once 

#include <knet/cipher/Cipher.h>
#include <knet/message/Message.h>
#include <knet/message/net/NetMessageTypes.h>
#include <knet/NetSecurity.h>

namespace gk {
/**
 * @struct NetHandshake 
 *
 * Handshake on TCP security setup
 */
struct NetHandshake : public Message
{
	SecurityLevel sl;
	byte 		  challenge[Cipher::LEN_CHALLENGE];

	NetHandshake()
		: sl( SECURITY0 )
	{
		type = NET_HANDSHAKE;

		::memset( challenge, 0, Cipher::LEN_CHALLENGE );
	}

	bool Pack( BitStream& bs )
	{
		Message::Pack( bs );

		bs.WriteInt( sl, 16 );
		bs.Write( Cipher::LEN_CHALLENGE, challenge );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		Message::Unpack( bs );

		uint lsl = SECURITY0;

		bs.ReadInt( lsl, 16 );
		sl = (SecurityLevel)lsl;

		bs.Read( Cipher::LEN_CHALLENGE, challenge );

		return bs.IsValid();
	}

	// create is not required since it is hard coded in network code
};

} // gk
