#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/tcp/impl/Connector.h>

#include <kcore/sys//Logger.h>
#include <knet/socket/Socket.h>
#include <knet/tcp/TcpCommunicator.h>
#include <knet/message/net/NetStateMessage.h>

namespace gk {

Connector::Connector()
: m_communicator( 0 )
{
}

Connector::~Connector()
{
    Fini();
}

bool 
Connector::Init( TcpCommunicator* communicator )
{
	K_ASSERT( communicator != 0 );

	m_communicator = communicator;

    Start();

    return true;
}

void 
Connector::Connect( const IpAddress& remote )
{
	m_reqQ.Put( remote );
}

int 
Connector::Run()
{
    while ( IsRunning() )
    {
		if ( m_reqQ.IsEmpty() )
		{
			Sleep( 10 );

			continue;
		}

		IpAddress addr; 

		bool rc = m_reqQ.Get( addr );
		K_ASSERT( rc );

		Socket* s = new Socket;
		
		s->CreateTcpSocket();

		if ( s->Connect( addr ) )
		{
			NetStateMessage* m = new NetStateMessage;

			m->state 		= NetStateMessage::TCP_CONNECTED;
			m->connectionId = 0;
			m->addr 		= addr;
			m->socket 		= s;

			m_communicator->Notify( MessagePtr( m ) );

			LOG( FT_DEBUG, 
					_T("Connector> Connected %d to %s"), 
					s->GetSystemSocket(), 
					addr.ToString().c_str() );
		}
		else
		{
			delete s;

			NetStateMessage* m = new NetStateMessage;

			m->state 		= NetStateMessage::TCP_CONNECT_FAIL;
			m->connectionId = 0;
			m->addr 		= addr;
			m->socket 		= 0;

			m_communicator->Notify( MessagePtr( m ) );

			LOG( FT_DEBUG, 
					_T("Failed to connect to %s"), 
					addr.ToString().c_str() );
		}
    }

    return 0;
}

void 
Connector::Fini()
{
    Stop();

}

} // gk 