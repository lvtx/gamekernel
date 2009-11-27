#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/message/MessageSerialization.h>

namespace gk { 

bool 
Pack( BitStream& bs, const IpAddress& addr )
{
	bs.Write( addr.ToString() );

	return bs.IsValid();
}

bool 
Unpack( BitStream& bs, IpAddress& addr )
{
	tstring s;

	bs.Read( s );

	addr.Init( s );

	return bs.IsValid();
}

} // gk
