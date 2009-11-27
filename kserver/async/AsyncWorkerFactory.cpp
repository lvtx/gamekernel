#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/async/AsyncWorkerFactory.h>
#include <kcore/util/StringUtil.h>

namespace gk 
{

AsyncWorkerFactory*
AsyncWorkerFactory::Instance()
{
	static AsyncWorkerFactory instance;

	return &instance;
}

AsyncWorkerFactory::AsyncWorkerFactory()
{
}

AsyncWorkerFactory::~AsyncWorkerFactory()
{
	cleanup();
}

void
AsyncWorkerFactory::Register( const tstring& cls, AsyncWorker* prototype )
{
	ScopedLock sl( m_lock );

	AsyncWorkerMap::iterator itr( m_workers.find( cls ) );

	if( itr != m_workers.end() )
	{
		delete itr->second;

		m_workers.erase( itr );
	}

	m_workers.insert( AsyncWorkerMap::value_type( cls, prototype ) );
}

AsyncWorker* 
AsyncWorkerFactory::Create( const tstring& cls )
{
	ScopedLock sl( m_lock );

	AsyncWorkerMap::iterator itr( m_workers.find( cls ) );

	if( itr == m_workers.end() )
	{
		return (AsyncWorker*)0;
	}

	AsyncWorker* m = itr->second;

	K_ASSERT( m != 0 );

	return m->Create();
}

AsyncWorker* 
AsyncWorkerFactory::Create( const char* cls )
{
	tstring wcls;

	StringUtil::ConvertToSystemCode( cls, wcls );

	return Create( wcls );
}

void
AsyncWorkerFactory::cleanup()
{
	AsyncWorkerMap::iterator itr( m_workers.begin() );

	for( ; itr != m_workers.end(); ++itr)
	{
		delete itr->second;
	}

	m_workers.clear();
}

} // namespace gk
