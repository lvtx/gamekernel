#pragma once 

#include <knet/socket/IpAddress.h>

namespace gk {

/** 
 * @struct NetGroupMember 
 *
 * NetGroup member structure
 */
struct NetGroupMember
{
	enum State 
	{
		  INIT
		, PREPARED
		, JOINED
	};

	uint 	  	connectionId;
	IpAddress 	in;
	IpAddress 	ex;
	State 	  	state;
	tstring		extra;

	NetGroupMember();
};

} // gk
