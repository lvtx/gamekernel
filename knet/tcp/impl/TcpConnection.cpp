#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/tcp/impl/TcpConnection.h>

#include <kcore/sys/Logger.h>
#include <kcore/sys/ScopedLock.h>
#include <knet/message/MessageFactory.h>
#include <knet/message/net/NetStateMessage.h>
#include <knet/message/net/NetHandshake.h>
#include <knet/tcp/TcpCommunicator.h>
#include <knet/NetErrors.h>

namespace gk {

TcpConnection::TcpConnection()
: m_communicator( 0 )
, m_id( 0 )
, m_socket( 0 )
, m_error( 0 )
, m_accepted( true )
, m_handshaking( false )
, m_lockRecv()
, m_lockSend()
, m_sendRequestCount( 0 )
, m_recvRequestCount( 0 )
, m_sendBuffer1()
, m_sendBuffer2()
, m_recvBuffer() 
, m_accBuffer( 0 )
, m_sendBlock() 
, m_recvBlock()
, m_groupId( 0 )
{
}

TcpConnection::~TcpConnection()
{
	Fini();
}

bool 
TcpConnection::Init( TcpCommunicator* communicator, uint id, Socket* socket, 
		             SecurityLevel sl, bool accepted )
{
    K_ASSERT( communicator != 0 );
    K_ASSERT( id > 0 );
    K_ASSERT( socket != 0 );

	m_communicator  = communicator;
	m_id 			= id;
    m_socket      	= socket;
	m_error 		= 0;
	m_sl 			= sl;
	m_accepted 		= accepted;
	m_handshaking   = true;

	m_sendRequestCount = 0;
	m_recvRequestCount = 0;

    m_sendBuffer1.Clear();
    m_sendBuffer2.Clear();
	m_recvBuffer.Clear();
	m_accBuffer = &m_sendBuffer1;

	m_groupId = 0;

    m_socket->SetNonblocking();

	LOG( FT_DEBUG, 
		 _T("TcpConnection::Init> Id %d Socket %d"),
		 id,
		 socket->GetSystemSocket() );

    return true;
}

void 
TcpConnection::StartHandshake()
{
	K_ASSERT( m_accepted );
	K_ASSERT( m_handshaking );

	if ( m_sl > SECURITY0 )
	{
		if ( m_accepted )
		{
			m_cipher.Init();
		}
	}

	sendHandshake(); 		// security level is sent with challenge

	NetStateMessage* nsm = new NetStateMessage;

	nsm->state 			= NetStateMessage::TCP_OPEN;
	nsm->connectionId 	= GetId();
	nsm->sl 			= m_sl;
	nsm->addr			= m_socket->GetPeerAddress();

	m_communicator->Notify( MessagePtr( nsm ) );
	
	m_handshaking = false; 	// finished quickly on server
	m_cipher.SetEstablished();
}

void 
TcpConnection::WaitHandshake()
{
	K_ASSERT( !m_accepted );
	K_ASSERT( m_handshaking );

	// we don't know security level here

	RequestRecv();
}

void 
TcpConnection::Run()
{
	// empty for now
	// [1] handshake 
}

void 
TcpConnection::Send( MessagePtr m )
{
	K_ASSERT( m_accBuffer != 0 );
	K_ASSERT( m.Get() != 0 );
	K_ASSERT( m->type != NET_HANDSHAKE );
	K_ASSERT( !m_handshaking );

	BitStream bs;

	m->Pack( bs );

	if ( m_sl > SECURITY0 )
	{
		m_cipher.Encrypt( bs );
	}

	{
		ScopedLock sl( m_lockSend );

		m_accBuffer->WriteInt( bs.GetBytePosition(), 16 );
		m_accBuffer->WriteInt( 0, 8 );
		m_accBuffer->Write( bs.GetBytePosition(), bs.GetBuffer() );
	}

    RequestSend();
}

void 
TcpConnection::Send( void* data, uint len )
{
	K_ASSERT( m_accBuffer != 0 );
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );

	if ( m_sl > SECURITY0 )
	{
		BitStream bs;

		bs.Write( len, data );

		m_cipher.Encrypt( bs );

		{
			ScopedLock sl( m_lockSend );

			m_accBuffer->WriteInt( bs.GetBytePosition(), 16 );
			m_accBuffer->WriteInt( 0, 8 );
			m_accBuffer->Write( bs.GetBytePosition(), bs.GetBuffer() );
		}
	}
	else
	{
		ScopedLock sl( m_lockSend );

		m_accBuffer->WriteInt( len, 16 );
		m_accBuffer->WriteInt( 0, 8 );
		m_accBuffer->Write( len, data );
	}

    RequestSend();
}

