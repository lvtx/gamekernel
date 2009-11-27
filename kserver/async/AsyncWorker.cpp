#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/async/AsyncWorker.h>
#include <kserver/Node.h>

namespace gk 
{

AsyncWorker::AsyncWorker()
: m_node( 0 )
, m_port( INVALID_HANDLE_VALUE )
, m_recvQ()
{

}

AsyncWorker::~AsyncWorker()
{
	Fini();
}

bool 
AsyncWorker::Init( Node* node )
{
	K_ASSERT( node != 0 );

	m_node = node;

	m_port = ::CreateIoCompletionPort( INVALID_HANDLE_VALUE, 0, 0, 0 );

    if ( m_port == INVALID_HANDLE_VALUE )
    {
        return false;
    }

	Start();

	return init() && IsRunning();
}

int 
AsyncWorker::Run()
{
	while ( IsRunning() )
	{
		MessagePtr m;

		if ( m_recvQ.Get( m ) )
		{
			onMessage( m );
		}

		// order is important to stop thread. See Fini()
		
        ulong           bytes       = 0;
        ulong           perIoKey    = 0;
        LPOVERLAPPED    ov          = 0;

        (void)::GetQueuedCompletionStatus( 
                        m_port, 
                        (LPDWORD)&bytes, 
                        (PULONG_PTR)&perIoKey, 
                        &ov, 
                        INFINITE );
	}

	return 0;
}

void 
AsyncWorker::Notify( MessagePtr m )
{
	m_recvQ.Put( m );

    ::PostQueuedCompletionStatus( m_port, 0, 0, 0 );
}

void 
AsyncWorker::Fini()
{
	Stop();

	fini();

    if ( m_port != INVALID_HANDLE_VALUE )
    {
        CloseHandle( m_port );

        m_port = INVALID_HANDLE_VALUE;
    }
}

void 
AsyncWorker::helpStop()
{
	::PostQueuedCompletionStatus( m_port, 0, 0, 0 );
	::PostQueuedCompletionStatus( m_port, 0, 0, 0 );
}

bool 
AsyncWorker::init()
{
	return true;
}

void 
AsyncWorker::onMessage( MessagePtr /* m */ )
{

}

void 
AsyncWorker::fini()
{

}

} // gk
