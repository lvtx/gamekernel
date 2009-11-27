#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/group/NetGroupMember.h>

namespace gk {

NetGroupMember::NetGroupMember()
: connectionId( 0 )
, in()
, ex()
, state( INIT )
, extra()
{
}

} // gk