// IoAgent { 
HANDLE 
TcpConnection::RequestHandle()
{
	K_ASSERT( m_socket != 0 );

	return (HANDLE)m_socket->GetSystemSocket();
}

bool 
TcpConnection::RequestSend()
{
    if ( HasError() )
    {
        return false;
    }

	ScopedLock slSend( m_lockSend );

    if ( m_sendRequestCount > 0 )
    {
		K_ASSERT( m_sendRequestCount == (uint)1 );

        return true; // we already have one. 1-send.
    }


	K_ASSERT( m_accBuffer != 0 );

	if ( m_accBuffer->GetBitPosition() == 0 )
	{
		return true; // no data to send
	}

	BitStream* cacc = m_accBuffer;

	K_ASSERT( m_sendRequestCount == (uint)0 );

	if ( m_accBuffer == &m_sendBuffer1 )
	{
		m_accBuffer = &m_sendBuffer2;
	}
	else 
	{
		K_ASSERT( m_accBuffer == &m_sendBuffer2 );

		m_accBuffer = &m_sendBuffer1;
	}

	m_accBuffer->Clear();
	m_accBuffer->Reset();

	K_ASSERT( cacc != m_accBuffer );

	m_sendBlock.op  		= IoBlock::OP_WRITE;
    m_sendBlock.buf.buf    = (char*)cacc->GetBuffer();
    m_sendBlock.buf.len    = (ulong)cacc->GetBytePosition();
    m_sendBlock.totalLen   = m_sendBlock.buf.len;
    m_sendBlock.extra      = this;

    int error = m_socket->AsyncSend( &m_sendBlock );

    if ( error > 0 )
    {
        OnIoError( error, &m_sendBlock );

        return false;
    }
	
    m_sendRequestCount.Inc();

    return true; 
}

bool 
TcpConnection::RequestRecv()
{
    ScopedLock sl( m_lockRecv );

    if ( HasError() )
    {
        return false;
    }

    if ( m_recvRequestCount > 0 )
    {
		K_ASSERT( m_recvRequestCount == (uint)1 );

        return true; // we already have one. 1-recv 
    }

    m_recvBlock.op          = IoBlock::OP_READ;
    m_recvBlock.buf.buf     = 0;   // no buffer needed
    m_recvBlock.buf.len     = 0; 
    m_recvBlock.totalLen    = 0;   // not used for recv. we used 0 recv
    m_recvBlock.extra       = this;

    int error = m_socket->AsyncRecv( &m_recvBlock );

    if ( error > 0 )
    {
        OnIoError( error, &m_recvBlock );

        return false;
    }

	K_ASSERT( m_recvRequestCount == (uint)0 );

    m_recvRequestCount.Inc();

    return true; 
}

void 
TcpConnection::OnSendCompleted( IoBlock* io )
{
	K_ASSERT( io != 0 );
	K_ASSERT( io->buf.buf != 0 );
    K_ASSERT( io->op == IoBlock::OP_WRITE );
	K_ASSERT( io->buf.buf != (char*)m_accBuffer->GetBuffer() );

	ScopedLock sl( m_lockSend ); // Windows spin lock is reentrant

	m_sendRequestCount.Dec();

	RequestSend();
}

void 
TcpConnection::OnSendCompleted( IoBlock* io, uint bytesSent )
{
    K_ASSERT( io != 0 );
	K_ASSERT( io->buf.buf != 0 );
    K_ASSERT( bytesSent > 0 );
    K_ASSERT( io->totalLen > bytesSent );
    K_ASSERT( io->op == IoBlock::OP_WRITE );

	LOG( FT_DEBUG, _T("Incomplete send") );

    // move byets forward. 
    ::memmove_s( (void*)io->buf.buf,               io->buf.len, 
                 (void*)(io->buf.buf + bytesSent), io->buf.len - bytesSent );

    // decrease total bytes
    io->totalLen -= bytesSent;
    io->buf.len  -= bytesSent;

    int error = m_socket->AsyncSend( io );

    if ( error > 0 )
    {
        OnIoError( error, io );
    }
}

