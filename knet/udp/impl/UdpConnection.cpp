#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/udp/impl/UdpConnection.h>

#include <knet/message/net/NetStateMessage.h>
#include <knet/udp/UdpCommunicator.h>
#include <knet/udp/impl/Reliable.h>
#include <knet/tcp/impl/TcpConnection.h>
#include <kcore/sys/ScopedLock.h>
#include <kcore/sys/Logger.h>
#include <kcore/util/Random.h>

namespace gk {

UdpConnection::UdpConnection()
: m_state( INITIAL )
, m_error( ERROR_NONE )
, m_selfTag( 0 )
, m_remoteTag( 0 )
, m_peer()
, m_in()
, m_ex()
, m_settled( false )
, m_recvBlock1( 2048 )
, m_recvBlock2( 2048 )
, m_readBlock( 0 )
, m_accBlock( 0 )
, m_recvLock()
, m_blockCount( 0 )
, m_lossRate( 0 )
, m_communicator( 0 )
, m_reliable( 0 )
{

}

UdpConnection::~UdpConnection()
{
	K_ASSERT( m_reliable 	== 0 );
}

bool
UdpConnection::Init( UdpCommunicator* communicator, uint selfTag, uint remoteTag, const IpAddress& in, const IpAddress& ex )
{
	K_ASSERT( m_state == INITIAL );

	K_ASSERT( communicator != 0 );
	K_ASSERT( selfTag > 0 );
	K_ASSERT( remoteTag > 0 );
	K_ASSERT( selfTag != remoteTag );

	m_state 			= INITIAL;
	m_communicator  	= communicator;
	m_blockCount 		= 0;
	m_lossRate 			= 0;

	m_recvBlock1.Reset();
	m_recvBlock2.Reset();

	m_readBlock = &m_recvBlock1;
	m_accBlock = &m_recvBlock2;

	m_settled = false;

	m_reliable 	= new Reliable;
	(void)m_reliable->Init( communicator, this );


	m_selfTag 	= selfTag;
	m_remoteTag = remoteTag;
	m_in 	= in;
	m_ex 	= ex;

	sendSyn();

	m_state = SYN_SENT;

	m_tickHpnResend.Reset();
	m_tickSynResend.Reset();
	m_tickOpenTimeout.Reset();

	m_settled = false;

	LOG( FT_DEBUG, 
		_T("UdpConnection::Connect> self[%d] remote[%d] in[%s] ex[%s]"), 
		m_selfTag, m_remoteTag, in.ToString().c_str(), ex.ToString().c_str() );

	return true;
}

void 
UdpConnection::Settle( const IpAddress& addr )
{
	if ( m_settled )
	{
		K_ASSERT( addr == m_peer );

		return;
	}

	m_peer 		= addr;
	m_settled   = true;

	LOG( FT_DEBUG, 
		 _T("UdpConnection::Settle> remote[%d] self[%d] IP %s"), 
		 m_remoteTag, m_selfTag, m_peer.ToString().c_str() );
}

void 
UdpConnection::OnRecv( void* data, uint len )
{
	// NOTE: data is owned by Communicator
	
	K_ASSERT( data != 0 );
	K_ASSERT( len >= sizeof( UdpHeader ) );
	K_ASSERT( len <= MAX_SEGMENT_SIZE );

	// lock required for buffer 
	ScopedLock sl( m_recvLock );			// this only lock for UDP

	m_accBlock->WriteInt( len, 16 );		// must be byte aligned
	m_accBlock->Write( len, data );

	++m_blockCount;

	// Udp cannot process messages here 
	// since lock propagation is very deep 
	// down to Reliable and UdpCommunicator

	LOG( FT_DEBUG_FLOW, _T("UdpConnection::OnRecv> remote[%d] self[%d] %d bytes"), 
		 m_remoteTag, m_selfTag, len );
}

bool
UdpConnection::SendReliable( void* data, uint len )
{
	// NOTE: data is owned by Communicator
	
	K_ASSERT( m_state == OPEN );

	if ( m_state != OPEN )
	{
		return false;
	}

	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );
	K_ASSERT( len < MAX_SEGMENT_SIZE );

