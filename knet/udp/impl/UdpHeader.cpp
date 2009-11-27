#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/udp/impl/UdpHeader.h>

namespace gk {

UdpHeader::UdpHeader()
: control( 0 )
, length( sizeof( UdpHeader ) )
, srcId( 0 )
, dstId( 0 )
, seq( 0 )
, ack( 0 )
, bodyLen( 0 )
{
}

void
UdpHeader::Set( byte bit )
{
	byte on = 0x01;

	if ( bit > 0 )
	{
		on = on << bit;
	}

	control = control | on;	
}

bool 
UdpHeader::IsSet( byte bit ) const
{
	byte on = control;

	if ( bit > 0 )
	{
		on = on >> bit;
	}

	return ( on & 0x01 );	
}

UdpSendBlock::UdpSendBlock()
: header()
, data( 0 )
, len( 0 )
, rxCount( 0 )
, rxmt()
{
}

UdpRecvBlock::UdpRecvBlock()
: seq( 0 )
, data( 0 )
, len( 0 )
, ordered( false )
, ackCount( 0 )
{
}

} // gk
