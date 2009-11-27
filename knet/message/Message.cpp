#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/message/Message.h>	

namespace gk {

Message::Message()
: type( 0 ) // 0 is invalid type
, remote( 0 )
, remotes()
{
	contextKey.clear();
}

Message::~Message()
{
}

bool 
Message::Pack( BitStream& bs )
{
	K_ASSERT( type > 0 );

	bs.Write( type );
	bs.Write( contextKey );

	return bs.IsValid();
}

bool 
Message::Unpack( BitStream& bs )
{
	// type is already read in to create a Message
	
	K_ASSERT( type > 0 );

	bs.Read( contextKey );

	return bs.IsValid();
}

Message* 
Message::Create()
{
	K_ASSERT( !_T("Should implement this") );

	return (Message*)0; // invalid
}

} // gk
