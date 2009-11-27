#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/udp/UdpCommunicator.h>

#include <knet/udp/impl/UdpConnection.h>
#include <knet/group/NmGroupRelay.h>
#include <knet/message/Message.h>
#include <knet/message/MessageFactory.h>
#include <knet/message/net/NetStateMessage.h>
#include <knet/tcp/impl/TcpConnection.h>
#include <kcore/sys/ScopedLock.h>
#include <kcore/sys/Logger.h>

#include <algorithm>

namespace 
{
struct fun_is_error_connection 
{
	bool operator()( std::pair<const uint, gk::UdpConnection*>& v ) 
	{
		return ( v.second->GetError() > 0 || v.second->IsClosed() );
	}
};
} // anonymous namespace

namespace gk {

UdpCommunicator::UdpCommunicator()
: m_socket() 
, m_listener( 0 )
, m_ios( 0 )
, m_selfTag( 0 )
, m_relay( 0 )
, m_recvBuffer( 1024 )
, m_recvCount( 0 )
, m_connections()
, m_connLock()
{
}

UdpCommunicator::~UdpCommunicator()
{
}

bool 
UdpCommunicator::Init( MessageListener* listener, 
		               IoService* ios, 
					   const IpAddress& addr,
					   uint selfTag, 
					   SecurityLevel sl, 
					   byte* challenge,
					   TcpConnection* relay  )
{
	K_ASSERT( listener != 0 );
	K_ASSERT( ios != 0 );

	m_listener  = listener;
	m_ios 		= ios;
	m_relay 	= relay;
	m_sl 		= sl;
	m_selfTag   = selfTag;

	if ( sl > SECURITY0 )
	{
		m_cipher.Init( challenge, Cipher::LEN_CHALLENGE, true );
		m_cipher.SetEstablished( true );
	}
	
	if ( !m_socket.CreateUdpSocket() )
	{
		return false;
	}

	if ( !m_socket.Bind( addr ) )
	{
		LOG( FT_ERROR, _T("UdpCommunicator::Init> Socket bind %s"), addr.ToString().c_str() );

		return false;
	}

	m_socket.SetNonblocking();
	m_socket.SetIgnoreUdpReset();

	if ( !m_ios->BindIo( this ) )
	{
		LOG( FT_ERROR, _T("UdpCommunicator::Init> Bind IO error") );

		return false;
	}

	LOG( FT_DEBUG, _T("UdpCommunicator::Init> Self[%d] Addr[%s]"), 
		 m_selfTag, addr.ToString().c_str() );

	RequestRecv();
	
	return true;
}

uint
UdpCommunicator::Connect( uint remoteTag, const IpAddress& in, const IpAddress& ex )
{
	K_ASSERT( m_ios != 0 );
	K_ASSERT( m_listener != 0 );
	K_ASSERT( m_socket.GetSystemSocket() != INVALID_SOCKET );

	UdpConnection* c = FindByTag( remoteTag );

	if ( c != 0 )
	{
		return c->GetRemoteTag();
	}

	c = new UdpConnection;

	(void)c->Init( this, m_selfTag, remoteTag, in, ex ); // start hole punching

	{
		ScopedLock sl( m_connLock );

		m_connections.insert( ConnectionMap::value_type( c->GetRemoteTag(), c ) );
	}

	return c->GetRemoteTag();
}

void 
UdpCommunicator::Close( uint connectionId )
{
	K_ASSERT( m_ios != 0 );
	K_ASSERT( m_listener != 0 );
	K_ASSERT( m_socket.GetSystemSocket() != INVALID_SOCKET );

	UdpConnection* c = FindByTag( connectionId );	

	if ( c == 0 )
	{
		LOG( FT_WARN, 
			 _T("UdpCommunicator::Close> %d not found"), 
			 connectionId );

		return;
	}

	c->Close();
}

void 
UdpCommunicator::CloseByTag( uint tag )
{
	K_ASSERT( m_ios != 0 );
	K_ASSERT( m_listener != 0 );
	K_ASSERT( m_socket.GetSystemSocket() != INVALID_SOCKET );

	UdpConnection* c = FindByTag( tag );	

	if ( c == 0 )
	{
		LOG( FT_WARN, 
			 _T("UdpCommunicator::CloseByTag> %d not found"), 
			 tag );

		return;
	}

	c->Close();
}

void 
UdpCommunicator::Run()
{
	if ( m_ios == 0  || m_listener == 0 )
	{
		return;
	}

	processConnections();
}

void
UdpCommunicator::Send( uint tag, MessagePtr m, int qos )
{
	K_ASSERT( m_ios != 0 );
	K_ASSERT( m_listener != 0 );
	K_ASSERT( m_socket.GetSystemSocket() != INVALID_SOCKET );

	UdpConnection* c = FindByTag( tag );

	send( c, m, qos );
}

void
UdpCommunicator::SendByTag( MessagePtr m, int qos )
{
	K_ASSERT( m_ios != 0 );
	K_ASSERT( m_listener != 0 );
	K_ASSERT( m_socket.GetSystemSocket() != INVALID_SOCKET );

	if ( m->remote == 0 )
	{
		multicast( m->remotes, m, qos );
	}
	else
	{
		UdpConnection* c = FindByTag( m->remote );

		send( c, m, qos );
	}
}

void 
UdpCommunicator::Broadcast( MessagePtr m, int qos )
{
	BitStream bs;

	bool rc = m->Pack( bs );

	if ( !rc )
	{
		LOG( FT_WARN, 
			 _T("UdpCommunicator::Broadcast> %d pack error"), 
			 m->type );

		return;
	}

	Message::RemoteList remotes;

	ConnectionMap::iterator i( m_connections.begin() );
	ConnectionMap::iterator iEnd( m_connections.end() );
	
	for ( ; i != iEnd; ++i )
	{
		UdpConnection* c = i->second;
		K_ASSERT( c != 0 );

		remotes.push_back( c->GetRemoteTag() );
	}

	multicast( m->remotes, m, qos );
}

void 
UdpCommunicator::SendTo( byte* data, uint len, const IpAddress& to )
{
	if ( m_sl > SECURITY0 )
	{
		BitStream bs;

		bs.Write( len, data );

		m_cipher.Encrypt( bs );		

		m_socket.SendTo( bs.GetBuffer(), bs.GetBytePosition(), to );

		LOG( FT_DEBUG_FLOW, _T("SendTo> Encrypted %d To %s"), len, to.ToString().c_str() );
	}
	else
	{
		m_socket.SendTo( data, len, to );

		LOG( FT_DEBUG_FLOW, _T("SendTo> Plain %d To %s"), len, to.ToString().c_str() );
	}
}

void 
UdpCommunicator::OnRecv( uint remoteTag, void* data, uint len )
{
	K_ASSERT( m_ios != 0 );
	K_ASSERT( m_listener != 0 );
	K_ASSERT( m_socket.GetSystemSocket() != INVALID_SOCKET );
	K_ASSERT( remoteTag > 0 );
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );

