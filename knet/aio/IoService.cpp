#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/aio/IoService.h>
#include <kcore/sys/Logger.h>

namespace gk {

IoService::IoService()
: m_port( INVALID_HANDLE_VALUE ), 
  m_workerCount( 0 )
{
}

IoService::~IoService()
{
    Fini();
}

bool 
IoService::Init()
{
    m_port = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 );

    if ( m_port == INVALID_HANDLE_VALUE )
    {
        return false;
    }
    
    int properWorkerCount = getProperWorkerCount();

    if ( properWorkerCount > MAX_WORKERS )
    {
        properWorkerCount = MAX_WORKERS;
    }

    m_workerCount = 0;

    for ( int i = 0; i < properWorkerCount; ++i )
    {
        if ( m_workers[i].Init( m_port ) )
        {
            ++m_workerCount;
        }
    }

    return ( m_port != 0 && m_workerCount > 0 );
}

bool 
IoService::BindIo( IoAgent* agent )
{
    K_ASSERT( agent != 0 );
    K_ASSERT( agent->RequestHandle() != INVALID_HANDLE_VALUE );

    HANDLE v = ::CreateIoCompletionPort( 
                        agent->RequestHandle(), 
                        m_port, 
                        (ULONG_PTR)agent, 
                        0 );        

    LOG( FT_DEBUG, 
         _T("[IoService] Bound 0x%x to handle %d"), 
         m_port, 
         agent->RequestHandle() );

    return ( v != 0 && v == m_port ); 
}

void 
IoService::UnbindIo( IoAgent* agent )
{
    K_ASSERT( agent != 0 );
	
	// hmm... UnbindIo
}

bool 
IoService::BeginSend( IoAgent* agent )
{
    K_ASSERT( agent != 0 );

    return agent->RequestSend();
}

bool 
IoService::BeginRecv( IoAgent* agent )
{
	K_ASSERT( agent != 0 );

    return agent->RequestRecv();
}

void 
IoService::Fini()
{
    for ( int i = 0; i < m_workerCount; ++i )
    {
        m_workers[i].Fini();
    }

    if ( m_port != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_port );

        m_port = INVALID_HANDLE_VALUE;
    }

	LOG( FT_INFO, _T("IoService::Fini> Finished") );
}

uint 
IoService::getProperWorkerCount() const
{
    SYSTEM_INFO si;

    GetSystemInfo( &si );

    return si.dwNumberOfProcessors;
}

} // gk
