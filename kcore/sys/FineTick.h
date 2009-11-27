#pragma once

namespace gk 
{
/**
 * @class FineTick
 * 
 * Tick count using QueryPerformanceCounter/QueryPerformanceFrequency
 */
class FineTick 
{
public:
    FineTick();
    ~FineTick();

    /**
     * Returns elapsed time in seconds
	 *
	 * @return elapsed time in seconds
     */
    double Elapsed() const;

    /**
     * Reset start time to the current time 
     */
    void Reset() const;

private:
    mutable longlong start_;
};

inline
FineTick::FineTick()
{
   Reset();
}

inline
FineTick::~FineTick()
{
}

inline
double
FineTick::Elapsed() const
{
    longlong end = 0;
    ::QueryPerformanceCounter( (LARGE_INTEGER*)&end );

    longlong freq = 0;
    ::QueryPerformanceFrequency( (LARGE_INTEGER*)&freq );

    K_ASSERT( freq > 0 );

    return (double( (end - start_) ) / freq);
}

inline
void
FineTick::Reset() const
{
     ::QueryPerformanceCounter( (LARGE_INTEGER*)&start_ );
}

} // gk 
