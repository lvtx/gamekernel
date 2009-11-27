#pragma once 

#include <kcore/sys/Lock.h>
#include <knet/socket/IpAddress.h>
#include <knet/udp/impl/UdpHeader.h>
#include <knet/message/BitStream.h>

namespace gk {

class UdpCommunicator;

/**
 * @class UdpConnection 
 *
 * A very simple, short message UDP communication without in order delivery.
 * Fragmentation and order delivery utility will be provided by upper layer.
 *
 * [1] Handling errors
 *     - UdpConnection must report to UdpCommunicator with a message when error happens. 
 *     - UdpCommunicator cleans up those error connections during tick 
 *
 * [2] Locking 
 *     - Contentions with IoWorkers happen
 *     - Only recv call needs a lock. (m_recvLock)
 *     - These can be called, "role lock" instead of "object lock"
 */
class UdpConnection 
{
	friend class Reliable;

public:
	UdpConnection();
	~UdpConnection();

	/**
	 * Start opening UdpConnection. Send SYN.
	 *
	 * @param selfTag The tag of myself given by server
	 * @param remoteTag The tag of remote given by server
	 * @param in The internal ip:port of remote peer
	 * @param ex The external ip:port of remote peer 
	 */
	bool Init( UdpCommunicator* communicator, uint selfTag, uint remoteTag, const IpAddress& in, const IpAddress& ex );

	/**
	 * Target address is settled
	 *
	 * @param addr The address to settle
	 */
	void Settle( const IpAddress& addr );

	/**
	 * Communicator put bytes to this UdpConnection 
	 *
	 * @param data The bytes received
	 * @param len The length of bytes received
	 */
	void OnRecv( void* data, uint len );

	/**
	 * Send bytes to network reliably
	 *
	 * @param data The bytes to send
	 * @param len The length of bytes to send
	 * @return true if successful
	 */
	bool SendReliable( void* data, uint len ); 

	/**
	 * Send data to network reliable and ordered
	 *
	 * @param data The bytes to send
	 * @param len The length of bytes to send
	 * @return true if successful
	 */
	bool SendOrdered( void* data, uint len ); 

	/**
	 * Send bytes to network which can be lost
	 *
	 * @param data The bytes to send
	 * @param len The length of bytes to send
	 */
	bool SendLossy( void* data, uint len ); 

	/**
	 * Send bytes to network without any processing 
	 *
	 * @param data The bytes to send
	 * @param len The length of bytes to send
	 */
	bool SendRaw( void* data, uint len ); 

	/**
	 * Run periodical routines. Retransmission, NUL to keep-alive, HPN if started
	 */
	void Run();

	/**
	 * Close the UdpConnection. Send RST and wait for some time.
	 */
	void Close();

	/**
	 * Cleanup
	 */
	void Fini();

	/** 
	 * Get my tag of udp connection
	 */
	uint GetSelfTag() const;

	/**
	 * Get remote tag of udp connection
	 */
	uint GetRemoteTag() const;

	/**
	 * Returns error
	 */
	int GetError() const;

	/**
	 * true if open 
	 */
	bool IsOpen() const;

	/**
	 * True if closed
	 */
	bool IsClosed() const;

	/**
	 * Returns peer address
	 */
	const IpAddress& GetPeerAddress() const;
	const IpAddress& GetInAddress() const;
	const IpAddress& GetExAddress() const;

	/** 
	 * Set loss rate for testing
	 */
	void SetLossy( uint rate );

private:
	enum State
	{
		  INITIAL
		, CLOSED
		, SYN_SENT
		, SYN_RCVD
		, OPEN
		, CLOSE_WAIT
	};

	enum 
	{
		  OPEN_TIMEOUT 			= 30000
		, CONNECTION_TIMEOUT    = 15000
		, KEEP_ALIVE_TIMEOUT    =   800
		, HPN_RESEND_INTERVAL 	=   400
		, SYN_RESEND_INTERVAL 	=   800
		, ACK_RESEND_INTERVAL 	=   800
		, CLOSE_WAIT_TIMEOUT 	=  1000
		, START_RTT 			=   100
	};

	enum 
	{
		  ERROR_NONE
		, ERROR_UDP_OPEN_TIMEOUT
		, ERROR_UDP_CLOSED
	};

	void onSynRcvd();
	void onReset();
	void onNulRcvd();
	void onAckRcvd();
	void onUnreliableRecv( const UdpHeader& header, void* data, uint len );

	void processRecvBlocks();
	void processBlock( void* data, uint len );
	void processHpn();
	void processSyn();
	void processAck();
	void processTimeout();
	void processReliable();
	void processKeepAlive();

	void sendHpn();
	void sendSyn();
	void sendAck();
	void sendNul();
	void sendRst();

	void notifyOpen();
	void notifyOpenTimeout();
	void notifyClosed();

private:
	int 				m_state;
	int 				m_error;
	uint 				m_selfTag;
	uint 				m_remoteTag;
	IpAddress 			m_peer;
	IpAddress 			m_in;
	IpAddress 			m_ex;
	bool 				m_settled;

	BitStream 			m_recvBlock1;
	BitStream 			m_recvBlock2;
	BitStream*			m_readBlock;
	BitStream*			m_accBlock;
	Mutex 				m_recvLock;
	uint 				m_blockCount;
	uint				m_lossRate;			// loss simulation

	Tick				m_tickHpnResend;
	Tick				m_tickSynResend;
	Tick				m_tickAckResend;
	Tick 				m_tickKeepAlive;
	Tick 				m_tickOpenTimeout;
	Tick 				m_tickInactive;
	Tick 				m_tickCloseWait;

	UdpCommunicator* 	m_communicator;	
	Reliable* 			m_reliable;
};

inline 
uint 
UdpConnection::GetSelfTag() const 
{
	return m_selfTag;
}

inline 
uint 
UdpConnection::GetRemoteTag() const 
{
	return m_remoteTag;
}

inline
int 
UdpConnection::GetError() const
{
	return m_error;
}

inline
const IpAddress& 
UdpConnection::GetPeerAddress() const
{
	return m_peer;
}

inline
const IpAddress& 
UdpConnection::GetInAddress() const
{
	return m_in;
}

inline
const IpAddress& 
UdpConnection::GetExAddress() const
{
	return m_ex;
}

inline
bool 
UdpConnection::IsOpen() const
{
	return m_state == OPEN;
}

inline
bool 
UdpConnection::IsClosed() const
{
	return m_state == CLOSED;
}

inline
void 
UdpConnection::SetLossy( uint rate )
{
	m_lossRate = rate;
}

} // gk 
