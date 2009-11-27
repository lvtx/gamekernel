#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/debug/CallStack.h>
#include <kcore/debug/SymbolEngine.h>

#include <dbghelp.h>

using namespace gk;

SymbolEngine* 
SymbolEngine::Instance()
{
    static SymbolEngine instance;

    return &instance;
}

SymbolEngine::SymbolEngine()
: initialized_(false)
{
}

bool 
SymbolEngine::Initialize()
{
    initialized_ = (SymInitialize( GetCurrentProcess(),  0, true ) == TRUE);

    return initialized_;
}

bool 
SymbolEngine::Translate( uint8 addr, tstring& name )
{
    K_ASSERT( initialized_ );

    static char symbolBuffer[ sizeof(IMAGEHLP_SYMBOL64) + 255 ];
    memset( symbolBuffer , 0 , sizeof(IMAGEHLP_SYMBOL64) + 255 );

    // Cast it to a symbol struct:
    IMAGEHLP_SYMBOL64 * symbol = (IMAGEHLP_SYMBOL64*) symbolBuffer;

    // Need to set the first two fields of this symbol before obtaining name info:
    symbol->SizeOfStruct    = sizeof(IMAGEHLP_SYMBOL64) + 255;
    symbol->MaxNameLength   = 254;

    // The displacement from the beginning of the symbol is stored here: pretty useless
    uint8 displacement = 0;

    // Get the symbol information from the address of the instruction pointer register:
    if ( SymGetSymFromAddr64(GetCurrentProcess(), addr, &displacement, symbol ) )
    {
#if defined(UNICODE) || defined(_UNICODE)
		size_t len = strlen( symbol->Name );

        TCHAR* wc = (TCHAR*)::malloc( sizeof(TCHAR) * len * 2 );

        MultiByteToWideChar( CP_ACP, 0, symbol->Name, -1, wc, (int)len*2 );
        name = wc;
		::free( wc );
#else
		name = symbol->Name;
#endif
        return true;
    }
    // else -- failed

    name = _T("Unknown");

    return false;
}

void 
SymbolEngine::Destroy()
{
    if ( initialized_ )
    {
        SymCleanup( GetCurrentProcess() );

        initialized_ = false;
    }
}
