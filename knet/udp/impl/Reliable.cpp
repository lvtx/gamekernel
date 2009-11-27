#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/udp/impl/Reliable.h>

#include <kcore/sys/Logger.h>
#include <kcore/sys/ScopedLock.h>
#include <knet/udp/UdpCommunicator.h>
#include <knet/udp/impl/UdpConnection.h>

namespace gk {

struct UdpEakHeader 
{
	UdpHeader header;
	byte 	  eakCount;
	int 	  eak;
};

Reliable::Reliable()
: m_communicator( 0 )
, m_connection( 0 )
, m_sendList()
, m_recvList()
, m_sendSeq( 0 )
, m_recvCumAck( 0 )
, m_sendCumAck( 0 )
, m_outstandingSegs( 0 )
, m_zeroSendCount( 0 )
, m_averageRtt( START_RTT )
, m_cumAckInterval( CUMULATIVE_ACK_INTERVAL )
, m_tickCumulativeAck() 
, m_tickResend()
{
}

Reliable::~Reliable()
{
}

bool 
Reliable::Init( UdpCommunicator* communicator, UdpConnection* connection )
{
	K_ASSERT( communicator != 0 );
	K_ASSERT( connection != 0 );

	m_communicator 	= communicator;
	m_connection 	= connection;

	m_sendList.clear();
	m_recvList.clear();

	m_sendSeq 			= 0;
	m_recvCumAck 		= 0; 	// suppose seq starts from 1 always
	m_sendCumAck 		= 0; 	// suppose seq starts from 1 always
	m_outstandingSegs 	= 0;
	m_zeroSendCount 	= 0;
	m_averageRtt 		= START_RTT;

	m_cumAckInterval	= CUMULATIVE_ACK_INTERVAL;

	m_tickCumulativeAck.Reset();
	m_tickResend.Reset();

	return true;
}

void 
Reliable::Run()
{
	processZeroSend();
	processResend();
	processCumAck();
}

void 
Reliable::OnAck( const UdpHeader& header )
{
	K_ASSERT( header.ack > 0 );

	if ( m_recvCumAck >= header.ack )
	{
		LOG( FT_DEBUG, 
			 _T("Reliable::OnAck> self[%d] remote[%d] duplicate cum ack %d"), 
			 m_connection->GetSelfTag(), 
			 m_connection->GetRemoteTag(),
			 header.ack );

		return;
	}

	m_recvCumAck = header.ack;

	LOG( FT_DEBUG, 
		 _T("Reliable::OnAck> ack %d"), 
		 header.ack );

	SendBlockList::iterator i( m_sendList.begin() );

	int totalRtt = 0;
	int totalCount = 0;

	for ( ; i != m_sendList.end(); ) 
	{
		UdpSendBlock* sb = *i;
		K_ASSERT( sb != 0 );

		if ( sb->header.seq > header.ack )
		{
			break;
		}

		totalRtt += sb->rxmt.Elapsed(); 
		++totalCount;

		i = m_sendList.erase( i++ );

		--m_outstandingSegs;

		K_ASSERT( m_outstandingSegs >= 0 );

		delete sb;
	}

	if ( totalCount > 0 )
	{
		// moving average. slowly converges.
		m_averageRtt = ( totalRtt / totalCount + m_averageRtt ) / 2;
	}
}

void 
Reliable::OnEak( const UdpHeader& header, void* data, uint len )
{
	// only 1 EAK under current protocol 
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );

	byte* p = (byte*)data;

	byte* head = (byte*)(p + header.length);

	byte eakCount = *head;

	int* eakHead = (int*)((byte*)( head + 1 ));

	for ( byte i=0; i<eakCount; ++i )
	{
		int eak = eakHead[i];	

		runEak( eak );
	}
}

