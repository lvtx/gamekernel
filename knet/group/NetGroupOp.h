#pragma once 

#include <knet/NetSecurity.h>

namespace gk {

/**
 * @struct NetGroupOp
 *
 * Used to command network group operatins.
 */
struct NetGroupOp 
{
	enum Op 
	{
		  CREATE 
		, JOIN
		, LEAVE
		, DESTROY
	};

	uint 			groupId;
	uint 			connectionId;
	SecurityLevel 	sl;
	tstring			extra;
	Op 				op;

	NetGroupOp() 
	: groupId( 0 ) 
	, connectionId( 0 )
	, sl( SECURITY0 )
	, extra()
	, op( CREATE )
	{
	}
};

} // gk