	return m_reliable->Send( data, len, false );
}

bool
UdpConnection::SendOrdered( void* data, uint len )
{
	// NOTE: data is owned by Communicator
	
	K_ASSERT( m_state == OPEN );

	if ( m_state != OPEN )
	{
		return false;
	}

	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );
	K_ASSERT( len < MAX_SEGMENT_SIZE );

	return m_reliable->Send( data, len, true );
}

bool
UdpConnection::SendLossy( void* data, uint len )
{
	// NOTE: data is owned by Communicator
	
	K_ASSERT( m_state == OPEN );

	if ( m_state != OPEN )
	{
		return false;
	}

	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );
	K_ASSERT( len <= MAX_SEGMENT_SIZE );

	UdpHeader header;

	header.Set( UdpHeader::ACK );
	header.seq 		= 0;
	header.ack 		= m_reliable->GetSendCumAck();
	header.length 	= sizeof( UdpHeader );
	header.bodyLen  = len;
	byte* sdata 	= (byte*)g_allocator.Alloc( sizeof( UdpHeader ) + len );
	uint slen 		= sizeof( UdpHeader ) + len;

	::memcpy( sdata, (void*)&header, sizeof( UdpHeader ) );
	::memcpy( (void*)(sdata + header.length), data, len );

	return SendRaw( sdata, slen );
}

bool
UdpConnection::SendRaw( void* data, uint len )
{
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );
	K_ASSERT( len <= MAX_SEGMENT_SIZE );

	if ( m_lossRate > 0 )
	{
		uint rand = (uint)(Random::Rand() % 100);

		if ( rand <= m_lossRate )
		{
			LOG( FT_DEBUG_FLOW, 
				 _T("UdpConnection::SendRaw> remote[%d] self[%d] lost by rate %d"), 
				 m_remoteTag,
				 m_selfTag, 
				 m_lossRate );

			return true;
		}
	}

	int slen = 0;
	
	if ( m_settled )
	{
		m_communicator->SendTo( (byte*)data, len, m_peer );
	}
	else
	{
		m_communicator->SendTo( (byte*)data, len, m_in );

		if ( m_in != m_ex )
		{
			m_communicator->SendTo( (byte*)data, len, m_ex );
		}
	}

	return slen > 0;
}

void 
UdpConnection::Run()
{
	processRecvBlocks();

	switch ( m_state )
	{
	case CLOSED:
		// nothing to do
		break;
	case SYN_SENT:
		{
			processHpn();
			processSyn();
			processTimeout();
		}
		break;
	case SYN_RCVD:
		{
			processHpn();
			processSyn();
			processAck();
			processTimeout();
		}
		break;
	case OPEN:
		{
			processReliable();
			processKeepAlive();
		}
		break;
	case CLOSE_WAIT:
		{
			processKeepAlive();
		}
		break;
	}
}

void 
UdpConnection::Close()
{
	sendRst();

	m_state = CLOSE_WAIT;
}

void 
UdpConnection::Fini()
{
	Close();

	if ( m_reliable != 0 )
	{
 		m_reliable->Fini();

		delete m_reliable;

		m_reliable = 0;
	}
}

void 
UdpConnection::onSynRcvd()
{
	// UdpConnection: send SYN in SYN_SENT, SYN_RCVD
	//             if ACK recieved, then UdpConnection is open 
	
	LOG( FT_DEBUG_FLOW, 
		 _T("UdpConnection::onSynRcvd> self[%d] -> remote[%d] State %d syn"), 
		 m_selfTag,
		 m_remoteTag, 
		 m_state );

	switch ( m_state )
	{
	case CLOSED:
		{
			m_state = SYN_RCVD;
			sendSyn();
		}
		break;
	case SYN_SENT:
		{
			m_state = SYN_RCVD;
		}
		break;
	case SYN_RCVD:
		{
			sendSyn();
			sendAck();
		}
		break;
	case OPEN:
	case CLOSE_WAIT:
		break;
	}
}

