#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/async/AsyncPool.h>

namespace gk 
{

AsyncPool::AsyncPool()
: m_node( 0 )
, m_groups()
, m_lock()
{
}

AsyncPool::~AsyncPool()
{
	Fini();
}

bool 
AsyncPool::Init( Node* node, bool luaMode )
{
	K_ASSERT( node != 0 );

	m_node		= node;
	m_luaMode	= luaMode;
	
	return true;
}

bool 
AsyncPool::CreateGroup( const tstring& cls, const tstring& name, uint count )
{
	ScopedLock sl( m_lock );

	if ( m_groups.find( name ) != m_groups.end() )
	{
		return true; // already exist
	}

	AsyncGroup* group = new AsyncGroup();

	bool rc = group->Init( m_node, name, cls, count, m_luaMode );

	if ( !rc )
	{
		delete group;

		LOG( FT_ERROR, _T("AsyncPool::CreateGroup> Failed to create %s"), name.c_str() );

		return false;
	}

	m_groups.insert( GroupMap::value_type( name, group ) );

	LOG( FT_INFO, _T("AsyncPool::CreateGroup> %s created"), name.c_str() ); 

	return true;
}

void 
AsyncPool::Notify( const tstring& name, MessagePtr m )
{
	ScopedLock sl( m_lock );

	GroupMap::iterator i( m_groups.find( name ) );

	if ( i != m_groups.end() )
	{
		AsyncGroup* group = i->second;

		group->Notify( m );
	}	
}

void 
AsyncPool::DestroyGroup( const tstring& name )
{
	ScopedLock sl( m_lock );

	GroupMap::iterator i( m_groups.find( name ) );

	if ( i != m_groups.end() )
	{
		AsyncGroup* group = i->second;

		group->Fini();

		delete group;

		m_groups.erase( i );

		LOG( FT_INFO, _T("AsyncPool::DestroyGroup> %s destroyed"), name.c_str() ); 
	}
}

void 
AsyncPool::Fini()
{
	ScopedLock sl( m_lock );

	GroupMap::iterator i( m_groups.begin() );
	GroupMap::iterator iEnd( m_groups.end() );

	for ( ; i != iEnd; ++i )
	{
		AsyncGroup* group = i->second;

		group->Fini();

		delete group;
	}

	m_groups.clear();

	m_node = 0;

	LOG( FT_INFO, _T("AsyncPool::Fini> Finished") );
}

} // gk

