#pragma once 

#include <cassert>

extern void nassert( bool cond, const char* cs, int line, const char* file );

#define K_ASSERT( cond ) nassert( (cond), #cond, __LINE__, __FILE__ )

#define K_DELETE( p ) \
if ( p != 0 ) \
{ \
	delete p; \
	p = 0; \
}

#define K_RETURN_IF( cond ) \
	if ( (cond) ) return;

#define K_RETURN_V_IF( cond, val ) \
	if ( (cond) ) return (val);


