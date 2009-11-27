#pragma once 

#include <knet/message/Message.h>
#include <knet/message/net/NetMessageTypes.h>

namespace gk {

/**
 * @struct NmGroupRelay 
 *
 * Relay message to switch in network layer.
 */
struct NmGroupRelay : public Message 
{
	std::vector<uint> relays;
	byte  			  data[512];
	uint  			  len;

	bool Pack( BitStream& bs )
	{
		Message::Pack( bs );

		K_ASSERT( len > 0 );
		K_ASSERT( !relays.empty() );

		bs.WriteInt( relays.size(), 8 ); // 128 connections max

		std::vector<uint>::iterator i( relays.begin() );
		std::vector<uint>::iterator iEnd( relays.end() );

		for ( ; i != iEnd; ++i )
		{
			bs.Write( *i );
		}

		bs.WriteInt( len, 9 ); 			 // 512 max
		bs.Write( len, data );	

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		Message::Unpack( bs );

		uint count = 0;

		bs.ReadInt( count, 8 );

		for ( uint i=0; i<count; ++i )
		{
			uint relay = 0;

			bs.Read( relay );

			relays.push_back( relay );
		}

		bs.ReadInt( len, 9 ); 			// 512 max
		bs.Read( len, data );	

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new NmGroupRelay;
	}

	NmGroupRelay()
	: len( 0 )
	{
		type = NET_GROUP_RELAY;

		::memset( data, 0, 512 );
		relays.clear();
	}
};

} // gk
