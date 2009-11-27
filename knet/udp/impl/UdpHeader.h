#pragma once 

#include <kcore/mem/AllocatorAware.h>
#include <kcore/sys/Tick.h>
#include <kcore/sys/Buffer.h>

namespace gk {

enum 
{
	  MAX_SEGMENT_SIZE  	= 512 		// MTU safe? 
};

/**
 * @struct UdpHeader 
 *
 */
struct UdpHeader 
{
	byte control; 				///< SYN|ACK|EAK|RST|NUL|HPN|RLE|ORD
	byte length; 				///< Header length
	uint srcId; 				///< connection id of source
	uint dstId; 				///< connection id of destination 
	int  seq; 					///< sequence number 
	int  ack; 					///< cumulative ack number
	int  bodyLen;				///< body length

	enum 
	{ 
		  SYN = 7
		, ACK = 6
		, EAK = 5
		, RST = 4
		, NUL = 3
		, HPN = 2
		, RLE = 1
		, ORD = 0
	};

	/**
	 * [1] When EAK is set, 
	 * byte eakCount
	 * int  eaks[eakCount]
	 *
	 * [2] When HPN is set, 
	 * TODO - specify laster when writing NetClient/NetServer
	 *
	 * EAK, RST, NUL, HPN, FRG are mutually exclusive.
	 * They cannot be set at the same time.
	 * This means only one extended field can be used.
	 * This is for simpler implementation of recv routine.
	 */

	UdpHeader();

	void Set( byte bit );
	bool IsSet( byte bit ) const;
};

/**
 * @struct UdpSendBlock 
 *
 * Kept till ack/eak is received.
 */
struct UdpSendBlock : public AllocatorAware
{
	UdpHeader 		header;		///< when resending don't use extend field 
	byte*	 		data; 		///< payload data
	uint 			len; 		///< length of payload

	byte 	   		rxCount; 	///< transmission count
	Tick    		rxmt; 		///< retransmit timer	

	UdpSendBlock();
};

/**
 * @struct UdpRecvBlock 
 *
 * Only keeps out of order recvs.
 *
 */
struct UdpRecvBlock  : public AllocatorAware
{
	int 			seq; 		///< received sequence
	void* 			data;		///< only for segments received not in order
	uint 			len; 		///< segment payload size
	uint			ackCount; 
	bool			ordered;

	UdpRecvBlock();
};

}  // udp
