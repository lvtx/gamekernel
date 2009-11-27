#pragma once

#include <kcore/sys/ScopedLock.h>
#include <knet/message/Message.h>

#include <hash_map>

namespace gk 
{

/**
 * @class MessageFactory
 *
 * A factory for messages
 */
class MessageFactory
{
public:
	/**
	 * Access a singleton
	 */
	static MessageFactory* Instance();

	/**
	 * Destructor
	 */
	~MessageFactory();

	/**
	 * Register a message protype 
	 *
	 * @param protype The prototype message for a type.
	 */
	void Register( Message* prototype );

	/**
	 * Create a message of type
	 *
	 * @param type The message type to create 
	 * @return The message created
	 */
	MessagePtr Create( int type );

private:
	typedef stdext::hash_map<int, Message*> MessageMap;

	MessageFactory();

	void cleanup();

private:
	MessageMap 	m_messages;
	Mutex		m_lock;
};

} // knet 

#define REGISTER_MESSAGE( m ) \
	gk::MessageFactory::Instance()->Register( m )

