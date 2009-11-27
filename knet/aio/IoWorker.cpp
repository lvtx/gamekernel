#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/aio/IoWorker.h>
#include <knet/aio/IoAgent.h>
#include <knet/aio/IoBlock.h>
#include <kcore/sys/Logger.h>

namespace gk {

IoWorker::IoWorker()
: port_( 0 )
{
}

IoWorker::~IoWorker()
{
    Fini();
}

bool 
IoWorker::Init( HANDLE port )
{
    K_ASSERT( port != 0 );

    port_ = port;

    Start();

    return IsRunning();
}

int 
IoWorker::Run()
{
    LOG( FT_SERVICE, _T("IoWorker started") );

    while ( IsRunning() )
    {
        // [1] Get completion status

        ulong           bytes       = 0;
        ulong           perIoKey    = 0;
        LPOVERLAPPED    ov          = 0;

        int rc = ::GetQueuedCompletionStatus( 
                        port_, 
                        (LPDWORD)&bytes, 
                        (PULONG_PTR)&perIoKey, 
                        &ov, 
                        INFINITE );

		if ( !IsRunning() )
		{
			break;
		}

        if ( perIoKey == NULL ) // shutdown
        {
            break;
        }

        // [2] Branch on rc 

        if ( rc == 0 )  
        {
            handleRcZero( bytes, perIoKey, ov );
        }
        else
        {
            handleRcNonZero( bytes, perIoKey, ov );
        }
    }

    LOG( FT_SERVICE, _T("IoWorker stopped") );

    return 0;
}

void
IoWorker::handleRcZero( ulong bytes, 
                             ulong perIoKey, 
                             LPOVERLAPPED ov )
{
    // when rc == 0, there are two cases

    bytes;
    perIoKey;

    if ( ov == NULL )  // This seems not happen for INFINITE wait
    {
        // GetQueuedCompletionStatus failed

        return;
    }

    // ov != NULL && perIoKey && bytes saved

    K_ASSERT( ov != NULL );

    // This is when error occurs on perIoKey

    IoBlock* ab = (IoBlock*)(ov);

    IoAgent* c = (IoAgent*)ab->extra;
    K_ASSERT( c != 0 );

    int errorCode = GetLastError();

    if ( errorCode == 0 )
    {
        errorCode = WSAESHUTDOWN;
    }

    c->OnIoError( errorCode, ab );
}

void
IoWorker::handleRcNonZero( ulong bytes, 
                                ulong perIoKey, 
                                LPOVERLAPPED ov )
{
    K_ASSERT( ov != 0 );  // Is this guaranteed? 

    perIoKey;

    IoBlock* ab = (IoBlock*)ov;
    K_ASSERT( ab != 0 );

    ab->transferred = bytes;

    switch ( ab->op )
    {
    case IoBlock::OP_READ:
        {
            IoAgent* c = (IoAgent*)ab->extra;
            K_ASSERT( c != 0 );

            if ( bytes == 0 )
            {
                if ( ab->buf.len != 0 ) // if not 0-recv
                {
                    c->OnIoError( WSAECONNRESET, ab );

                    return;
                }
            }

            c->OnRecvCompleted( ab ); // ask for recv again 
        }
        break;
    case IoBlock::OP_WRITE:
        {
            IoAgent* c = (IoAgent*)ab->extra;
            K_ASSERT( c != 0 );

            if ( bytes == 0 ) 
            {
                K_ASSERT( ab->buf.len > 0 );
                K_ASSERT( ab->totalLen > 0 );

                c->OnIoError( WSAECONNRESET, ab ); 

                return;
            }

            if ( ab->totalLen > bytes )
            {
                c->OnSendCompleted( ab, bytes );
            }
            else
            {
                c->OnSendCompleted( ab ); // ask for send again if required
            }
        }
        break;
    default:
        K_ASSERT( !_T("Should not reach here") );
        break;
    }
}

void 
IoWorker::Fini()
{
    if ( !IsRunning() )
	{
		return;
	}

    Stop();

	LOG( FT_INFO, _T("IoWorker::Fini> Finished") );
}

void 
IoWorker::helpStop()
{
    // NOTE:     
    // when ::PostQueuedCompletionStatus( port_, 0, 0, 0 ); is passed to IOCP, 
    // it can be routed to any worker thread. 
    // So we need to have enough PQCS requests to shutdown all threads. 
    //
    // This is strange, but it is identified during unit test once. 

    ::PostQueuedCompletionStatus( port_, 0, 0, 0 );
    ::PostQueuedCompletionStatus( port_, 0, 0, 0 );
    ::PostQueuedCompletionStatus( port_, 0, 0, 0 );
}

} // gk 
