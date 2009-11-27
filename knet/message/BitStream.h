#pragma once 

#include <kcore/sys/Buffer.h>

namespace gk {

/**
 * @class BitStream
 *
 * BitStream to save bandwidth without much CPU overhead
 */
class BitStream : public Buffer
{
public:

   /**
	* Default to maximum write size being the size of the buffer.
	*
	* @param data The buffer provided outside
	* @param size The buffer size
	*/
	BitStream( byte* data, uint size ); 

	/**
	 * Optionally, specify a maximum write size.
	 *
	 * @param data The buffer provided outside
	 * @param size The buffer size
	 * @param maxWriteSize The limited size of write
	 */
	BitStream( byte* data, uint size, uint maxWriteSize); 

	/**
	 * Creates a resizable BitStream
	 *
	 * @param size The buffer size
	 */
	BitStream( uint size = Buffer::DEFAULT_BUFFER_SIZE ); 

	/**
	 * Sets the maximum read and write sizes for the BitStream.
	 *
	 * @param maxWriteSize The limited size of write 
	 */
	void SetMaxSizes( uint maxReadSize, uint maxWriteSize );

	/**
	 * Sets the maximum read and write bit sizes for the BitStream.
	 *
	 * @param maxReadBits The limited read bits
	 * @param maxWriteSize The limited write bits
	 */
	void SetMaxBitSizes( uint maxReadBits, uint maxWriteBits );

	/**
	 * Resets the read/write position to 0 and clears any m_error state.
	 */
	void Reset();

	/**
	 * Clears the m_error state from an attempted read or write overrun
	 */
	void Clear(); 

	/**
	 * Returns a pointer to the next byte in the BitStream 
	 * from the current bit position
	 *
	 * @return The raw internal bytes
	 */
	byte* GetBytePtr();

	/**
	 * Returns the current position in the stream rounded up to the next byte.
	 *
	 * @return The current byte position of operation
	 */
	uint GetBytePosition() const;

	/**
	 * Returns the current bit position in the stream
	 *
	 * @return The current bit position of operation
	 */
	uint GetBitPosition() const;

	/**
	 * Sets the position in the stream to the first bit of byte newPosition.
	 *
	 * @param newPosition The new byte position
	 */
	void SetBytePosition( uint newPosition );

	/**
	 * Sets the position in the stream to newBitPosition.
	 *
	 * @param newPosition The new bit position
	 */
	void SetBitPosition( uint newBitPosition );

	/**
	 * Advances the position in the stream by numBits.
	 *
	 * @param numBits The amount of bits to advance
	 */
	void AdvanceBitPosition( int numBits );

	/**
	 * @return The maximum readable bit position
	 */
	uint GetMaxReadBitPosition() const; 

	/**
	 * @return the number of bits that can be written into the BitStream without resizing
	 */
	uint GetBitSpaceAvailable() const;

	/**
	 * Pads the bits up to the next byte boundary with 0's.
	 */
	void ZeroToByteBoundary();

	/**
	 * Add len bytes to buffer 
	 */
	bool Accumulate( byte* v, uint len );

	/**
	 * Writes an unsigned integer value between 0 and 2^(bitCount - 1) into the stream.
	 *
	 * @param value The value to write
	 * @param bitCount The number of bits to use for the value
	 */
	void WriteInt( uint value, byte bitCount );

	/**
	 * Reads an unsigned integer value between 0 and 2^(bitCount - 1) from the stream.
	 *
	 * @param value The reference to the value to hold
	 * @param bitCount The number of bits to use for the value
	 */
	void ReadInt( uint& value, byte bitCount );

	/**
	 * Writes an unsigned integer value between 0 and 2^(bitCount -1) into the stream 
	 * at the specified position, without changing the current write position.
	 *
	 * @param value The value to write
	 * @param bitCount The number of bits to use for the value
	 * @param bitPosition The position of bit to start write 
	 */
	void WriteIntAt( uint value, byte bitCount, uint bitPosition);

	/**
	 * Writes a signed integer value between -2^(bitCount-1) and 2^(bitCount-1) - 1.
	 *
	 * @param value The value to write
	 * @param bitCount The number of bits to use for the value
	 */
	void WriteSignedInt( int value, byte bitCount);

	/**
	 * Reads a signed integer value between -2^(bitCount-1) and 2^(bitCount-1) - 1.
	 *
	 * @param value The reference to the value to hold
	 * @param bitCount The number of bits to use for the value
	 */
	void ReadSignedInt( int& v, byte bitCount );

	/**
	 * Write float 
	 *
	 * @param f The value to write
	 * @param bitCount The number of bits to use for the value
	 */
	void WriteFloat( float f, byte bitCount );

	/**
	 * Read float 
	 *
	 * @param f The reference to the value to hold
	 * @param bitCount The number of bits to use for the value
	 */
	void ReadFloat( float& f, byte bitCount );

