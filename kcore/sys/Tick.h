#pragma once

#include <kcore/sys/Atomic.h>

namespace gk 
{

/**
 * @class Tick
 * 
 * Tick count using GetTickCount()
 */
class Tick 
{
public:
    Tick();
    ~Tick();

    /**
     * @return Elapsed time in seconds
     */
    unsigned long Elapsed() const;

    /**
     * Reset start time 
     */
    void Reset() const;

private:
    mutable long start_;
};

inline
Tick::Tick()
{
   Reset();
}

inline
Tick::~Tick()
{
}

inline
unsigned long
Tick::Elapsed() const
{
	return (::GetTickCount() - start_ );
}

inline
void
Tick::Reset() const
{
	start_ = ::GetTickCount();
}

} // gk 
