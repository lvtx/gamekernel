#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/db/TransactionRunner.h>

#include <kserver/Node.h>

namespace gk {

TransactionRunner::TransactionRunner()
: m_node( 0 )
, m_pool( 0 )
{

}

TransactionRunner::~TransactionRunner()
{
	Fini();
}

bool 
TransactionRunner::Init( Node* node, DbPool* pool )
{
	K_ASSERT( node != 0 );
	K_ASSERT( pool != 0 );

	m_node = node;
	m_pool = pool;
	m_transCount = 0;

	Start();

	return IsRunning();
}

void 
TransactionRunner::Request( Transaction* tx )
{
	m_transCount.Inc();

	m_transQ.Put( tx );
}

int 
TransactionRunner::Run() 
{
	while ( IsRunning() )
	{
		processTransQ();

		::Sleep( 10 );
	}

	return 0;
}

void 
TransactionRunner::Fini()
{
	Stop();
	
	processTransQ();
}

uint 
TransactionRunner::GetTransactionCount() const
{
	return m_transCount;
}

void 
TransactionRunner::processTransQ()
{
	Transaction* tx = 0;

	while ( m_transQ.Get( tx ) )
	{
		processTrans( tx );
	}
}

void 
TransactionRunner::processTrans( Transaction* tx )
{
	K_ASSERT( tx != 0 );

	LOG( FT_DEBUG, _T("TransactionRunner::processTrans> %d"), tx->GetTxNo() );

	tx->Execute( m_pool );	

	CellId dst = tx->GetCellId();

	if ( !dst.IsValid() )
	{
		delete tx;

		return;
	}

	m_node->Completed( tx );

	m_transCount.Dec();
}

} // gk
