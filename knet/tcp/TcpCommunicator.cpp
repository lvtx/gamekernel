#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/tcp/TcpCommunicator.h>

#include <kcore/sys/Logger.h>
#include <knet/group/NmGroupRelay.h>
#include <knet/message/net/NetMessageTypes.h>
#include <knet/message/net/NetStateMessage.h>

namespace gk {

TcpCommunicator::TcpCommunicator()
: m_ios( 0 )
, m_connections()
, m_acceptors()
, m_connector()
, m_messages()
, m_nextConnectionId( 1 )
{
}

TcpCommunicator::~TcpCommunicator()
{
    Fini();
}

bool 
TcpCommunicator::Init( MessageListener* listener, IoService* ios )
{
    K_ASSERT( listener != 0 );
	K_ASSERT( ios != 0 );

    m_listener 	= listener;
	m_ios 		= ios;

	m_nextConnectionId = 1;

	m_connector.Init( this );

	LOG( FT_INFO, _T("TcpCommunicator::Init> Inited") );

    return true;
}

bool
TcpCommunicator::Listen( const IpAddress& addr, SecurityLevel sl )
{
	AcceptorMap::iterator i( m_acceptors.find( addr.GetKey() ) );		

	if ( i != m_acceptors.end() )
	{
		LOG( FT_WARN, 
			 _T("TcpCommunicator::Listen> %s already listening"), 
		     addr.ToString().c_str() );

		return true; 
	}

	Socket* s = new Socket;

	s->CreateTcpSocket();
	
	bool rc = s->Bind( addr );

	if ( !rc )
	{
		delete s;
		
		LOG( FT_ERROR, 
			 _T("TcpCommunicator::Listen> Failed to listen on %s"), 
			 addr.ToString().c_str() );

		return false;
	}

	s->Listen();

	Acceptor* acceptor = new Acceptor;

	rc = acceptor->Init( this, s, sl );

	if ( !rc )
	{
		acceptor->Fini(); // socket is delete here

		delete acceptor;

		LOG( FT_ERROR, 
			 _T("TcpCommunicator::Listen> Failed to init acceptor on %s"), 
			 addr.ToString().c_str() );

		return false;
	}

	LOG( FT_INFO, 
		 _T("TcpCommunicator::Listen> Start listening on %s"), 
		 addr.ToString().c_str() );

	m_acceptors.insert( AcceptorMap::value_type( addr.GetKey(), acceptor ) );

	return true;
}

void 
TcpCommunicator::StopListen( const IpAddress& addr )
{
	AcceptorMap::iterator i( m_acceptors.find( addr.GetKey() ) );		

	if ( i != m_acceptors.end() )
	{
		Acceptor* acceptor = i->second;

		acceptor->Fini();

		delete acceptor;

		m_acceptors.erase( i );
	}
}

void
TcpCommunicator::Connect( const IpAddress& remote )
{
	m_connector.Connect( remote );
}

void 
TcpCommunicator::Close( uint connectionId )
{
	TcpConnection* c = FindById( connectionId );

	if ( c != 0 )
	{
		c->Close();
	}
}

void
TcpCommunicator::Send( uint connectionId, MessagePtr m )
{
	ConnectionMap::iterator i( m_connections.find( connectionId ) );

	if ( i != m_connections.end() )
	{
		TcpConnection* c = i->second;
		K_ASSERT( c != 0 );

		c->Send( m );
	}
	else
	{
		LOG( FT_WARN, 
			 _T("TcpCommunicator::Send> %d not found"), 
			 connectionId );
	}
}

void 
TcpCommunicator::Send( const std::vector<uint> connections, MessagePtr m )
{
	std::vector<uint>::const_iterator i( connections.begin() );
	std::vector<uint>::const_iterator iEnd( connections.end() );

	for ( ; i != iEnd; ++i )
	{
		Send( *i, m );
	}
}

void
TcpCommunicator::Run()
{
	processConnections();
	processMessages();
}

void 
TcpCommunicator::Notify( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );
	K_ASSERT( m_listener != 0 );

	LOG( FT_DEBUG, _T("TcpCommunicator::Notify> %d"), m->type );

	switch ( m->type )
	{
	case NET_STATE_MESSAGE:
	case NET_GROUP_RELAY:
	// add more message if required
		m_messages.Put( m );
		return;
	}

	m_listener->Notify( m );
}

void
TcpCommunicator::Fini()
{
	LOG( FT_DEBUG, _T("TcpCommunicator::Fini> start to finish") );

	cleanupMessages();

	LOG( FT_DEBUG, _T("TcpCommunicator::Fini> messages") );

	cleanupConnections();

	LOG( FT_DEBUG, _T("TcpCommunicator::Fini> connections") );

	cleanupAcceptors();

	LOG( FT_DEBUG, _T("TcpCommunicator::Fini> acceptors") );

	m_connector.Fini();

	LOG( FT_INFO, _T("TcpCommunicator::Fini> Finished") );
}

TcpConnection* 
TcpCommunicator::FindById( uint id )
{
	ConnectionMap::iterator i( m_connections.find( id ) );

	if ( i == m_connections.end() )
	{
		return (TcpConnection*)0;
	}

	return i->second;
}