	BitStream bs;

	bs.Write( len, data );

	if ( !bs.IsValid() )
	{
		// log error
		return;
	}

	ushort type;

	bs.Reset();
	bs.Read( type );

	MessagePtr m = MessageFactory::Instance()->Create( type );
	
	if ( m.Get() == 0 )
	{
		LOG( FT_ERROR, 
			 _T("UdpCommunicator::OnRecv> Type %d not registered"), 
			 type );

		return;
	}

	m->remote = remoteTag;

	// type is already read in 
	// Message does not read in type.

	bool rc = m->Unpack( bs );

	if ( !rc )
	{
		LOG( FT_ERROR, 
			 _T("UdpCommunicator::OnRecv> Type %d unpack failed"), 
			 type );

		return;
	}

	m_listener->Notify( m );
}

void 
UdpCommunicator::Notify( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	// Do we need to block UDP_CLOSED notification?
	//

	m_listener->Notify( m );
}

void 
UdpCommunicator::CheckRelay( uint connectionId )
{
	if ( m_relay == 0 ) return;

	if ( connectionId == m_relay->GetId() )
	{
		m_relay = 0; // it is disconnected
	}
}

// IoAgent {
HANDLE 
UdpCommunicator::RequestHandle()
{
	return (HANDLE)m_socket.GetSystemSocket();
}

