#pragma once 

#include <knet/message/Message.h>

namespace gk
{

/**
 * @class MessageListener 
 *
 * Interface for message listener.
 * Notify is called to notify message from other classes.
 */
class MessageListener
{
public:
	virtual ~MessageListener();
	virtual void Notify( MessagePtr m ) = 0;
};

inline
MessageListener::~MessageListener()
{
}

} // gk
