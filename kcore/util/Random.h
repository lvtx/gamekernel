#pragma once 

#include <kcore/sys/Atomic.h>

namespace gk 
{
/** 
 * @class Random 
 *
 * Wraps system random function
 */
class Random 
{
public:
	/**
	 * A system rand wrapper
	 *
	 * @return Random value generated with time seed.
	 */
	static int Rand();

private:
	static Atomic<int> seed;
};

} // gk 

