#pragma once

#include <kcore/base/Noncopyable.h>
#include <kcore/sys/Lock.h>

namespace gk 
{
/**
 * @class ScopedLock 
 * 
 * Scoped lock for a code block
 */
class ScopedLock : private Noncopyable
{
public:
    ScopedLock( Lockable& lock );
    ~ScopedLock();

private:
    Lockable& lock_;
};

inline
ScopedLock::ScopedLock( Lockable& lock )
: lock_( lock )
{
    lock_.Lock();
}

inline
ScopedLock::~ScopedLock()
{
    lock_.Unlock();
}

} // gk 