	/**
	 * Writes/Reads a signed float from -1 to 1 inclusive, 
	 * using bitCount bits of precision.
	 *
	 * @param f The value to write
	 * @param bitCount The number of bits to use for the value
	 */
	void WriteSignedFloat( float f, byte bitCount );
	void ReadSignedFloat( float& f, byte bitCount );

	/**
	 * Writes/Reads bitCount bits into/from the stream from bitPtr.
	 */
	bool WriteBits( uint bitCount, const void *bitPtr );
	bool ReadBits( uint bitCount, void *bitPtr );

	/**
	 * Writes/Reads a Buffer into the stream.  
	 * The Buffer can be no larger than 1024 bytes in size.
	 */
	bool Write( const Buffer *theBuffer );
	bool Read( Buffer *theBuffer );

	/**
	 * Writes/Reads a single boolean flag (bit) into the stream, 
	 * and returns the boolean that was written.
	 */
	bool WriteFlag( bool val );
	bool ReadFlag();

	/**
	 * Writes/Reads boolean
	 */
	bool Write( bool value );
	bool Read( bool& value );

	/** 
	 * Writes/Reads short 
	 */
	void Write( short value );
	void Read( short& value );
	void Write( ushort value );
	void Read( ushort& value );

	/**
	 * Writes/Reads a string into the stream.
	 */
	void Write( const tstring& s );
	void Read( tstring& s );

	template <class T>
	bool Write( const T& v );

	template <class T>
	bool Read( T& v );

	/**
	 * Writes/Reads byte data into/from the stream.
	 */
	bool Write( uint numBytes, const void* in );
	bool Read( uint numBytes,  void* out );

	/**
	 * Sets the bit at position bitCount to the value of set
	 */
	bool SetBit( uint m_posBit, bool set );

	/**
	 * Tests the value of the bit at position bitCount.
	 */
	bool TestBit( uint m_posBit );

	/**
	 * Returns whether the BitStream writing has exceeded the write target size.
	 */
	bool IsFull() const;

	/**
	 * Returns whether the stream has generated an m_error condition due to reading 
	 * or writing past the end of the buffer.
	 */
	bool IsValid() const;

private:
	enum 
	{
   	   RESIZE_PAD = 128, 	// minimum padding bytes
   	};