void 
Reliable::OnRecv( const UdpHeader& header, void* data, uint len )
{
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );
	K_ASSERT( header.length < len );
	K_ASSERT( header.IsSet( UdpHeader::RLE) );

	if ( header.seq <= m_sendCumAck )
	{
		LOG( FT_DEBUG, 
			 _T("Reliable::OnRecv> self[%d] remote[%d] dup %d checked by m_sendCumAck %d"), 
			 m_connection->GetSelfTag(), 
			 m_connection->GetRemoteTag(),
			 header.seq, 
			 m_sendCumAck );

		return; 
	}

	byte* p	   = (byte*)data;
	byte* head = (byte*)(p + header.length);

	// save payload only
	data = (void*)head;
	len = len - header.length;

	LOG( FT_DEBUG, _T("Reliable::OnRecv> self[%d] remote[%d] seq %d"), 
		 m_connection->GetSelfTag(), 
		 m_connection->GetRemoteTag(),
		 header.seq );

	// if in sequence
	if ( header.seq == m_sendCumAck + 1 )
	{
		LOG( FT_DEBUG, _T("Reliable::OnRecv> In order %d"), header.seq );

		m_communicator->OnRecv( m_connection->GetRemoteTag(), data, len );

		++m_sendCumAck;

		clearIfExists( m_sendCumAck );

		K_ASSERT( header.seq == m_sendCumAck );

		m_cumAckInterval = CUMULATIVE_ACK_INTERVAL;
	}
	else
	{
		LOG( FT_DEBUG, _T("Reliable::OnRecv>  Out of order %d"), header.seq );

		if ( isDuplicateRecv( header.seq ) )
		{
			LOG( FT_DEBUG, _T("Reliable::OnRecv> Dup %d check by search"), header.seq );

			return;
		}

		UdpRecvBlock* rv 	= allocRecvBlock();
		rv->ordered 	= false;
		rv->seq 	 	= header.seq;
		rv->ackCount 	= 0;

		if ( header.IsSet( UdpHeader::ORD ) )
		{
			rv->ordered = true;
			rv->data 	= allocData( len );
			rv->len  	= len;

			::memcpy( rv->data, data, len );
		}
		else
		{
			m_communicator->OnRecv( m_connection->GetRemoteTag(), data, len );
		}

		K_ASSERT( rv->seq > m_sendCumAck );

		insertRecvBlock( rv );
		runRecvBlockList();

		// only 1 eak is used.
		// send immediately when not in order
		sendEak( header.seq );

		m_tickCumulativeAck.Reset();
	}
}

bool 
Reliable::Send( void* data, uint len, bool ordered )
{
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );

	processZeroSend(); 		// To send previously asked ones first.

	UdpSendBlock* sb = allocSendBlock();

	sb->header.Set( UdpHeader::RLE );
	sb->header.Set( UdpHeader::ACK );

	if ( ordered )
	{
		sb->header.Set( UdpHeader::ORD );
	}

	sb->header.seq = ++m_sendSeq;
	sb->header.ack = m_sendCumAck;

	sb->header.srcId 	= m_connection->GetSelfTag();
	sb->header.dstId 	= m_connection->GetRemoteTag();
	sb->header.length   = sizeof( UdpHeader );
	sb->header.bodyLen	= len;
	sb->data 			= (byte*)allocData( sizeof( UdpHeader ) + len );

	::memcpy( sb->data, (void*)&sb->header, sizeof( UdpHeader ) );
	::memcpy( (void*)(sb->data + sb->header.length), data, len );

	sb->len 			= sizeof( UdpHeader ) + len;

	if ( m_outstandingSegs < MAX_OUTSTANDING_SEGMENTS )
	{
		m_connection->SendRaw( sb->data, sb->len );

		++sb->rxCount;

		++m_outstandingSegs;
		m_tickCumulativeAck.Reset();
	}
	else
	{
		++m_zeroSendCount;
	}

	m_sendList.push_back( sb );

	return true;
}

void 
Reliable::Fini()
{
	freeSendBlocks();
	freeRecvBlocks();
}

void
Reliable::processZeroSend()
{
	if ( m_zeroSendCount == 0 && 
		 m_outstandingSegs >= MAX_OUTSTANDING_SEGMENTS )
	{
		return;
	}

	SendBlockList::iterator i( m_sendList.begin() );

	for ( ; i != m_sendList.end(); ++i )
	{
		UdpSendBlock* sb = *i;
		K_ASSERT( sb != 0 );

		if ( sb->rxCount == 0 )
		{
			if ( resend( sb ) )
			{
				--m_zeroSendCount;

				K_ASSERT( m_zeroSendCount >= 0 );
			}
			else
			{
				return; // we still cannot send 'cause there are too many outstanding segments
			}
		}
	}
}

