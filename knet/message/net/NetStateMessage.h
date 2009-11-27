#pragma once 

#include <knet/message/Message.h>
#include <knet/message/net/NetMessageTypes.h>
#include <knet/socket/Socket.h>
#include <knet/NetSecurity.h>

namespace gk {
/**
 * @struct NetStateMessage 
 *
 * Internal message to report udp and tcp states
 */
struct NetStateMessage : public Message
{
	enum State 
	{
		  NONE
		, UDP_OPENED 
		, UDP_CLOSED
		, UDP_OPEN_TIMEOUT

		, TCP_CONNECTED
		, TCP_ACCEPTED
		, TCP_CONNECT_FAIL

		, TCP_OPEN 				// A connetion is ready for communication
		, TCP_CLOSED
	};

	uint 		state; 			// state mesage
	uint 		connectionId; 	// Most significant byte is for communicator
	uint		groupId;		// groupId if the connection is in a group
	IpAddress 	addr; 			// Remote or peer ip address 
	Socket*   	socket;			// When tcp accepted or connected
	SecurityLevel sl;

	NetStateMessage()
		: state( 0 )
		, connectionId( 0 )
		, groupId( 0 )
		, addr()
		, socket( 0 )
	{
		type = NET_STATE_MESSAGE;
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