	/**
	 * Resize buffer for the required bits 
	 */
	bool resizeBits( uint numBitsNeeded );

private:
   	uint m_posBit;            // The current bit position for reading/writing 
   	bool m_error;             // Flag set if overflow, or underflow
	uint m_maxReadBits;       // Max read bit position. Overflow check.
	uint m_maxWriteBits;      // Max write bit position. Overflow check.
};

inline
BitStream::BitStream( byte* data, uint size ) 
: Buffer( data, size ) 
{ 
	SetMaxSizes( size, size ); 
	Reset(); 
}

inline
BitStream::BitStream( byte* data, uint size, uint maxWriteSize) 
: Buffer( data, size ) 
{ 
	SetMaxSizes( size, maxWriteSize ); 
	Reset(); 
}

inline
BitStream::BitStream( uint size ) 
: Buffer( size ) 
{ 
	SetMaxSizes( GetSize(), GetSize() ); 
	Reset(); 
}

inline
void 
BitStream::SetMaxSizes( uint maxReadSize, uint maxWriteSize )
{
	K_ASSERT( maxReadSize > 0 );
	K_ASSERT( maxWriteSize > 0 );

	m_maxReadBits 	= maxReadSize << 3;
	m_maxWriteBits 	= maxWriteSize << 3;
}

inline
void 
BitStream::SetMaxBitSizes(uint maxReadBits, uint maxWriteBits )
{
	K_ASSERT( maxReadBits > 0 );
	K_ASSERT( maxWriteBits > 0 );

	m_maxReadBits 	= maxReadBits;
	m_maxWriteBits 	= maxWriteBits;
}

inline
void 
BitStream::Reset()
{
	m_posBit = 0;
	m_error  = false;
}

inline
void 
BitStream::Clear() 
{ 
	m_error = false; 

	Buffer::Clear(); 
}

inline
byte*  
BitStream::GetBytePtr()
{
	return GetBuffer() + GetBytePosition();
}

inline
uint 
BitStream::GetBytePosition() const
{
	return (m_posBit + 7) >> 3;
}

inline
uint 
BitStream::GetBitPosition() const
{
	return m_posBit;
}

inline
void 
BitStream::SetBytePosition( uint newPosition )
{
	m_posBit = newPosition << 3;
}

inline
void 
BitStream::SetBitPosition( uint newBitPosition )
{
   m_posBit = newBitPosition;
}

inline
void 
BitStream::AdvanceBitPosition( int numBits )
{
	SetBitPosition( GetBitPosition() + numBits);
}

inline
uint 
BitStream::GetMaxReadBitPosition() const 
{ 
	return m_maxReadBits; 
}

inline
uint 
BitStream::GetBitSpaceAvailable() const 
{ 
	return m_maxWriteBits - m_posBit; 
}

inline
void 
BitStream::ZeroToByteBoundary()
{
	if ( m_posBit & 0x7 )
	{
		WriteInt( 0, (byte)(( 8 - (m_posBit & 0x7) ) & 0xFF ) );
	}
}

inline
void 
BitStream::WriteInt( uint value, byte bitCount )
{
	K_ASSERT( bitCount > 0 );

   	WriteBits( bitCount, &value );
}

inline
void
BitStream::ReadInt( uint& value, byte bitCount )
{
	K_ASSERT( bitCount > 0 );

   	ReadBits( bitCount, &value );

   	if ( bitCount != 32 )
	{
		// clear the unrequired bits
      	value &= (1 << bitCount) - 1;
	}
}

inline
void 
BitStream::WriteIntAt( uint value, byte bitCount, uint bitPosition)
{
	K_ASSERT( bitCount > 0 );

	uint curPos = GetBitPosition();

	SetBitPosition( bitPosition );

	WriteInt( value, bitCount );

	SetBitPosition( curPos );
}

inline
void 
BitStream::WriteSignedInt( int value, byte bitCount)
{
	K_ASSERT( bitCount > 0 );

   if ( WriteFlag( value < 0 ) )
   {
      WriteInt( -value, bitCount - 1 );
   }
   else
   {
      WriteInt( value, bitCount - 1 );
   }
}

inline
void  
BitStream::ReadSignedInt( int& v, byte bitCount )
{
	K_ASSERT( bitCount > 0 );

	uint value = 0;

	if ( ReadFlag() )
	{
     	ReadInt( value, bitCount - 1 );

		v = value * -1;
	}
	else
	{
		ReadInt( value, bitCount - 1 );

		v = value;
	}
}

inline
void 
BitStream::WriteFloat( float f, byte bitCount )
{
	K_ASSERT( bitCount > 0 );

   	WriteInt( uint(f * ((1 << bitCount) - 1)), bitCount);
}

inline
void
BitStream::ReadFloat( float& f, byte bitCount )
{
	K_ASSERT( bitCount > 0 );

	uint v;

	ReadInt( v, bitCount );

	f = v/ float((1 << bitCount) - 1);
}

inline
void 
BitStream::WriteSignedFloat( float f, byte bitCount )
{
	K_ASSERT( bitCount > 0 );

	WriteSignedInt( int(f * ((1 << (bitCount - 1)) - 1)), bitCount );
}

inline
void
BitStream::ReadSignedFloat( float& f, byte bitCount )
{
	K_ASSERT( bitCount > 0 );

	int v;

   	ReadSignedInt( v, bitCount );
	
	f = v/ float( (1 << (bitCount - 1)) - 1 );
}

inline
bool 
BitStream::Write( bool value ) 
{ 
	WriteFlag( value ); 

	return !m_error; 
}

inline
bool 
BitStream::Read( bool& value ) 
{ 
	value = ReadFlag(); 

	return !m_error; 
}

inline
void 
BitStream::Write( const tstring& s )
{
	uint bitCount = ( sizeof( TCHAR ) * s.length() ) << 3; 

	K_ASSERT( bitCount < (1024 << 3) );

	WriteInt( bitCount, 10 );

	if ( bitCount > 0 )
	{
		WriteBits( bitCount, (void*)s.c_str() );
	}
}

inline
void 
BitStream::Read( tstring& s )
{
	uint bitCount = 0;

	ReadInt( bitCount, 10 );

	K_ASSERT( bitCount < (1024 << 3) );

	if ( bitCount > 0 )
	{
		uint byteCount = bitCount >> 3;

		byte* p = (byte*)g_allocator.Alloc( byteCount + 1 );

		::memset( p, 0, byteCount + 1 );

		ReadBits( bitCount, p ); // ReadBits modifies the pointer

		s = (TCHAR*)p; // read in TCHAR

		g_allocator.Free( p ); // ownership transferred
	}
}

inline
void 
BitStream::Write( short value )
{
	Write( 2, &value );
}

inline
void 
BitStream::Read( short& value )
{
	Read( 2, &value );
}

inline
void 
BitStream::Write( ushort value )
{
	Write( 2, &value );
}

inline
void 
BitStream::Read( ushort& value )
{
	Read( 2, &value );
}

template <class T>
inline
bool
BitStream::Write( const T& v )
{
	return Write( sizeof( T ), &v );
}

template <class T>
inline
bool 
BitStream::Read( T& v )
{
	return Read( sizeof( T ), &v );
}

inline
bool 
BitStream::Write( uint numBytes, const void* in )
{
	return WriteBits( numBytes << 3, in );
}

inline
bool 
BitStream::Read( uint numBytes,  void* out )
{
	return ReadBits( numBytes << 3, out );
}

inline
bool 
BitStream::IsFull() const
{ 
	return m_posBit > (GetSize() << 3); 
}

inline
bool 
BitStream::IsValid() const
{ 
	return !m_error; 
}

} // gk 