bool 
UdpCommunicator::RequestSend()
{
	K_ASSERT( !_T("UDP does not use IoService to send") );

	return false;	
}

bool 
UdpCommunicator::RequestRecv()
{
	ScopedLock sl( m_ioLock );

	if ( m_recvCount > 0 )
	{
		return true;
	}

	K_ASSERT( m_recvCount == 0 );

	// IoBlock can be static if we use one recv
	// 
	IoBlock* io 	= &m_recvBlock;

	io->op 			= IoBlock::OP_READ;
	io->buf.buf 	= (char*)m_recvBuffer.GetBuffer();
	io->buf.len 	= m_recvBuffer.GetSize();
	io->totalLen 	= io->buf.len;
	io->extra 	 	= this;
	::memset( (void*)&io->remote, 0, sizeof( sockaddr_in ) ); 
	io->remoteLen   = sizeof( sockaddr_in );

	int error = m_socket.AsyncRecvFrom( io );

	if ( error != 0 )
	{
		LOG( FT_ERROR, 
			_T("UdpCommunicator::RequestRecv> Error %d"), 
			error );

		// TODO: report somehow. Don't use OnIoError
		// Then, how?

		return false;
	}

	++m_recvCount;

	LOG( FT_DEBUG_FLOW, _T("UdpCommunicator::RequestRecv>") );

	return true; 
}

void 
UdpCommunicator::OnSendCompleted( IoBlock* /* io */ )
{
	K_ASSERT( !_T("UDP does not use IoService to send") );

	return ;	
}

void 
UdpCommunicator::OnSendCompleted( IoBlock* /* io */, uint /* bytesSent */ )
{
	K_ASSERT( !_T("UDP does not use IoService to send") );

	return;	
}

void 
UdpCommunicator::OnRecvCompleted( IoBlock* io )
{
	ScopedLock sl( m_ioLock );

	K_ASSERT( io == &m_recvBlock );

	LOG( FT_DEBUG_FLOW, _T("UdpCommunicator::OnRecvCompleted>") );

	--m_recvCount;

	K_ASSERT( m_recvCount == 0 );

	BitStream bs;

	bs.Write( io->transferred, io->buf.buf ); // another copy... copy increased...

	if ( m_sl > SECURITY0 )
	{
		m_cipher.Decrypt( bs );
	}

	UdpHeader header;
	::memcpy( &header, bs.GetBuffer(), sizeof( UdpHeader ) );

	UdpConnection* nc = FindByTag( header.srcId );

	if ( header.IsSet( UdpHeader::HPN ) )
	{
		IpAddress peer;

		peer.Init( (SOCKADDR*)&io->remote );

		if ( nc != 0 )
		{
			LOG( FT_DEBUG, 
				_T("UdpCommunicator::OnRecvCompleted> Self[%d] Tag %d Settle Addr %s"), 
				m_selfTag,
				header.seq, 
				peer.ToString().c_str() );

			nc->Settle( peer );	
		}

		RequestRecv();

		return;
	}

	if ( nc != 0 )
	{
		nc->OnRecv( (void*)bs.GetBuffer(), bs.GetBytePosition() );		
	}
	else
	{
		LOG( FT_ERROR, _T("UdpCommunicator::OnRecvCompleted> Self[%d] connection not found"), 
			 m_selfTag );
	}

	RequestRecv();
}

