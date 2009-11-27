#include "StdAfx.h"

#include <kcore/corebase.h>
#include <knet/message/MessageFactory.h>

namespace gk {

MessageFactory*
MessageFactory::Instance()
{
	static MessageFactory instance;
	return &instance;
}

MessageFactory::MessageFactory()
{
}

MessageFactory::~MessageFactory()
{
	cleanup();
}

void
MessageFactory::Register( Message* prototype )
{
	ScopedLock sl( m_lock );

	MessageMap::iterator itr( m_messages.find( prototype->type ) );

	if( itr != m_messages.end() )
	{
		delete itr->second;

		m_messages.erase( itr );
	}

	m_messages.insert( MessageMap::value_type( prototype->type, prototype ) );
}

MessagePtr
MessageFactory::Create( int type )
{
	ScopedLock sl( m_lock );

	MessageMap::iterator itr( m_messages.find( type ) );

	if( itr == m_messages.end() )
	{
		return MessagePtr();
	}

	Message* m = itr->second;
	K_ASSERT( m != NULL );

	return MessagePtr( m->Create() );
}

void
MessageFactory::cleanup()
{
	MessageMap::iterator itr( m_messages.begin() );
	
	for( ; itr != m_messages.end(); ++itr)
	{
		delete itr->second;
	}

	m_messages.clear();
}

} // gk
