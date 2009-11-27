#pragma once 

#include <kcore/sys/ScopedLock.h>

#include <deque>

namespace gk {
/** 
 * @class Queue
 * 
 * @brief A queue with a lock policy 
 *
 */
template <typename V, class LOCK = NullLock>
class Queue : public Noncopyable
{
public:
    Queue();
    ~Queue();

	/**
	 * Add an entry into a queue
	 *
	 * @param v A value to add
	 */
    void Put( const V& v );

	/**
	 * Check whether queue is empty 
	 *
	 * @return true if the queue is empty
	 */
	bool IsEmpty() const;

	/**
	 * Get an entry from a queue
	 *
	 * @param v The reference to hold result
	 * @return true if queue is not empty 
	 */
    bool Get( V& v);

private:
    typedef std::deque<V> Q;

    Q				q_;
    mutable LOCK    lock_;
};

template <typename V, class LOCK>
inline
Queue<V, LOCK>::Queue()
{
}

template <typename V, class LOCK>
inline
Queue<V, LOCK>::~Queue()
{
    q_.clear();
}

template <typename V, class LOCK>
inline
void
Queue<V, LOCK>::Put( const V& v )
{
    ScopedLock sl( lock_ );

    q_.push_back( v );
}

template <typename V, class LOCK>
inline
bool 
Queue<V,LOCK>::IsEmpty() const
{
	ScopedLock sl( lock_ );

	return q_.empty();
}

template <typename V, class LOCK>
inline
bool
Queue<V, LOCK>::Get( V& v ) 
{ 
    ScopedLock sl( lock_ );

	if ( q_.empty() )
	{
		return false;
	}

    v = q_.front(); // NOTE: Copy is inevitable

    q_.pop_front();

    return true;
}

} // gk 