void 
TcpConnection::OnRecvCompleted( IoBlock* /* io */ )
{
	ScopedLock sl( m_lockRecv );

	m_recvRequestCount.Dec();

    byte b[2048]; 

	// ::memset( b, 0, 1024 );

	// TODO: Optimization can be done with direct copy to recv buffer

	int len = m_socket->Recv( b, 2048 );

	if ( len == 0 )
	{
		OnIoError( SOCKET_ERROR, &m_recvBlock );

		return;
	}

	if ( len == SOCKET_ERROR )
	{
		int error = GetLastError();

		if ( error != WSAEWOULDBLOCK )
		{
			OnIoError( error, &m_recvBlock );
		}

		return;
	}

	K_ASSERT( len > 0 && len <= 2048 );

	LOG( FT_DEBUG_FLOW, _T("TcpConnection::OnRecvCompleted> %d added"), len );

	m_recvBuffer.Accumulate( b, len ); // a faster version of write. 

	LOG( FT_DEBUG_FLOW, _T("TcpConnection::OnRecvCompleted> Buff %d bytes"), m_recvBuffer.GetBytePosition() );

	MessagePtr m = buildMessage();

	while ( m.Get() != 0 && !HasError() )
	{
		m->remote = m_id;				//	
		m_communicator->Notify( m ); 	// NOTE: called by IOCP thread

		m = buildMessage();
	}

	RequestRecv();
}

void 
TcpConnection::OnIoError( int ec, IoBlock* io )
{	
	ScopedLock slRecv( m_lockRecv );
	ScopedLock slSend( m_lockSend );

	if ( m_socket == 0 )
	{
		return;
	}

	if ( HasError() )
	{
		return;
	}

	m_error = ec;
	
	if ( io->op == IoBlock::OP_READ )
	{
		if ( m_recvRequestCount > 0 )
		{
			m_recvRequestCount.Dec();
		}
	}
	else
	{
		if ( io->op == IoBlock::OP_WRITE )
		{
			if ( m_sendRequestCount > 0 )
			{
				m_sendRequestCount.Dec();
			}
		}
	}

    tstring e = m_socket->GetErrorDesc( m_error );

    LOG( FT_INFO, 
        _T("TcpConnection::OnIoError> Conn[%d] Socket[%d] with %d:%s"), 
         GetId(), 
         m_socket->GetSystemSocket(), 
         m_error, 
         e.c_str() );

	K_ASSERT( m_socket != 0 );

	// notify closed message 
	NetStateMessage* m = new NetStateMessage;

	m->state 		= NetStateMessage::TCP_CLOSED;
	m->connectionId = m_id;
	m->groupId		= m_groupId;
	m->addr 		= m_socket->GetPeerAddress();
	m->socket 		= 0;

	m_communicator->Notify( MessagePtr( m ) );
}
// } 

void
TcpConnection::Close()
{
	K_ASSERT( m_socket != 0 );

	m_socket->Close();

	// an error will occur and eventually cleaned up
}

void 
TcpConnection::Fini()
{
	// m_socket->Close() cancels IOCP recv

    if ( m_socket != 0 )
    {
        m_socket->Close();

		while ( ( m_recvRequestCount + m_sendRequestCount ) > 0 )
		{
			::Sleep( 1 ); // XXX: this is still not correct.
		}

		ScopedLock slRecv( m_lockRecv ); // race with OnRecvCompleted -> IoError
		ScopedLock slSend( m_lockSend );

        delete m_socket;

        m_socket = 0;
    }
}

