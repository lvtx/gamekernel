#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/sys/Lock.h>
#include <kcore/base/PanicError.h>

namespace gk {

const int DEFAULT_SPIN_COUNT = 4000;

Mutex::Mutex()
{
	int rc = ::InitializeCriticalSectionAndSpinCount( &cs_, DEFAULT_SPIN_COUNT );

    if ( rc == 0 )
    {
        // critical error 
        throw PanicError(_T("Mutex::Mutex() - Cannot initialize critical section"));
    }
}

Mutex::~Mutex()
{
	::DeleteCriticalSection( &cs_ );
}

void Mutex::Lock()
{
	::EnterCriticalSection( &cs_ );
}

void Mutex::Unlock()
{
	::LeaveCriticalSection( &cs_ );
}

} // gk
