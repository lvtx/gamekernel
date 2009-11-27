#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/db/TransactionServer.h>

namespace gk { 

TransactionServer::TransactionServer()
: m_node( 0 )
, m_pool( 0 )
{
}

TransactionServer::~TransactionServer()
{
	Fini();
}

bool 
TransactionServer::Init( Node* node, DbPool* pool )
{
	K_ASSERT( node != 0 );
	K_ASSERT( pool != 0 );

	m_node = node;
	m_pool = pool;
	m_nextTxNo = 1;

	for ( int i=0; i<RUNNER_COUNT; ++i )
	{
		bool rc = m_runners[i].Init( node, pool );

		if ( !rc )
		{
			return false;
		}
	}

	LOG( FT_INFO, _T("TransactionServer::Init>") );

	return true;
}

uint 
TransactionServer::Prepare( Transaction* tx )
{
	K_ASSERT( m_node != 0 );
	K_ASSERT( m_pool != 0 );
	K_ASSERT( tx != 0 );

	tx->SetTxNo( m_nextTxNo );
	m_nextTxNo.Inc();

	return tx->GetTxNo();
}

void
TransactionServer::Request( Transaction* tx )
{
	K_ASSERT( m_node != 0 );
	K_ASSERT( m_pool != 0 );
	K_ASSERT( tx != 0 );
	K_ASSERT( tx->GetTxNo() > 0 );

	int targetRunner = 0;

	uint targetTransCount = m_runners[targetRunner].GetTransactionCount(); 
	
	for ( int i=0; i<RUNNER_COUNT; ++i )
	{
		if ( targetTransCount > m_runners[i].GetTransactionCount() )
		{
			targetRunner 	 = i;
			targetTransCount = m_runners[i].GetTransactionCount(); 
		}
	}

	m_runners[targetRunner].Request( tx );
}

void 
TransactionServer::Fini()
{
	for ( int i=0; i<RUNNER_COUNT; ++i )
	{
		m_runners[i].Fini();
	}

	if ( m_pool != 0 )
	{
		m_pool->Fini();

		delete m_pool;

		m_pool = 0;
	}

	LOG( FT_INFO, _T("TransactionServer::Fini>") );
}

} // gk