void 
UdpCommunicator::OnIoError( int ec, IoBlock* io )
{
	ScopedLock sl( m_ioLock );

	--m_recvCount;

	K_ASSERT( io == &m_recvBlock );

	LOG( FT_DEBUG_FLOW, 
		_T("UdpCommunicator::OnIoError> S_addr %d Error %d"), 
		io->remote.sin_addr.S_un.S_addr, 
		ec );

	UdpConnection* c = FindByAddress( io->remote.sin_addr.S_un.S_addr );
	
	if ( c == 0 )
	{
		LOG( FT_WARN, 
			_T("UdpCommunicator::OnIoError> %d not found"), 
			io->remote.sin_addr.S_un.S_addr );

		// continue receive
		
		RequestRecv();

		return;
	}

	if ( ec == ERROR_PORT_UNREACHABLE )
	{
		// this can happen in UDP quite open

		RequestRecv();

		return;
	}
		
	c->Close(); // this will close eventually
}
// }

void 
UdpCommunicator::Fini()
{
	m_socket.Close(); // this makes IOCP to return

	m_ioLock.Lock();

	while ( m_recvCount > 0 )
	{
		m_ioLock.Unlock();

		::Sleep( 1 );

		m_ioLock.Lock();
	}

	m_ioLock.Unlock();

	ConnectionMap::iterator i( m_connections.begin() );
	ConnectionMap::iterator iEnd( m_connections.end() );
	
	for ( ; i != iEnd; ++i )
	{
		UdpConnection* c = i->second;

		K_ASSERT( c != 0 );

		c->Fini();

		delete c;
	}
	
	m_connections.clear();
	m_ids.reset();
}

void 
UdpCommunicator::SetLossy( uint tag, uint rate )
{
	UdpConnection* c = FindByTag( tag );

	if ( c == 0 )
	{
		return;
	}

	c->SetLossy( rate );
}

UdpConnection* 
UdpCommunicator::FindByAddress( ulong addrKey )
{
	m_connLock.Lock();

	ConnectionMap::iterator i( m_connections.begin() );
	ConnectionMap::iterator iEnd( m_connections.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpConnection* c = i->second;

		if ( c->GetInAddress().GetKey() == addrKey || 
			 c->GetExAddress().GetKey() == addrKey ||
			 c->GetPeerAddress().GetKey() == addrKey )
		{
			m_connLock.Unlock();

			return c;
		}
	}

	m_connLock.Unlock();

	return (UdpConnection*)0;
}

UdpConnection* 
UdpCommunicator::FindByTag( uint tag )
{
	ScopedLock sl( m_connLock );

	ConnectionMap::iterator i( m_connections.find( tag ) );

	if ( i != m_connections.end() )
	{
		return i->second;
	}

	return (UdpConnection*)0;
}

void 
UdpCommunicator::processConnections()
{
	// [1] check error, then cleanup and report
	processErrorConnections();
	processTickConnections();
}

void
UdpCommunicator::processErrorConnections()
{
	ScopedLock sl( m_connLock );

	if ( m_relay != 0 )
	{
		// When we can use relay, then skip removing error connections 
		
		return;
	}

	ConnectionMap::iterator ri = std::find_if( m_connections.begin(), 
											   m_connections.end(), 
											   fun_is_error_connection() );

	while ( ri != m_connections.end() )
	{
		UdpConnection* c = ri->second;

		K_ASSERT( c != 0 );
		K_ASSERT( c->GetError() > 0 );

		LOG( FT_ERROR, 
			 _T("UdpCommunicator::processErrorConnections> %d"), 
			 c->GetRemoteTag() );

		c->Fini();

		delete c;

		m_connections.erase( ri );
	
		// XXX: Is there a better approach for associative containers?
		ri = std::find_if( m_connections.begin(), 
						   m_connections.end(), 
						   fun_is_error_connection() );
	}
}

