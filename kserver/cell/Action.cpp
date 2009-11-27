#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/Action.h>
#include <kserver/cell/Cell.h>

namespace gk {

Action::Action()
: m_cell( 0 )
, m_finished( true )
, m_timeout( 0 )
, m_age( 0 )
, m_hsm( 0 )
{
}

Action::~Action()
{
}

bool 
Action::Init( Cell* cell )
{
	K_ASSERT( cell != 0 );

	m_cell 		= cell;
	m_timeout 	= 0;
	m_age 		= 0;
	m_hsm		= 0;
	
	m_finished = !init();

	LOG( FT_DEBUG, _T("Action::Init> %s"), m_name.c_str() );

	if ( !m_finished )
	{
		if ( m_hsm != 0 )
		{
			m_hsm->Init( this );
		}
	}

	return !m_finished;
}

bool 
Action::Run( uint age )
{
	K_ASSERT( m_cell != 0 );

	m_age += age;

	if ( m_timeout > 0 && m_age > m_timeout )
	{
		timeout();

		return false;
	}

	if ( m_hsm != 0 )
	{
		m_hsm->OnTick( age );
	}

	return run( age );
}

bool 
Action::Run( MessagePtr m )
{
	K_ASSERT( m_cell != 0 );

	if ( m_hsm != 0 )
	{
		m_hsm->OnMessage( m );
	}

	return run( m );
}

bool 
Action::Completed( Transaction* trans )
{
	K_ASSERT( trans != 0 );

	return completed( trans );
}

void 
Action::Fini()
{
	if ( m_hsm != 0 )
	{
		m_hsm->Fini();

		delete m_hsm;

		m_hsm = 0;
	}

	fini();

	m_finished = true;

	LOG( FT_DEBUG, _T("Action::Fini> %s"), m_name.c_str() );
}

bool 
Action::init()
{
	return true;
}

bool 
Action::run( uint /* tick */ )
{
	return true;	
}

bool 
Action::run( MessagePtr /* m */ )
{
	return true;
}

bool 
Action::completed( Transaction* /* trans */ )
{
	return true;
}

void 
Action::timeout()
{
	// empty
}

void 
Action::fini()
{
	// empty
}

void 
Action::setFinished()
{
	m_finished = true;
}

} // gk
