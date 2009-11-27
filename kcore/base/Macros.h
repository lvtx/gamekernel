#pragma once 

#include <cassert>

extern void nassert( bool cond, const char* cs, int line, const char* file );

#define K_ASSERT( cond ) nassert( (cond), #cond, __LINE__, __FILE__ )

/*
 * To provide a better or custom assert, write nassert function.
 * kcore/base/Assert.cpp has a real simple example.
 */