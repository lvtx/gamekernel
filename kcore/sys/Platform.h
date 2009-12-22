#pragma once 

namespace gk {

/**
 * @class Platform
 *
 * Wrapper for platform specific functions. 
 * 
 */
class Platform 
{
public:
	static sprintf( TCHAR* buf, TCHAR* fmt, ... );
};

} // namespace gk
