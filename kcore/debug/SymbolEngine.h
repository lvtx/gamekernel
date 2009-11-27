#pragma once

#include <string>

namespace gk 
{

/**
 * @class SymbolEngine
 *
 * Translates address into a debug symbol.
 *
 * pdb files must exist in the exe file directoy.
 * 
 * Note
 *  Singleton is as bad as global variables. 
 *  Therefore avoid using Singeton pattern. 
 *
 *  However, SymbolEngine is one of the exceptional cases. 
 *
 *  For symbol engine to work, avoid Maximize Speed (/O2) 
 *  Instead, use full optimization (/Ox) or custom optimzation
 */
class SymbolEngine
{
public:
    static SymbolEngine* Instance();

    bool Initialize();

    bool Translate( uint8 addr, tstring& name );

    void Destroy();

    bool IsInitialized() const;

private:
    SymbolEngine();        // hide 

    bool initialized_;
};

inline
bool 
SymbolEngine::IsInitialized() const
{
    return initialized_;
}

} // gk 

#pragma comment( lib, "dbghelp" )