void 
TcpCommunicator::processConnections()
{
    ConnectionMap::iterator i(m_connections.begin());

    for ( ; i != m_connections.end(); )
    {
        TcpConnection* c = i->second;
        K_ASSERT( c != 0 );

        if ( c )
        {
            if ( c->HasError() )
            {
                if ( c->GetRequestCount() == 0 )
                {
                    // message notification is done on closed or on error

					LOG( FT_DEBUG, 
						 _T("TcpCommunicator::processConnections> Cleaning %d"), 
						 c->GetId() );

                    delete c;

                    i = m_connections.erase( i );

                    continue;
                }
            }
            else
            {
                c->Run();
            }
        }

        ++i;
    }
}

void 
TcpCommunicator::processMessages()
{
	MessagePtr m;

	while ( m_messages.Get( m ) )
	{
		K_ASSERT( m.Get() != 0 );

		switch ( m->type ) 
		{
		case NET_STATE_MESSAGE:
			onStateMessage( m );
			break;
		case NET_GROUP_RELAY:
			onRelayMessage( m );
			break;
		default:
			LOG( FT_WARN, 
				 _T("TcpCommunicator::processMessages> Unknown %d"), 
				 m->type );
			break;
		}
	}
}

void 
TcpCommunicator::onStateMessage( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	NetStateMessage* nsm = static_cast<NetStateMessage*>( m.Get() );

	switch ( nsm->state )
	{
	case NetStateMessage::TCP_CONNECTED:
		{
			// Security level is set during handshake
			onNewConnection( nsm->socket, SECURITY0, false );
		}
		break;
	case NetStateMessage::TCP_ACCEPTED:
		{
			onNewConnection( nsm->socket, nsm->sl, true );
		}
		break;
	case NetStateMessage::TCP_CONNECT_FAIL:
	case NetStateMessage::TCP_OPEN:
	case NetStateMessage::TCP_CLOSED:
		{
			LOG( FT_DEBUG, _T("TcpCommunicator::onStateMessage> Notify OPEN/CLOSE/CONNECT_FAIL") );
			m_listener->Notify( m );
		}
		break;
	}
}

void 
TcpCommunicator::onRelayMessage( MessagePtr m )
{
	NmGroupRelay* relay = static_cast<NmGroupRelay*>( m.Get() );

	LOG( FT_DEBUG, 
		 _T("TcpCommunicator::onRelayMessage> len %d"), 
		 relay->len );

	std::vector<uint>::iterator i 	 = relay->relays.begin();
	std::vector<uint>::iterator iEnd = relay->relays.end();

	for ( ; i != iEnd; ++i )
	{
		// send only data part

		TcpConnection* c = FindById( *i );

		if ( c != 0 )
		{
			c->Send( relay->data, relay->len );
		}
	}
}

void 
TcpCommunicator::onNewConnection( Socket* s, SecurityLevel sl, bool accepted )
{
	K_ASSERT( s != 0 );

	TcpConnection* c = new TcpConnection;

	bool rc = c->Init( this, ++m_nextConnectionId, s, sl, accepted );

	if ( !rc )
	{
		LOG( FT_ERROR, 
			 _T("TcpCommunicator::onNewConnection> Connection init failed on Socket %d"), 
			 s->GetSystemSocket() );
		
		c->Fini();

		delete c;

		return;
	}

	rc = m_ios->BindIo( c );

	if ( !rc )
	{
		LOG( FT_ERROR, 
			 _T("TcpCommunicator::onNewConnection> Bind io failed on Socket %d"), 
			 s->GetSystemSocket() );

		 c->Fini();

		delete c;

		return;
	}

	// race condition can happen 

	if ( accepted )
	{
		rc = m_ios->BeginRecv( c );

		if ( !rc )
		{
			LOG( FT_ERROR, _T("TcpCommunicator::onNewConnection> %d BeginRecv fail"), c->GetId() );

			c->Fini();

			delete c;

			return;
		}
	}

	m_connections.insert( ConnectionMap::value_type( c->GetId(), c ) );

	if ( accepted )
	{
		c->StartHandshake();
	}
	else
	{
		c->WaitHandshake();
	}

	LOG( FT_DEBUG, 
		 _T("TcpCommunicator::onNewConnection> Connection %d Socket %d open"), 
		 c->GetId(), 
		 s->GetSystemSocket() );
}

void 
TcpCommunicator::cleanupMessages()
{
	MessagePtr m;

	while ( m_messages.Get( m ) )
	{
		K_ASSERT( m.Get() != 0 );

		switch ( m->type ) 
		{
		case NET_STATE_MESSAGE:
			{
				NetStateMessage* nsm = static_cast<NetStateMessage*>( m.Get() );

				if ( nsm->socket != 0 )
				{
					delete nsm->socket;
					nsm->socket = 0;
				}
			}
			break;
		default:
			LOG( FT_WARN, 
				 _T("TcpCommunicator::cleanupMessages> Message %d"), 
				 m->type );
			break;
		}
	}

}

void 
TcpCommunicator::cleanupConnections()
{
    ConnectionMap::iterator i( m_connections.begin());
    ConnectionMap::iterator iEnd( m_connections.end());

    for ( ; i != iEnd; ++i )
    {
        TcpConnection* c = i->second;
        K_ASSERT( c != 0 );

        c->Fini(); 

        delete c;
    }

    m_connections.clear();
}

void 
TcpCommunicator::cleanupAcceptors()
{
	AcceptorMap::iterator i( m_acceptors.begin() );
	AcceptorMap::iterator iEnd( m_acceptors.end() );

	for ( ; i != iEnd; ++i )
	{
		Acceptor* acceptor = i->second;
		K_ASSERT( acceptor != 0 );

		acceptor->Fini();

		delete acceptor;
	}

	m_acceptors.clear();
}

} // gk
