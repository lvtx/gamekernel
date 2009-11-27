#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/sys/Thread.h>
#include <kcore/base/PanicError.h>

#include <process.h>

namespace gk {

int threadFunc(void* t);

bool 
Thread::Start()
{
    uint4 tid = 0;

    running_ = true;

	thread_ = (unsigned long*)::CreateThread(
                                    0,
                                    0,
                                    (LPTHREAD_START_ROUTINE)threadFunc,
                                    (Thread*)this,
                                    0,
                                    &tid);

	if ( thread_ == 0 )
	{
		running_ = false;

		return false;
	}

	threadId_ = tid;


	::Sleep( 100 );

    return true;
}

int 
Thread::Run()
{
    return 0;
}

void 
Thread::Suspend()
{
    K_ASSERT( thread_ != 0 );
    
	if ( thread_ == 0 )
	{
		return;
	}

	if ( ::SuspendThread( thread_ ) < 0 )
    {
        throw PanicError(_T("Failed to suspend thread"));
    }

    suspended_ = true;
}

void 
Thread::Resume()
{
    K_ASSERT( thread_ != 0 );
    
	if ( thread_ == 0 )
	{
		return;
	}

	if ( ::ResumeThread( thread_ ) < 0 )
    {
        throw PanicError(_T("Failed to resume thread"));
    }

    suspended_ = false;
}

void 
Thread::Stop()
{
	if ( !IsRunning() ) {
		return;
	}

    if ( thread_ == 0 )
	{
		return;
	}

    running_   = false; // to notify and make a thread stop

    helpStop();
    
	::WaitForSingleObject( thread_, INFINITE );

	::CloseHandle( thread_ );  

    thread_ = 0;

    suspended_ = false;
}

void 
Thread::helpStop()
{
    // empty
}

int threadFunc(void* t)
{
	Thread* thread = reinterpret_cast<Thread*>(t);

    if ( thread )
    {
        return thread->Run();
    }

    return -1;
}

} // gk
