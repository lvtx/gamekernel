#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/test/TestAction.h>

#include <kcore/xml/tinyxml.h>
#include <kserver/cell/Cell.h>

namespace gk {

class DummyCell : public Cell
{
public:
	Cell* Create() 
	{
		return new DummyCell;
	}
};

TestAction::TestAction()
: m_cell( 0 )
{
}

TestAction::~TestAction()
{
	if ( m_cell != 0 )
	{
		delete m_cell;

		m_cell = 0;
	}
}

bool 
TestAction::Init( NetClient* client, uint idx, TiXmlElement* xe )
{
	m_cell = new DummyCell;

	if ( !load( xe ) )
	{
		return false;
	}

	m_client = client;
	m_idx    = idx;
	
	return Action::Init( m_cell );
}

bool 
TestAction::load( TiXmlElement* /* xe */ )
{
	return true;
}

} // gk
