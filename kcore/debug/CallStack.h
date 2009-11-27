#pragma once

#include <dbghelp.h>
#include <stack>
#include <fstream>

namespace gk 
{

/**
 * @class CallStack
 *
 * Traces stack frames
 *
 * Synopsis:
 *  
 *   CallStack cs;
 *
 *   if ( cs.Trace() )
 *   {
 *     cs.Print( se, out );
 *   }
 */
class CallStack 
{
public:
    CallStack();
    ~CallStack();

    /**
     * Traces stack using context. 
     * If context is 0, then asm routine is used.
     * 
     * @param context - running context (process registers)
     *
     * @return false - when error, true - success
     */
    bool Trace( CONTEXT* context = 0 );

    /**
     * Prints out symbols to out
     */
    void Print( tostream& out );

private:
    typedef std::stack<STACKFRAME64, std::deque<STACKFRAME64> > StackFrame;

    StackFrame frames_;
};

} // gk