void 
UdpConnection::onReset()
{
	if ( m_state != CLOSE_WAIT )
	{
		m_state = CLOSE_WAIT;

		m_tickCloseWait.Reset();

		LOG( FT_DEBUG_FLOW, 
		 	 _T("UdpConnection::onReset> self[%d] -> remote[%d] syn"), 
		 	 m_selfTag,
		 	 m_remoteTag );
	}
}

void 
UdpConnection::onNulRcvd()
{
	LOG( FT_DEBUG_FLOW, 
		 _T("UdpConnection::onNulRecvd> self[%d] <- remote[%d]"), 
		 m_selfTag,
		 m_remoteTag );

	sendAck(); // Needs to send something
}

void 
UdpConnection::onAckRcvd()
{
	// UdpConnection: send SYN in SYN_SENT, SYN_RCVD
	//             if ACK recieved, then UdpConnection is open 
	//             

	LOG( FT_DEBUG_FLOW, 
		_T("UdpConnection::onAckRecvd> State %d self[%d] <- remote[%d]"), 
		m_state, 
		m_selfTag, 
		m_remoteTag );

	switch ( m_state )
	{
	case INITIAL:
	case SYN_SENT:
		{
			sendAck();

			m_state = OPEN;

			LOG( FT_INFO, 
				 _T("UdpConnection::onAckRcvd> self[%d] remote[%d] open from SYN_SENT"), 
				 m_selfTag, m_remoteTag );

			m_tickKeepAlive.Reset();
			m_tickInactive.Reset();

			notifyOpen();
		}
		break;
	case SYN_RCVD:
		{
			sendAck();

			m_state = OPEN;

			LOG( FT_INFO, 
				 _T("UdpConnection::onAckRcvd> self[%d] remote[%d] open from SYN_RCVD"), 
				 m_selfTag, m_remoteTag );

			m_tickKeepAlive.Reset();
			m_tickInactive.Reset();

			notifyOpen();
		}
		break;
	case OPEN:
	case CLOSED:
	case CLOSE_WAIT:
		break;
	}
}

void 
UdpConnection::onUnreliableRecv( const UdpHeader& header, void* data, uint len )
{
	byte* p = (byte*)data;

	p = (byte*)( p + header.length );

	len = len - header.length;

	m_communicator->OnRecv( m_remoteTag, (void*)p, len ); // pass the received data
}

void 
UdpConnection::processRecvBlocks()
{
	uint len = 0;

	Buffer buf( MAX_SEGMENT_SIZE );


	int blockCount = 0;

	{
		ScopedLock sl( m_recvLock );

		if ( m_blockCount == 0 )
		{
			return;
		}

		blockCount   = m_blockCount;
		m_blockCount = 0;

		// buffer switch 
		BitStream* p  = m_readBlock;
		m_readBlock  = m_accBlock;
		m_accBlock = p;
		
		m_readBlock->Reset(); // read from beginning
	}

	while ( blockCount > 0 )
	{
		buf.Clear();

		m_readBlock->ReadInt( len, 16 );
		K_ASSERT( len > 0 );
		K_ASSERT( len <= MAX_SEGMENT_SIZE );

		m_readBlock->Read( len, (void*)buf.GetBuffer() );

		processBlock( buf.GetBuffer(), len );

		--blockCount;
	}

	K_ASSERT( blockCount == 0 );

	m_readBlock->Reset();
	m_readBlock->Clear(); // clear content and error
}

