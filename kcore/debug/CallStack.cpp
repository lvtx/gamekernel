#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/debug/CallStack.h>
#include <kcore/debug/SymbolEngine.h>

#if defined(_M_IX86)
#define SIZEOFPTR 4
#define AXREG eax
#define BPREG ebp
#elif defined(_M_X64)
#define SIZEOFPTR 8
#define X86X64ARCHITECTURE IMAGE_FILE_MACHINE_AMD64
#define AXREG rax
#define BPREG rbp
#else
#error Unsupported platform - I386 or AMD64 only
#endif // _M_IX86

namespace gk 
{
static DWORD_PTR getprogramcounterx86x64 ();


CallStack::CallStack()
{
    
}

CallStack::~CallStack()
{
}

bool 
CallStack::Trace( CONTEXT* context )
{
    // silently ignore if symbol engine is not initialized. 
    if ( !SymbolEngine::Instance()->IsInitialized() )
	{
		return true;
	}

    CONTEXT capture;
    memset( &capture, 0, sizeof(CONTEXT) );

    if ( context == 0 ) // use kernel function to retrieve context
    {
        // RtlCaptureContext() doesn't help... why?
        context = &capture;
    }

    K_ASSERT( context != 0 );

    DWORD_PTR    framepointer;
    DWORD_PTR    programcounter;

    programcounter = getprogramcounterx86x64();
    __asm mov [framepointer], BPREG // Get the frame pointer (aka base pointer)

    STACKFRAME64 frame;
    memset( &frame, 0, sizeof(frame) );

    frame.AddrPC.Offset    = programcounter;
    frame.AddrPC.Mode      = AddrModeFlat;  // defined in dbghelp.h
    frame.AddrFrame.Offset = framepointer;
    frame.AddrFrame.Mode   = AddrModeFlat;

    while ( StackWalk64( 
                IMAGE_FILE_MACHINE_I386, 
                GetCurrentProcess(), 
                GetCurrentThread(), 
                &frame, 
                context, 
                0, 
                SymFunctionTableAccess64, 
                SymGetModuleBase64,
                0 ) == TRUE )
    {
        frames_.push( frame );
    }

    return true;
}

void 
CallStack::Print( tostream& out )
{
    // silently ignore if symbol engine is not initialized. 
    if ( !SymbolEngine::Instance()->IsInitialized() )
	{
		return;
	}

    tstring name;

    while ( !frames_.empty() )
    {
        STACKFRAME64& frame = frames_.top();

        SymbolEngine::Instance()->Translate( frame.AddrPC.Offset, name );

        out << name << std::endl;

        frames_.pop();
    }
}

#pragma auto_inline(off)
DWORD_PTR getprogramcounterx86x64 ()
{
    DWORD_PTR programcounter;

    __asm mov AXREG, [BPREG + SIZEOFPTR] // Get the return address out of the current stack frame
    __asm mov [programcounter], AXREG    // Put the return address into the variable we'll return

    return programcounter;
}

#pragma auto_inline(on)

} // gk 
