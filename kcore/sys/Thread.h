#pragma once

#include <kcore/sys/Atomic.h>
#include <kcore/base/Noncopyable.h>

namespace gk 
{
/** 
 * @class Thread 
 * 
 * @brief A thread wrapper
 *
 * Note
 *  Child class must Stop in destructor since destruction 
 *  is called from  child class to parent class.
 */
class Thread : public Noncopyable 
{
public:
	Thread();
	virtual ~Thread();

	/**
	 * Starts thread 
	 */
	bool Start();

	/**
	 * Thread execution function
	 */
	virtual int Run();

	/**
	 * Suspend thread
	 */
	void Suspend();

	/**
	 * Resume thread
	 */
	void Resume();

	/**
	 * Stops and kills the thread
	 */
	void Stop();

	/**
	 * Checks whether this is running
     * Access Lock/Unlock
	 *
	 * @return true if this thread is running
	 */
	bool IsRunning() const;

	/**
	 * Checks whether this is suspended
     * Access Lock/Unlock 
	 *
	 * @return true if this thread is suspended
	 */
	bool IsSuspended() const;

	/**
	 * @return thread id of this thread
	 */
	ulong GetThreadId() const;

	/**
	 * sleeps for ms milliseconds
	 */
	static void Sleep(uint ms);

protected:
    virtual void helpStop();

protected:
	HANDLE thread_;
	ulong  threadId_;

    mutable Atomic<bool> running_;
	mutable Atomic<bool> suspended_;
};

inline
Thread::Thread() 
	: threadId_( 0 ), 
	  running_(false),
	  suspended_(false),
	  thread_(0)
{
}

inline
Thread::~Thread()
{
    K_ASSERT( !IsRunning() ); // child must Stop in destructor
}

inline
bool
Thread::IsRunning() const
{
	return running_;
}

inline
bool
Thread::IsSuspended() const
{
	return suspended_;
}

inline
void 
Thread::Sleep(uint ms)
{
	::Sleep(ms);
}

inline
ulong 
Thread::GetThreadId() const
{
	return threadId_;
}

} // gk 
