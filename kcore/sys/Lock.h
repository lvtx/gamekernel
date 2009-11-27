#pragma once

namespace gk 
{
/**
 * @class Lockable 
 * 
 *  Provides lock interface.
 */
class Lockable 
{
public:
    Lockable();
    virtual ~Lockable();

    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};


inline
Lockable::Lockable() 
{
}

inline
Lockable::~Lockable()
{
}

/** 
 * @class NullLock 
 *
 * Do nothing lock
 */
class NullLock : public Lockable 
{
public:
    NullLock() {}
    ~NullLock() {}

    virtual void Lock() {}
    virtual void Unlock() {}
};

/**
 * @class Mutex 
 * 
 *  Platform dependency on Windows 2000 or higher
 *  
 *  NOTE
 *   Windows CRITICAL_SECTION is a recursive lock.
 *   gamekernel depends on this fact.
 */
class Mutex : public Lockable 
{
public:
    Mutex();
    ~Mutex();

    virtual void Lock();
    virtual void Unlock();

private:
    mutable CRITICAL_SECTION cs_;
};

} // gk 
