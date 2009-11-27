#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/CellRunner.h>
#include <kserver/cell/Cell.h>

namespace gk {

CellRunner::CellRunner()
{
}

CellRunner::~CellRunner()
{
	Fini();
}

bool 
CellRunner::Init( Cell* cell )
{
	K_ASSERT( cell != 0 );

	m_cell		= cell;

	return true;
}

bool 
CellRunner::MakeActive()
{
	K_ASSERT( !IsRunning() );

	Start();

	return IsRunning();
}

bool 
CellRunner::MakePassive()
{
	K_ASSERT( IsRunning() );

	Stop();

	return !IsRunning();
}

int 
CellRunner::Run()
{
	K_ASSERT( m_cell != 0 );

	while ( IsRunning() )
	{
		m_cell->ResetProcessCount();

		m_cell->Run();

		if ( m_cell->GetProcessCount() == 0 )
		{
			::Sleep( 1 ); // XXX: active logic can be delayed 1 ms when there is no traffic
		}
	}

	return 0;
}

void 
CellRunner::Fini()
{
	Stop();

	m_cell = 0;
}

} // gk
