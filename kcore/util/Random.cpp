#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/util/Random.h>

#include <kcore/sys/Lock.h>
#include <kcore/sys/ScopedLock.h>
#include <time.h>

namespace gk 
{

Atomic<int> Random::seed;

int Random::Rand()
{
	if ( seed == 0 )
	{
		::srand( (unsigned)::time( NULL ) ); 

		seed = 1;
	}

	return ::rand();
}

} // gk