void 
UdpConnection::processBlock( void* data, uint len )
{
	K_ASSERT( data != 0 );
	K_ASSERT( len >= sizeof( UdpHeader ) );

	m_tickKeepAlive.Reset();
	m_tickInactive.Reset();

	UdpHeader header;	

	::memcpy( (void*)&header, data, sizeof( UdpHeader ) );

	LOG( FT_DEBUG_FLOW, _T("UdpConnection::processBlock> self[%d] remote[%d] len %d body %d"), 
		 m_selfTag, m_remoteTag, len, header.bodyLen );

	if ( header.IsSet( UdpHeader::SYN ) )
	{
		K_ASSERT( header.srcId > 0 );

		m_selfTag = header.dstId; 
		onSynRcvd();	

		return;
	}

	if ( header.IsSet( UdpHeader::RST ) )
	{
		onReset();

		return;
	}

	if ( header.IsSet( UdpHeader::NUL ) )
	{
		onNulRcvd();
	}

	if ( header.IsSet( UdpHeader::ACK ) )
	{
		if ( m_state == OPEN && header.ack > 0 )
		{
			m_reliable->OnAck( header );
		}
		else
		{
			onAckRcvd();
		}
	}

	if ( m_state != OPEN )
	{
		return;
	}

	// only continue when connected

	if ( header.IsSet( UdpHeader::EAK ) )
	{
		m_reliable->OnEak( header, data, len );

		// EAK is processed only for EAK

		return;
	}

	if ( header.bodyLen == 0 )
	{
		return;
	}

	if ( header.IsSet( UdpHeader::RLE ) )
	{
		m_reliable->OnRecv( header, data, len );
	}
	else
	{
		onUnreliableRecv( header, data, len );
	}
}

void 
UdpConnection::processHpn()
{
	if ( m_state == SYN_SENT || m_state == SYN_RCVD )
	{
		if ( m_tickHpnResend.Elapsed() > HPN_RESEND_INTERVAL )
		{
			sendHpn();

			LOG( FT_DEBUG_FLOW, 
				 _T("UdpConnection::processHpn> self[%d] -> remote[%d] hpn sent"), 
				 m_selfTag, m_remoteTag );

			m_tickHpnResend.Reset();
		}
	}
}

void 
UdpConnection::processSyn()
{
	if ( m_state == SYN_SENT || m_state == SYN_RCVD )
	{
		if ( m_tickSynResend.Elapsed() > SYN_RESEND_INTERVAL )
		{
			sendSyn();

			LOG( FT_DEBUG_FLOW, 
				 _T("UdpConnection::processSyn> self[%d] -> remote[%d] syn sent"), 
				 m_selfTag,
				 m_remoteTag );

			m_tickSynResend.Reset();
		}
	}
}

void 
UdpConnection::processAck()
{
	if ( m_state == SYN_RCVD )
	{
		if ( m_tickAckResend.Elapsed() > ACK_RESEND_INTERVAL )
		{
			sendAck();

			m_tickAckResend.Reset();
		}
	}
}

void 
UdpConnection::processTimeout()
{
	if ( m_state == SYN_SENT || m_state == SYN_RCVD )
	{
		if ( m_tickOpenTimeout.Elapsed() > OPEN_TIMEOUT )
		{
			m_error = ERROR_UDP_OPEN_TIMEOUT;

			LOG( FT_DEBUG_FLOW, 
			     _T("UdpConnection::processTimeout> self[%d] remote[%d] timed out"), 
				 m_selfTag, m_remoteTag );

			notifyOpenTimeout();

			m_tickOpenTimeout.Reset();
		}
	}
}

void 
UdpConnection::processReliable()
{
	m_reliable->Run();
}

