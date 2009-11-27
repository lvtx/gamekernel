#pragma once 

namespace gk {

enum SecurityLevel 
{
	  SECURITY0 		///< plain. version validation.
	, SECURITY1 		///< encrypted with a session key
	, SECURITY2 		///< SECURITY1 + sequence check. Not implemented yet.
};

} // gk
