#pragma once 

#include <kcore/mem/AllocatorAware.h>
#include <kcore/sys/SharedPointer.h>
#include <kcore/sys/Queue.h>
#include <knet/message/BitStream.h>
#include <knet/message/ContextKey.h>

#include <vector>

namespace gk 
{
/**
 * @class Message 
 * 
 * Message is the base of communication in gamekernel.
 *
 * Following steps are required to use Message for communication. 
 *
 * [1] Define a message class inherited from Message
 * [2] Write Pack/Unpack
 * [3] Register the class to MessageFactory if it is received
 */
struct Message : public AllocatorAware
{
	enum 
	{
		  MESSAGE_TYPE_SYSTEM_BEGIN = 1 	// Begin of internal message type
		, MESSAGE_TYPE_SYSTEM_END   = 1024  // End of internal message type
	};

	enum UdpQosParameter
	{
		  RELIABLE = 1 			// udp reliable send option
		, ORDERED  = 2 			// udp ordered send option
		, LOSSY    = 3 			// udp lossy send option
	};

	typedef std::vector<uint> RemoteList;

	ushort 		type; 			// Message type
	uint 		remote; 		// Has different meaning, usually connection id	
	RemoteList 	remotes; 		// Only used when sending message to several connections
	tstring 	contextKey; 	// context key for context based dispatching.   

	Message();
	virtual ~Message();

	/**
	 * Serialize this message to a bitstream
	 *
	 * @return true when successful
	 */
	virtual bool Pack( BitStream& bs );

	/**
	 * Loads a message from a bitstream 
	 *
	 * @return true when successful
	 */
	virtual bool Unpack( BitStream& bs );

	/**
	 * Prototype for message 
	 *
	 * @return heap allocated and default constructed message
	 */
	virtual Message* Create();
};

typedef SharedPointer<Message> MessagePtr;
typedef Queue<MessagePtr, Mutex> MessageQ;

} // gk 