void 
UdpCommunicator::processTickConnections()
{
	ScopedLock sl( m_connLock );

	ConnectionMap::iterator ci( m_connections.begin() );	
	ConnectionMap::iterator ciEnd( m_connections.end() );	

	for ( ; ci != ciEnd; ++ci )
	{
		UdpConnection* c = ci->second;

		K_ASSERT( c != 0 );
		K_ASSERT( c->GetError() == 0 || m_relay != 0 );

		c->Run();
	}
}

void
UdpCommunicator::send( UdpConnection* c, MessagePtr m, int qos )
{
	if ( c == 0 )
	{
		LOG( FT_WARN, _T("UdpCommunicator::send> UDP connection is null") );
		
		return;
	}

	BitStream bs;

	bool rc = m->Pack( bs );

	if ( !rc )
	{
		LOG( FT_WARN, 
			 _T("UdpCommunicator::send> %d pack error"), 
			 m->type );

		return;
	}

	if ( !c->IsOpen() )
	{
		if ( m_relay != 0 )
		{
			NmGroupRelay* r = new NmGroupRelay;

			r->remote = m_relay->GetId();
			r->relays.push_back( c->GetRemoteTag() );

			K_ASSERT( bs.GetBytePosition() <= 512 );

			::memcpy( r->data, bs.GetBuffer(), bs.GetBytePosition() );

			r->len = bs.GetBytePosition();

			m_relay->Send( MessagePtr( r ) ); 

			LOG( FT_DEBUG, 
				 _T("UdpCommunicator::send> Relay message %d"), 
				 m->type );

			return;
		}

		LOG( FT_WARN, 
			 _T("UdpCommunicator::send> Sending before open %d"), 
			 c->GetRemoteTag() );
	}

	switch ( qos )
	{
	case Message::RELIABLE:
		{
			c->SendReliable( bs.GetBuffer(), bs.GetBytePosition() );
		}
		break;
	case Message::ORDERED:
		{
			c->SendOrdered( bs.GetBuffer(), bs.GetBytePosition() );
		}
		break;
	default:
		{
			K_ASSERT( qos == Message::LOSSY );

			c->SendLossy( bs.GetBuffer(), bs.GetBytePosition() );
		}
		break;
	}
}

void
UdpCommunicator::multicast( const Message::RemoteList& tags, MessagePtr m, int qos )
{
	typedef std::vector<uint> RelayList;

	BitStream bs;

	bool rc = m->Pack( bs );

	if ( !rc )
	{
		LOG( FT_WARN, 
			 _T("UdpCommunicator::multicast> %d pack error"), 
			 m->type );

		return;
	}

	RelayList relayList;

	Message::RemoteList::const_iterator i( tags.begin() );
	Message::RemoteList::const_iterator iEnd( tags.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpConnection* c = FindByTag( *i );

		if ( c == 0 )
		{
			LOG( FT_WARN, _T("UdpCommunicator::multicast> UDP connection tag[%d] is null"), *i );

			continue;
		}

		if ( !c->IsOpen() )
		{
			relayList.push_back( c->GetRemoteTag() );

			continue;
		}

		switch ( qos )
		{
		case Message::RELIABLE:
			{
				c->SendReliable( bs.GetBuffer(), bs.GetBytePosition() );
			}
			break;
		case Message::ORDERED:
			{
				c->SendOrdered( bs.GetBuffer(), bs.GetBytePosition() );
			}
			break;
		default:
			{
				K_ASSERT( qos == Message::LOSSY );

				c->SendLossy( bs.GetBuffer(), bs.GetBytePosition() );
			}
			break;
		}
	}

	if ( !relayList.empty() )
	{
		if ( m_relay != 0 )
		{
			NmGroupRelay* r = new NmGroupRelay;

			r->remote = m_relay->GetId();
			r->relays = relayList;

			K_ASSERT( bs.GetBytePosition() <= 512 );

			::memcpy( r->data, bs.GetBuffer(), bs.GetBytePosition() );

			r->len = bs.GetBytePosition();

			m_relay->Send( MessagePtr( r ) ); 
		}
	}
}

} // gk
