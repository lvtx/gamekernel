#pragma once 

#include <knet/udp/impl/UdpHeader.h>

#include <hash_map>

namespace gk {

class UdpCommunicator;
class UdpConnection;

/**
 * @class Reliable 
 *
 * Reliable UDP communication based around RFC908
 *
 * Concepts added:
 * [1] Send always succeeds without sending (zero send count)
 * [2] Only 1 header extension is allowed 
 * [3] Cumulative ack is delayed (to decrease ack traffic)
 * [4] RTT is measured on ack with retransmission timer (incorrect, but usable)
 *
 * Important TODO list:
 * [1] Buffer memory management 
 */
class Reliable 
{
public:
	Reliable();
	~Reliable();

	/**
	 * Init with connection and communicator.
	 *
	 * @param communicator The UdpCommunicator to notify 
	 * @param connection The UdpConnection to support
	 * @return true if successful
	 */
	bool Init( UdpCommunicator* communicator, UdpConnection* connection );

	/**
	 * Process ack, keep alive, etc
	 */
	void Run();

	/**
	 * When ack received
	 *
	 * @param header The UdpHeader received.
	 */
	void OnAck( const UdpHeader& header );

	/**
	 * When eak received
	 *
	 * @param header The UdpHeader received
	 * @param data The data received
	 * @param len The length of data received
	 */
	void OnEak( const UdpHeader& header, void* data, uint len );

	/**
	 * When a segment received
	 *
	 * @param header The UdpHeader received
	 * @param data The data received
	 */
	void OnRecv( const UdpHeader& header, void* data, uint len );

	/**
	 * Send a segment. Eventually calls UdpConnection::SendRaw
	 *
	 * @param data The bytes to send
	 * @param len The length of bytes to send
	 * @param ordered The tag to direct ordered delivery
	 * @return true if successful
	 */
	bool Send( void* data, uint len, bool ordered = false );

	/**
	 * Cleans up 
	 */
	void Fini();

	/**
	 * Get calculated cumulative ack to send
	 */
	uint GetSendCumAck() const;

private:
	typedef std::list<UdpSendBlock*> SendBlockList;
	typedef std::list<UdpRecvBlock*> RecvBlockList;

	enum 
	{
		  MAX_OUTSTANDING_SEGMENTS 	= 512
		, START_RTT 			 	= 80 // ms
	};

	enum 
	{
		  CUMULATIVE_ACK_INTERVAL 	= 60
		, RESEND_CHECK_INTERVAL   	= 30
	};

	void processZeroSend();
	void processResend();
	void processCumAck();

	void runEak( int seq );
	bool isDuplicateRecv( int seq );
	void insertRecvBlock( UdpRecvBlock* rv );
	void runRecvBlockList();
	void clearIfExists( int seq );

	bool resend( UdpSendBlock* sb, bool force = false );

	void sendEak( uint seq );
	void sendCumAck();

	UdpSendBlock* allocSendBlock();
	void freeSendBlock( UdpSendBlock* p );

	UdpRecvBlock* allocRecvBlock();
	void freeRecvBlock( UdpRecvBlock* p );

	void* allocData( uint len );
	void  freeData( void* data, uint len );

	void freeSendBlocks();
	void freeRecvBlocks();

private:
	UdpCommunicator* m_communicator;
	UdpConnection* 	 m_connection;

	SendBlockList  	 m_sendList;
	RecvBlockList  	 m_recvList;

	int 		m_sendSeq;
	int 		m_recvCumAck;			// last cum ack received from sender
	int 		m_sendCumAck;			// last cum ack calculated on receiver
	int 		m_outstandingSegs; 		// sent, but unack'ed segments
	int 		m_zeroSendCount;
	int 		m_averageRtt; 			// one way trip time

	uint		m_cumAckInterval;

	Tick 		m_tickCumulativeAck;
	Tick 		m_tickResend;
};

inline
uint 
Reliable::GetSendCumAck() const
{
	return m_sendCumAck;
}

} // gk 
