#pragma once

namespace gk 
{
/**
 * @class SharedPointer
 *
 * @brief A thread-safe shared pointer
 *
 * Use this as an automatic garbage collection for widely used pointers
 * like Signal. Explicit handling of pointers is sometimes better. 
 *
 * NOTE
 *  Lock does not work on Windows NT 3.51 and earlier and Windows 95.
 *
 *  To synchronize pointed object access, use outside lock.
 */
template <class X> 
class SharedPointer
{
public:
    typedef X element_type;

    explicit SharedPointer(X* p = 0) // allocate a new counter
        : counter_(0) 
    {
        if ( p ) 
        {
            counter_ = new counter(p); // we need to have a heap object
        }
    }

    ~SharedPointer()
    {
        release();
    }

    SharedPointer(const SharedPointer& r) throw()
    {
        acquire(r.counter_);
    }

    SharedPointer& operator=(const SharedPointer& r)
    {
        if (this != &r) 
        {
            release();
            acquire(r.counter_);
        }

        return *this;
    }

    X& operator*()  const throw()   
    {
        return *counter_->ptr;
    }

    X* operator->() const throw()   
    {
        return counter_->ptr;
    }

    X* Get()        const throw()   
    {
        return counter_ ? counter_->ptr : 0;
    }

    bool IsUnique() const
    {
        return (counter_ ? counter_->count == 1 : true);
    }

	template <class T>
	bool Cast( T*& ptr )
	{
		NS_ASSERT( counter_->ptr != 0 );

#ifdef _DEBUG
		ptr = dynamic_cast<T*>( counter_->ptr );	// Debug version
#else
		ptr = static_cast<T*>( counter_->ptr );		// Release version without checking
#endif

		return true;
	}

private:
    struct counter 
    {
        X*              ptr;
        volatile long  count;

        counter(X* p = 0, ulong c = 1) 
            : ptr(p)
        {
            InterlockedExchange( (volatile LONG*)&count, c );
        }
    };
    
    counter* counter_;

    void acquire(counter* c) throw()
    {
        // increment the count
        counter_ = c;

        if ( c ) 
        {
            InterlockedIncrement( (volatile LONG*)&counter_->count );
        }
    }

    void release()
    { 
        // decrement the count, delete if it is 0
        if ( counter_ ) 
        {
            ulong v = InterlockedDecrement( (volatile LONG*)&counter_->count );

            if ( v == 0 ) 
            {
                delete counter_->ptr;
                delete counter_;
            }

            counter_ = 0;
        }
    }
};

}  // gk 