void 
UdpConnection::processKeepAlive()
{
	if ( m_state == OPEN )
	{
		if ( m_tickKeepAlive.Elapsed() > KEEP_ALIVE_TIMEOUT )
		{
			sendNul();

			m_tickKeepAlive.Reset();

			LOG( FT_DEBUG_FLOW, 
				 _T("UdpConnection::processKeepAlive> self[%d] remote[%d] Nul"), 
				 m_selfTag, 
				 m_remoteTag );
		}

		if ( m_tickInactive.Elapsed() > CONNECTION_TIMEOUT )
		{
			sendRst();

			m_state = CLOSE_WAIT;

			m_tickCloseWait.Reset();

			LOG( FT_DEBUG_FLOW, 
				 _T("UdpConnection::processKeepAlive> self[%d] remote[%d] Rst"), 
				 m_selfTag, 
				 m_remoteTag );
		}
	}

	if ( m_state == CLOSE_WAIT )
	{
		if ( m_tickCloseWait.Elapsed() > CLOSE_WAIT_TIMEOUT )
		{
			m_error = ERROR_UDP_CLOSED;

			m_tickCloseWait.Reset(); // 

			m_state = CLOSED;

			notifyClosed();

			LOG( FT_INFO, 
				 _T("UdpConnection::processKeepAlive> self[%d] remote[%d] Closed"), 
				 m_selfTag, m_remoteTag );
		}
	}
}

void 
UdpConnection::sendHpn()
{
	UdpHeader header;

	header.control  = 0;
	header.length 	= sizeof( UdpHeader );
	header.srcId  	= GetSelfTag();
	header.dstId  	= GetRemoteTag();
	header.seq 		= m_selfTag;
	header.ack 		= 0;

	header.Set( UdpHeader::HPN );

	SendRaw( (void*)&header, sizeof( UdpHeader ) );
}

void 
UdpConnection::sendSyn()
{
	UdpHeader header;

	header.control  = 0;
	header.length 	= sizeof( UdpHeader );
	header.srcId  	= GetSelfTag();
	header.dstId  	= GetRemoteTag();
	header.seq 		= m_selfTag;
	header.ack 		= 0;

	header.Set( UdpHeader::SYN );

	SendRaw( (void*)&header, sizeof( UdpHeader ) );
}

void 
UdpConnection::sendAck()
{
	UdpHeader header;

	header.control  = 0;
	header.length 	= sizeof( header );
	header.srcId  	= GetSelfTag();
	header.dstId  	= GetRemoteTag();
	header.seq 		= 0;
	header.ack 		= m_reliable->GetSendCumAck();

	header.Set( UdpHeader::ACK );

	SendRaw( (void*)&header, sizeof( header ) );
}

void 
UdpConnection::sendNul()
{
	UdpHeader header;

	header.control  = 0;
	header.length 	= sizeof( header );
	header.srcId  	= GetSelfTag();
	header.dstId  	= GetRemoteTag();
	header.seq 		= 0;
	header.ack 		= m_reliable->GetSendCumAck();

	header.Set( UdpHeader::NUL );

	SendRaw( (void*)&header, sizeof( header ) );
}

void 
UdpConnection::sendRst()
{
	UdpHeader header;

	header.control  = 0;
	header.length 	= sizeof( header );
	header.srcId  	= GetSelfTag();
	header.dstId  	= GetRemoteTag();
	header.seq 		= 0;
	header.ack 		= 0;

	header.Set( UdpHeader::RST );

	SendRaw( (void*)&header, sizeof( header ) );
}

void 
UdpConnection::notifyOpen()
{
	NetStateMessage* m = new NetStateMessage;

	m->state 		= NetStateMessage::UDP_OPENED;
	m->connectionId = m_remoteTag;
	m->addr 		= m_peer;	

	m_communicator->Notify( MessagePtr( m ) );
}

void 
UdpConnection::notifyOpenTimeout()
{
	NetStateMessage* m = new NetStateMessage;

	m->state		= NetStateMessage::UDP_OPEN_TIMEOUT;
	m->connectionId = m_remoteTag;
	m->addr 		= m_peer;	

	m_communicator->Notify( MessagePtr( m ) );
}

void 
UdpConnection::notifyClosed()
{
	NetStateMessage* m = new NetStateMessage;

	m->state		= NetStateMessage::UDP_CLOSED;
	m->connectionId = m_remoteTag;
	m->addr 		= m_peer;

	m_communicator->Notify( MessagePtr( m ) );
}

} // gk
