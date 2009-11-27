#pragma once 

namespace gk { 

/**
 * @class ContextKey 
 * 
 * Used to remember dispatching context in message
 */
class ContextKey 
{
public:
	ContextKey();
	~ContextKey();

	/**
	 * Append short value to the key  
	 *
	 * @param v The value to append
	 */
	void Update( short v );

	/**
	 * Append unsigned short value to the key  
	 *
	 * @param v The value to append
	 */
	void Update( ushort v );

	/**
	 * Append int value to the key  
	 *
	 * @param v The value to append
	 */
	void Update( int v );

	/**
	 * Append unsigned int value to the key  
	 *
	 * @param v The value to append
	 */
	void Update( uint v );

	/**
	 * Append string value to the key  
	 *
	 * @param v The value to append
	 */
	void Update( const tstring& v );

	/**
	 * Clear the key
	 */
	void Clear();

	/**
	 * Get key updated so far
	 */
	const tstring& GetKey();

	/**
	 * Copy construction 
	 */
	ContextKey( const ContextKey& rhs );

	/**
	 * Copy operator
	 */
	ContextKey& operator=( const ContextKey& rhs );

private:
	tstring m_key;
};

inline
ContextKey::ContextKey( const ContextKey& rhs )
{
	m_key = rhs.m_key;
}

inline
ContextKey& 
ContextKey::operator=( const ContextKey& rhs )
{
	m_key = rhs.m_key;

	return *this;
}


} // gk
