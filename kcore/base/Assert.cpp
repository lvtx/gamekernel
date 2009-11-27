#include "stdafx.h"

#include <kcore/corebase.h>

#include <cassert>

void nassert( bool cond, const char* cs, int line, const char* file )
{
	file;
	line;
	cs;

	if ( !cond )
	{
#ifdef _DEBUG
		__asm { int 3 };
#endif
		
		assert( cond );
	}
}