MessagePtr 
TcpConnection::buildMessage()
{
	// buildMessage must be called only from OnRecvCompleted()

	uint usedLen	= 0;
	uint bitPos		= m_recvBuffer.GetBitPosition();
	uint byteLen    = m_recvBuffer.GetBytePosition();

	LOG( FT_DEBUG_FLOW, _T("buildMessage> byteLen %d"), byteLen );

	K_ASSERT( (bitPos & 0x07) == 0 );

	if ( (bitPos & 0x07) != 0 )
	{
		LOG( FT_ERROR, _T("TcpConnection::buildMessage> byte align error") );
	}

	if ( byteLen < HEADER_LEN )
	{
		LOG( FT_DEBUG_FLOW, _T("buildMessage> Header len check") );

		return MessagePtr();
	}

	uint messageLen = 0;

	m_recvBuffer.SetBitPosition( 0 );			// read from front
	m_recvBuffer.ReadInt( messageLen, 16 );

	LOG( FT_DEBUG_FLOW, _T("buildMessage> Message Len %d"), messageLen );

	if ( messageLen > MAX_PACKET_LEN )
	{
		LOG( FT_WARN, 
			_T("TcpConnection::buildMessage> Too big message %d"), 
			messageLen );

		OnIoError( NET_ERROR_MESSAGE_UNPACK, &m_recvBlock );

		return MessagePtr();
	}
	
	if ( (messageLen + (uint)HEADER_LEN) > byteLen ) // not enough data
	{
		m_recvBuffer.SetBitPosition( bitPos ); // recover for accumulation

		LOG( FT_DEBUG_FLOW, _T("buildMessage> Not enough data %d for %d. pos %d"), 
			byteLen, messageLen + HEADER_LEN, m_recvBuffer.GetBytePosition() );

		return MessagePtr();
	}

	uint control = 0;

	m_recvBuffer.ReadInt( control, 8 );

	if ( m_handshaking && !m_accepted ) 
	{
		// use the buffer to setup cipher

		LOG( FT_DEBUG, _T("TcpConnection::buildMessage> Handshake %d"), m_id );

		ushort type = 0;

		m_recvBuffer.Read( type );

		if ( type != NET_HANDSHAKE )
		{
			LOG( FT_ERROR, 
		 		 _T("TcpConnection::buildMessage> Decryption error"), 
		 		 type );

			OnIoError( NET_ERROR_SECURITY, &m_recvBlock );

			return MessagePtr();
		}

		NetHandshake hs;

		bool rc = hs.Unpack( m_recvBuffer );

		if ( !rc )
		{
			LOG( FT_ERROR, 
				 _T("TcpConnection::buildMessage> Handshake failed to unpack"), 
				 type );

			OnIoError( NET_ERROR_MESSAGE_UNPACK, &m_recvBlock );

			return MessagePtr();
		}

		m_sl = hs.sl;

		if ( m_sl > SECURITY0 )
		{
			m_cipher.Init( hs.challenge, Cipher::LEN_CHALLENGE, !m_accepted );
		}

		NetStateMessage* nsm = new NetStateMessage;

		nsm->state 			= NetStateMessage::TCP_OPEN;
		nsm->connectionId 	= GetId();
		nsm->sl 			= m_sl;
		nsm->addr			= m_socket->GetPeerAddress();

		m_communicator->Notify( MessagePtr( nsm ) );

		m_handshaking = false;
		m_cipher.SetEstablished();

		usedLen = messageLen + HEADER_LEN;

		K_ASSERT( usedLen <= (bitPos >> 3) );

		m_recvBuffer.TruncateFront( usedLen );
		m_recvBuffer.SetBitPosition( bitPos - (usedLen << 3) );

		LOG( FT_DEBUG_FLOW, _T( "used %d pos %d buf len %d"), usedLen, bitPos >> 3, m_recvBuffer.GetBytePosition() );

		return MessagePtr();
	}

	K_ASSERT( !m_handshaking );

	// just message part is encrypted
	if ( m_sl > SECURITY0 )
	{
		bool rc = m_cipher.Decrypt( m_recvBuffer, HEADER_LEN, messageLen );

		if ( !rc )
		{
			LOG( FT_ERROR, 
	 			 _T("TcpConnection::buildMessage> Decryption error") );

			OnIoError( NET_ERROR_SECURITY, &m_recvBlock );

			return MessagePtr();
		}
	}

	// we have a message 

	ushort type = 0;

	m_recvBuffer.Read( type );

	K_ASSERT( type > 0 );

	MessagePtr m = MessageFactory::Instance()->Create( type );

	if ( m.Get() == 0 )
	{
		::Sleep( 1000 );  // wait for log message

		LOG( FT_ERROR, 
			 _T("TcpConnection::buildMessage> Type %d is not registered"), 
			 type );

		OnIoError( NET_ERROR_MESSAGE_NOT_REGISTERED, &m_recvBlock );

		return MessagePtr();
	}

	bool rc = m->Unpack( m_recvBuffer );

	if ( !rc )
	{
		LOG( FT_ERROR, 
			 _T("TcpConnection::buildMessage> Type %d failed to unpack"), 
			 type );

		OnIoError( NET_ERROR_MESSAGE_UNPACK, &m_recvBlock );

		return MessagePtr();
	}

	usedLen = messageLen + HEADER_LEN;

	K_ASSERT( usedLen <= (bitPos >> 3) );

	m_recvBuffer.TruncateFront( usedLen );
	m_recvBuffer.SetBitPosition( bitPos - (usedLen << 3) );

	K_ASSERT( (bitPos & 0x7) == 0 ); // byte aligned

	LOG( FT_DEBUG_FLOW, _T( "used %d buf len %d"), 
		 (byteLen - m_recvBuffer.GetBytePosition()), 
		 m_recvBuffer.GetBytePosition() );

	return m;
}

void 
TcpConnection::sendHandshake()
{
	K_ASSERT( m_accepted );
	K_ASSERT( m_handshaking );

	NetHandshake hs;

	hs.sl = m_sl;

	::memcpy( hs.challenge, m_cipher.GetChallenge(), Cipher::LEN_CHALLENGE );
	
	BitStream bs;

	hs.Pack( bs );

	ScopedLock sl( m_lockSend );

	m_accBuffer->WriteInt( bs.GetBytePosition(), 16 );
	m_accBuffer->WriteInt( 0, 8 );
	m_accBuffer->Write( bs.GetBytePosition(), bs.GetBuffer() );

    RequestSend();
}

} // gk
