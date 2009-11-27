#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/tcp/impl/Acceptor.h>

#include <kcore/sys/Logger.h>
#include <knet/message/net/NetStateMessage.h>
#include <knet/tcp/TcpCommunicator.h>

namespace gk {

Acceptor::Acceptor()
: m_communicator( 0 )
, m_socket( 0 )
{
}

Acceptor::~Acceptor()
{
   	Fini();
}

bool 
Acceptor::Init( TcpCommunicator* communicator, Socket* socket, SecurityLevel sl )
{
	K_ASSERT( communicator != 0 );
	K_ASSERT( socket != 0 );

	m_communicator  = communicator;
	m_socket 		= socket;
	m_sl 			= sl;

    Start();

    return true;
}

int 
Acceptor::Run()
{
    while ( IsRunning() )
    {
        Socket* socket = m_socket->Accept();
        K_ASSERT( socket != 0 );

		if ( !IsRunning() ) // check running state again
		{
			break;
		}

		NetStateMessage* m = new NetStateMessage;

		m->state  = NetStateMessage::TCP_ACCEPTED;
        m->addr   = socket->GetPeerAddress();
        m->socket = socket;
		m->sl 	  = m_sl;

		m_communicator->Notify( MessagePtr( m ) );

        LOG( FT_DEBUG,
             _T("Accepted new connection %d from %s"),
             socket->GetSystemSocket(),
             m->addr.ToString().c_str() );
    }

    return 0;
}

void
Acceptor::Fini()
{
    Stop();

    if ( m_socket != 0 )
    {
        m_socket->Close();

        delete m_socket;

        m_socket = 0;
    }

	LOG( FT_INFO, _T("Acceptor::Fini> Finished") );
}

void
Acceptor::helpStop()
{
    Socket client;

    if ( client.CreateTcpSocket() )
    {
		IpAddress addr;

		addr.Init( _T("127.0.0.1"), m_socket->GetAddress().GetPort() );

        (void)client.Connect( m_socket->GetAddress() );
		(void)client.Connect( addr );
    }
}

} // gk
