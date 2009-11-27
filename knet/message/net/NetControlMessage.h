#pragma once 

#include <knet/message/Message.h>
#include <knet/message/net/NetMessageTypes.h>
#include <knet/socket/Socket.h>
#include <knet/NetSecurity.h>

namespace gk {
/**
 * @struct NetControlMessage 
 *
 * Internal message to control network 
 */
struct NetControlMessage : public Message
{
	enum Control 
	{
		  NONE
		, UDP_LISTEN
		, UDP_OPEN
		, UDP_CLOSE
		, UDP_MAKE_LOSSY

		, TCP_LISTEN
		, TCP_CONNECT
		, TCP_CLOSE
	};

	uint 		control; 		// control
	uint 		connectionId; 	// connnection id when greater than 0  
	IpAddress 	remote; 		// Remote address
	uint 		param; 			// parameter if any
	SecurityLevel sl;

	NetControlMessage()
		: control( 0 )
		, connectionId( 0 )
		, remote()
		, param( 0 )
		, sl( SECURITY0 )
	{
		type = NET_CONTROL_MESSAGE;
	}

	bool Pack( BitStream& /* bs */ )
	{
		K_ASSERT( !_T("Internal message") );

		return false;
	}

	bool Unpack( BitStream& /* bs */ )
	{
		K_ASSERT( !_T("Internal message") );

		return false;
	}
};

} // gk