void 
Reliable::processResend()
{
	uint rtt = m_averageRtt;

	int resendCount = 0;

	if ( m_tickResend.Elapsed() > RESEND_CHECK_INTERVAL )
	{
		SendBlockList::iterator i( m_sendList.begin() );
		SendBlockList::iterator iEnd( m_sendList.end() );

		for ( ; i != iEnd; ++i )
		{
			UdpSendBlock* sb = *i;
			K_ASSERT( sb != 0 );

			if ( sb->rxmt.Elapsed() > rtt + rtt * sb->rxCount )
			{
				resend( sb, true );

				++resendCount;
			}
		}

		m_tickResend.Reset();
	}
}

void 
Reliable::processCumAck()
{
	if ( m_tickCumulativeAck.Elapsed() > m_cumAckInterval )
	{
		sendCumAck();

		m_cumAckInterval *= 2;

		m_tickCumulativeAck.Reset();
	}
}

void 
Reliable::runEak( int seq )
{
	SendBlockList::iterator i( m_sendList.begin() );
	SendBlockList::iterator iEnd( m_sendList.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpSendBlock* sb = *i;
		K_ASSERT( sb != 0 );

		if ( sb->header.seq == seq )
		{
			LOG( FT_DEBUG, _T("Reliable::runEak> self[%d] remote[%d] %d acked") , 
		 		 m_connection->GetSelfTag(), 
		 		 m_connection->GetRemoteTag(),
				 seq );

			m_sendList.erase( i );

			delete sb;

			return;
		}
	}

	i    = m_sendList.begin();
	iEnd = m_sendList.end();

	for ( ; i != iEnd; ++i )
	{
		UdpSendBlock* sb = *i;

		if ( sb->header.seq > seq )
		{
			return;
		}

		resend( sb );
	}
}

bool 
Reliable::isDuplicateRecv( int seq )
{
	RecvBlockList::iterator i( m_recvList.begin() );
	RecvBlockList::iterator iEnd( m_recvList.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpRecvBlock* rb = *i;
		K_ASSERT( rb != 0 );

		if ( rb->seq == seq )
		{
			return true;
		}
	}

	return false;
}

void 
Reliable::insertRecvBlock( UdpRecvBlock* rv )
{
	K_ASSERT( rv != 0 );
	K_ASSERT( !isDuplicateRecv( rv->seq ) );

	RecvBlockList::iterator i( m_recvList.begin() );
	RecvBlockList::iterator iEnd( m_recvList.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpRecvBlock* rb = *i;

		if ( rb->seq > rv->seq )
		{
			m_recvList.insert( i, rv );

			return;
		}
	}

	m_recvList.push_back( rv );
}

void 
Reliable::runRecvBlockList()
{
	RecvBlockList::iterator i( m_recvList.begin() );

	for ( ; i != m_recvList.end(); )
	{
		UdpRecvBlock* p = *i;

		K_ASSERT( p->seq > m_sendCumAck );

		if ( p->seq == ( m_sendCumAck + 1) ) // in order 
		{
			if ( p->ordered ) // ordered QoS
			{
				K_ASSERT( p->data != 0 );
				K_ASSERT( p->len > 0 );

				// deliver and free
				LOG( FT_DEBUG, 
				 	 _T("Reliable::runRecvBlockList> self[%d] remote[%d] seq %d made available in order"), 
		 		 	 m_connection->GetSelfTag(), 
		 		 	 m_connection->GetRemoteTag(),
					 p->seq );
				
				m_communicator->OnRecv( m_connection->GetRemoteTag(), p->data, p->len );

				freeData( p->data, p->len );
			}

			++m_sendCumAck;

			K_ASSERT( p->seq == m_sendCumAck );

			i = m_recvList.erase( i++ );

			freeRecvBlock( p );
		}
		else
		{
			LOG( FT_DEBUG, 
				 _T("Reliable>runRecvBlockList> Not in order %d, Cum %d, waiting..."), 
				 p->seq,
				 m_sendCumAck );

			return;
		}
	}
}

