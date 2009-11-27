#pragma once

namespace gk 
{
/**
 * @class Atomic
 *
 * @brief locked access to a flag data.
 * 
 * NOTE: T must be compatible with long type.
 */
template <typename T>
class Atomic 
{
public:
    Atomic()
	: v_()
	{
	}

	/**
	 * Constructs with a default value 
	 *
	 * @param v Initial value
	 */
    explicit Atomic( const T& v )
	{
		::InterlockedExchange( (volatile long*)&v_, v );
	}

    ~Atomic()
	{
	}

    /**
     * Copy construction 
     */
    Atomic( const Atomic<T>& rhs )
	{
		T v = (T)rhs;

		::InterlockedExchange( (volatile long*)&v_, v );
	}

	/**
	 * Atomic add
	 *
	 * @param v Value to add
	 * @return Reference to this object
	 */
	const Atomic& Add( const T& v )
	{
		::InterlockedExchangeAdd( (volatile long*)&v_, v );

		return *this;
	}

	/**
	 * Atomic increment by 1
	 *
	 * @return Reference to this object
	 */
	const Atomic& Inc()
	{
		::InterlockedIncrement( (volatile long*)&v_ );

		return *this;
	}

	/**
	 * Atomic decrement
	 *
	 * @return Reference to this object
	 */
	const Atomic& Dec()
	{
		::InterlockedDecrement( (volatile long*)&v_ );

		return *this;
	}
	
    /**
     * Assignment with a raw value 
	 *
	 * @param v Value to assign
	 * @return Reference to this object
     */
    const Atomic& operator=( const T& v )
	{
		::InterlockedExchange( (volatile long*)&v_, v );

		return *this;
	}

    /**
     * Type conversion to T. Because of Locking, this cannot be const
     */
    operator T () const
	{
		return (T)::InterlockedCompareExchange( (volatile long*)&v_, v_, v_ );
	}	

    /**
     * Comparison of raw values
	 *
	 * @param v Value to compare
	 * @return true if the values are same
     */
    bool operator==( const T& v)
	{
		T ev = ::InterlockedCompareExchange( (volatile long*)&v_, v_, v_ );

		return ( ev == v );
	}

private:
    volatile T v_;
};

/**
 * @class Atomic<bool>
 *
 * Specialization of Atomic for bool type. 
 * Reason: Warning can ocurr during type casting.
 */
template <>
class Atomic<bool> 
{
public:
	Atomic()
		: v_( false )
	{
	}

	explicit Atomic( const bool& v )
	{
		::InterlockedExchange( (volatile long*)&v_, v );
	}

	~Atomic()
	{
	}

	/**
	 * Copy construction 
	 */
	Atomic( const Atomic<bool>& rhs )
	{
		bool v = (bool)rhs;

		::InterlockedExchange( (volatile long*)&v_, (long)v );
	}

	/**
	 * Assignment with a raw value 
	 */
	const Atomic& operator=( const bool& v )
	{
		::InterlockedExchange( (volatile long*)&v_, (long)v );

		return *this;
	}

	operator bool () const
	{
		return ( ::InterlockedCompareExchange( (volatile long*)&v_, v_, v_ ) != 0 );
	}

	/**
	 * Comparison of raw values
	 */
	bool operator==( const bool& v)
	{
		bool ev = ( ::InterlockedCompareExchange( (volatile long*)&v_, v_, v_ ) != 0 );

		return ( ev == v );
	}

private:
	volatile bool v_;	
};

} // gk 
