#include "StdAfx.h"

#include <kcore/corebase.h>
#include <kserver/cell/CellFactory.h>

#include <kcore/util/StringUtil.h>

namespace gk {

CellFactory*
CellFactory::Instance()
{
	static CellFactory instance;

	return &instance;
}

CellFactory::CellFactory()
{
}

CellFactory::~CellFactory()
{
	cleanup();
}

void
CellFactory::Register( const tstring& cls, Cell* prototype )
{
	ScopedLock sl( m_lock );

	CellMap::iterator itr( m_cells.find( cls ) );

	if( itr != m_cells.end() )
	{
		delete itr->second;

		m_cells.erase( itr );
	}

	m_cells.insert( CellMap::value_type( cls, prototype ) );
}

Cell* 
CellFactory::Create( const tstring& cls )
{
	ScopedLock sl( m_lock );

	CellMap::iterator itr( m_cells.find( cls ) );

	if( itr == m_cells.end() )
	{
		return (Cell*)0;
	}

	Cell* m = itr->second;

	K_ASSERT( m != 0 );

	return m->Create();
}

Cell* 
CellFactory::Create( const char* cls )
{
	tstring wcls;

	StringUtil::ConvertToSystemCode( cls, wcls );

	return Create( wcls );
}

void
CellFactory::cleanup()
{
	CellMap::iterator itr( m_cells.begin() );

	for( ; itr != m_cells.end(); ++itr)
	{
		delete itr->second;
	}

	m_cells.clear();
}

} // gk