void 
Reliable::clearIfExists( int seq )
{
	RecvBlockList::iterator i( m_recvList.begin() );
	RecvBlockList::iterator iEnd( m_recvList.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpRecvBlock* rb = *i;

		if ( rb->seq == seq )
		{
			freeRecvBlock( rb );

			m_recvList.erase( i );

			return;
		}
	}
}

bool
Reliable::resend( UdpSendBlock* sb, bool forced )
{
	if ( !forced )
	{
		if ( m_outstandingSegs >= MAX_OUTSTANDING_SEGMENTS )
		{
			return false;
		}
	}

	sb->header.ack = m_sendCumAck;
					
	::memcpy( sb->data, (void*)&sb->header, sizeof( UdpHeader ) );

	m_connection->SendRaw( sb->data, sb->len );

	m_tickCumulativeAck.Reset();
	++m_outstandingSegs;

	++sb->rxCount;
	sb->rxmt.Reset();

	if ( sb->rxCount > 1 )
	{
		LOG( FT_DEBUG, 
			 _T("Reliable::resend> self[%d] remote[%d] %d"), 
		 	 m_connection->GetSelfTag(), 
		 	 m_connection->GetRemoteTag(),
		 	 sb->header.seq );
	}

	return true;
}

void 
Reliable::sendEak( uint seq )
{
	UdpEakHeader eh;

	eh.header.control  = 0;
	eh.header.length 	= sizeof( eh );
	eh.header.srcId  	= m_connection->GetSelfTag();
	eh.header.dstId  	= m_connection->GetRemoteTag();
	eh.header.seq 		= 0;
	eh.header.ack 		= 0;
	eh.eakCount 		= 1;
	eh.eak 				= seq;

	eh.header.Set( UdpHeader::EAK );

	m_connection->SendRaw( (void*)&eh, sizeof( eh ) );
}

void 
Reliable::sendCumAck()
{
	UdpHeader header;

	header.control  = 0;
	header.length 	= sizeof( header );
	header.srcId  	= m_connection->GetSelfTag();
	header.dstId  	= m_connection->GetRemoteTag();
	header.seq 		= 0;
	header.ack 		= m_sendCumAck;

	header.Set( UdpHeader::ACK );

	m_connection->SendRaw( (void*)&header, sizeof( header ) );
}

UdpSendBlock* 
Reliable::allocSendBlock()
{
	return new UdpSendBlock();
}

void 
Reliable::freeSendBlock( UdpSendBlock* p )
{
	K_ASSERT( p != 0 );

	delete p;
}

UdpRecvBlock* 
Reliable::allocRecvBlock()
{
	return new UdpRecvBlock();
}

void 
Reliable::freeRecvBlock( UdpRecvBlock* p )
{
	K_ASSERT( p != 0 );
			  
	delete p;
}

void* 
Reliable::allocData( uint len )
{
	return g_allocator.Alloc( len );
}

void 
Reliable::freeData( void* data, uint /* len */ )
{
	g_allocator.Free( data );
}

void 
Reliable::freeSendBlocks()
{
	SendBlockList::iterator i( m_sendList.begin() );
	SendBlockList::iterator iEnd( m_sendList.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpSendBlock* sb = *i;

		K_ASSERT( sb->data != 0 );
		K_ASSERT( sb->len > 0 );
		
		freeData( sb->data, sb->len );
		freeSendBlock( sb );
	}

	m_sendList.clear();
}

void 
Reliable::freeRecvBlocks()
{
	RecvBlockList::iterator i( m_recvList.begin() );
	RecvBlockList::iterator iEnd( m_recvList.end() );

	for ( ; i != iEnd; ++i )
	{
		UdpRecvBlock* rb = *i;

		if ( rb->data != 0 )
		{
			K_ASSERT( rb->len > 0 );

			freeData( rb->data, rb->len );
		}

		freeRecvBlock( rb );
	}

	m_recvList.clear();
}

} // gk
