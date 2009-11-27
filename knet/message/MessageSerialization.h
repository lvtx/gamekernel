#pragma once 

#include <knet/message/BitStream.h>
#include <knet/socket/IpAddress.h>

namespace gk
{

/**
 * Packs an IpAddress
 */
bool Pack( 		BitStream& bs, const IpAddress& addr );

/**
 * Unpacks an IpAddress
 */
bool Unpack( 	BitStream& bs, IpAddress& addr );

// add more serialization functions here

} // gk
