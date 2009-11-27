#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/async/AsyncGroup.h>
#include <kserver/async/AsyncWorkerFactory.h>
#include <kcore/util/Random.h>

namespace gk 
{

AsyncGroup::AsyncGroup()
: m_workers()
, m_name()
{
}

AsyncGroup::~AsyncGroup()
{
}

bool 
AsyncGroup::Init( Node* node, const tstring& name, const tstring& cls, uint count, bool luaMode )
{
	K_ASSERT( count > 0 );
	K_ASSERT( name.size() > 0 );

	m_name		= name;
	m_luaMode	= luaMode;

	for ( uint i=0; i<count; ++i )
	{
		AsyncWorker* worker = AsyncWorkerFactory::Instance()->Create( cls );

		if ( worker->Init( node ) )
		{
			m_workers.push_back( worker );
		}
		else
		{
			delete worker;
		}
	}

	return m_workers.size() > 0;
}

void 
AsyncGroup::Notify( MessagePtr m )
{
	uint size   = m_workers.size();
	uint target = Random::Rand() % size;	

	m_workers[target]->Notify( m );
}

void 
AsyncGroup::Fini()
{
	WorkerList::iterator i( m_workers.begin() );
	WorkerList::iterator iEnd( m_workers.end() );

	for ( ; i != iEnd; ++i )
	{
		AsyncWorker* worker = *i;

		worker->Fini();

		if ( !m_luaMode )
		{
			delete worker;
		}
	}

	m_workers.clear();

	LOG( FT_INFO, _T("AsyncGroup::Fini> Finished") );
}

} // namespace gk
