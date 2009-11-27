#pragma once 

#include <kcore/mem/Allocator.h>

namespace gk {

/**
 * @class Buffer 
 *
 * @brief Byte buffer allocated dynamically. 
 */
class Buffer 
{
public:
   	enum 
	{
   	   DEFAULT_BUFFER_SIZE = 128,  // initial buffer size
   	};
	
   	/**
	 * Create a Buffer from a chunk of memory.
	 *
	 * @param data The passed in buffer
	 * @param len  The length of the buffer
	 */
   	Buffer( byte *data, uint len );

    /**
	 * Create a Buffer by allocating DEFAULT_BUFFER_SIZE
	 *
	 * @param len The length of the buffer
	 */
   	Buffer( uint len = DEFAULT_BUFFER_SIZE );

	/**
	 * Copy constructor. Copies buffer.
	 */
	Buffer( const Buffer& rhs );

	/**
	 * Destroys buffer 
	 */
	~Buffer();

	/**
	 * Set the Buffer to point to a new chunk of memory. ownership is not taken.
	 *
	 * @param data The passed in buffer 
	 * @param len The length of the buffer
	 */
	void SetBuffer( byte* data, uint len );

	/**
	 * Set the Buffer to point to a new chunk of memory, indicating whether
	 * it should own the chunk or not.
	 *
	 * @param data The passed in buffer 
	 * @param len The length of the buffer
	 */
	void SetBuffer( byte* data, uint len, bool owner );

	/** 
	 * Attempts to resize the buffer.
	 * 
	 * @param newSize The new required buffer size
	 * @return true if it owns its own memory, false otherwise.
	 */
	bool Resize( uint newSize );

	/**
	 * Appends the specified buffer to the end of the byte buffer.
	 *
	 * @param data The buffer to append
	 * @param size The size of the buffer length to append
	 * @return false If it does not own its own memory.
	 */
	bool AppendBuffer( const byte* data, uint size );

	/**
	 * Appends the specified Buffer to the end of this byte buffer.
	 *
	 * @param rhs The buffer to append
	 * @return false If it does not own its own memory.
	 */
	bool AppendBuffer( const Buffer& rhs );

	/**
	 * Truncate front len data by memmove_s
	 *
	 * @param len The length to truncate from front
	 */
	void TruncateFront( uint len );

	/**
	 * Copies the current buffer into a newly allocated buffer 
	 * that the Buffer owns.
	 */
	void TakeOwnership();

	/**
	 * Clear buffer content without freeing the memory
	 */
	void Clear();

	/**
	 * Does this Buffer own its own memory?
	 */
	bool IsOwner() const;

	/**
	 * @return buffer size
	 */
	uint GetSize() const;

	/**
	 * @return buffer
	 */
	byte* GetBuffer() const;

protected:
	byte* 		m_data; 		// buffer
	uint 		m_size;			// buffer length 
	bool 		m_owner; 		// is owner?
};

inline
Buffer::Buffer( const Buffer& rhs )
{
	m_size   = rhs.m_size;
	m_data 	= rhs.m_data;
	m_owner 	= false;
}

inline
Buffer::~Buffer()
{
	if ( m_owner )
	{
		if ( m_data != 0 )
		{
			g_allocator.Free( m_data );
			m_data = 0;
			m_size = 0;
		}
	}
}

inline
void 
Buffer::SetBuffer( byte* data, uint len )
{
	K_ASSERT( data != 0 );
	K_ASSERT( len > 0 );

	if ( m_owner )
	{
		m_owner = false;

		if ( m_data != 0 )
		{
			g_allocator.Free( m_data );
			m_data = 0;
			m_size = 0;
		}
	}

	m_data = data;
	m_size = len;
}

inline
void 
Buffer::SetBuffer( byte* data, uint len, bool owner )
{
	SetBuffer( data, len );
	m_owner = owner;
}

inline
bool 
Buffer::Resize( uint newSize )
{
	K_ASSERT( newSize > 0 );

	if ( m_size >= newSize )
	{
		m_size = newSize;
	}
	else if( m_owner )
 	{
 		byte* p = (byte*)g_allocator.Alloc( newSize );

		if ( m_data != 0 )
		{
			K_ASSERT( m_size > 0 );

			// memcpy is faster than realloc call
			::memcpy( p, (void*)m_data, m_size );

			m_size = newSize;

			g_allocator.Free( m_data );
		}

		m_data = p;

		return true;
	}

	return false;
}

inline
bool 
Buffer::AppendBuffer( const byte* data, uint size )
{
	uint start = m_size;

	if ( !Resize( m_size + size ) )
	{
		return false;
	}

	::memcpy( m_data + start, data, size );

	return true;
}

inline
bool 
Buffer::AppendBuffer( const Buffer& rhs )
{
	return AppendBuffer( rhs.GetBuffer(), rhs.GetSize() );
}

inline
void 
Buffer::TruncateFront( uint len )
{
	K_ASSERT( m_data != 0 );
	K_ASSERT( m_size > 0 );
	K_ASSERT( len > 0 );
	K_ASSERT( len <= m_size );

	if ( len == m_size )
	{
		::memset( m_data, 0, m_size );

		return;
	}

	byte* p = (byte*)m_data;

    ::memmove_s( (void*)p, m_size, (void*)(p + len), m_size - len );
}

inline
void 
Buffer::TakeOwnership()
{
	if ( !m_owner )
	{
		byte* p = (byte*)g_allocator.Alloc( m_size );

		::memcpy( (void*)p, (void*)m_data, m_size );

		m_data 	= p;
		m_owner = true;
	}
}

inline
void 
Buffer::Clear()
{
	if ( m_data != 0 && m_size > 0 )
	{
		::memset( m_data, 0, m_size );
	}
}

inline
bool 
Buffer::IsOwner() const
{
	return m_owner;
}

inline
uint 
Buffer::GetSize() const
{
	return m_size;
}

inline
byte* 
Buffer::GetBuffer() const
{
	return m_data;
}

} // gk 


