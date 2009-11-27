#include "stdafx.h"

#include <kcore/corebase.h>
#include <knet/message/BitStream.h>

#include <math.h>

#pragma warning( disable: 4244 )	// conversion from int to byte 

namespace gk {

bool
BitStream::Accumulate( byte* v, uint len )
{
	K_ASSERT( (m_posBit & 0x7) == 0 );

	uint bitCount = len << 3;

	if ( (bitCount + m_posBit) > m_maxWriteBits )
	{
      	if ( !resizeBits( bitCount + m_posBit - m_maxWriteBits ) )
	  	{
         	return false;
	  	}
   	}

	byte* destPtr = GetBuffer() + ( m_posBit >> 3 );

	::memcpy( destPtr, v, len );

	m_posBit += bitCount;

	return true;
}

bool 
BitStream::WriteBits( uint bitCount, const void *bitPtr )
{
	if ( !bitCount )
   	{
      	return true;
	}

	if ( bitCount + m_posBit > m_maxWriteBits )
	{
      	if ( !resizeBits( bitCount + m_posBit - m_maxWriteBits ) )
	  	{
         	return false;
	  	}
   	}

   	uint upShift  	= m_posBit & 0x7;
   	uint downShift 	= 8 - upShift;

   	const byte *sourcePtr = (byte *) bitPtr;
   	byte *destPtr = GetBuffer() + (m_posBit >> 3);

   	// if this write is for <= 1 byte, and it will all fit in the
   	// first dest byte, then do some special masking.
   	if ( downShift >= bitCount )
   	{
      	byte mask = (byte)( ( ((1 << bitCount) - 1) << upShift ) & 0xFF );
      	*destPtr = (*destPtr & ~mask) | ((*sourcePtr << upShift) & mask);
      	m_posBit += bitCount;

      	return true;
   	}

   	// check for byte aligned writes -- this will be
   	// much faster than the shifting writes.
   	if ( !upShift ) 
   	{
      	m_posBit += bitCount;

      	for( ; bitCount >= 8; bitCount -= 8 )
		{
         	*destPtr++ = *sourcePtr++;
		}

      	if ( bitCount )
      	{
         	byte mask = (1 << bitCount) - 1;
         	*destPtr = (*sourcePtr & mask) | (*destPtr & ~mask);
      	}

      	return true;
   	}

   	// the write destination is not byte aligned.
   	byte sourceByte;
   	byte destByte = *destPtr & (0xFF >> downShift);
   	byte lastMask = 0xFF >> (7 - ((m_posBit + bitCount - 1) & 0x7));

   	m_posBit += bitCount;

   	for( ; bitCount >= 8; bitCount -= 8 )
   	{
      	sourceByte = *sourcePtr++;
      	*destPtr++ = destByte | (sourceByte << upShift);
      	destByte = sourceByte >> downShift;
   	}

   	if ( bitCount == 0 )
   	{
      	*destPtr = (*destPtr & ~lastMask) | (destByte & lastMask);

      	return true;
   	}

   	if ( bitCount <= downShift )
   	{
      	*destPtr = (*destPtr & ~lastMask) | ((destByte | 
				   (*sourcePtr << upShift)) & lastMask);

      	return true;
   	}

   	sourceByte = *sourcePtr;
   	*destPtr++ = destByte | (sourceByte << upShift);
   	*destPtr   = (*destPtr & ~lastMask) | ((sourceByte >> downShift) & lastMask);

   	return true;
}

bool 
BitStream::ReadBits( uint bitCount, void *bitPtr )
{
   	if ( !bitCount )
	{
      	return true;
	}

   	if ( bitCount + m_posBit > m_maxReadBits )
   	{
      	m_error = true;
      	return false;
   	}

   	byte *sourcePtr = GetBuffer() + (m_posBit >> 3);
   	uint byteCount  = (bitCount + 7) >> 3;

   	byte *destPtr  = (byte *) bitPtr;
   	uint downShift = m_posBit & 0x7;
   	uint upShift   = 8 - downShift;

   	if ( !downShift )
   	{
      	while ( byteCount--)
        	*destPtr++ = *sourcePtr++;

      	m_posBit += bitCount;
      	return true;
   	}

   	byte sourceByte = *sourcePtr >> downShift;
   	m_posBit       += bitCount;

   	for ( ; bitCount >= 8; bitCount -= 8 )
   	{
      	byte nextByte = *++sourcePtr;
     	*destPtr++ = sourceByte | (nextByte << upShift);
      	sourceByte = nextByte >> downShift;
   	}

   	if ( bitCount )
   	{
      	if ( bitCount <= upShift )
      	{
         	*destPtr = sourceByte;
         	return true;
      	}

      	*destPtr = sourceByte | ( (*++sourcePtr) << upShift);
  	}

   	return true;
}

bool 
BitStream::SetBit(uint bitCount, bool set)
{
   	if ( bitCount >= m_maxWriteBits )
   	{
      	if ( !resizeBits(bitCount - m_maxWriteBits + 1 ) )
		{
         	return false;
		}
   	}

	if ( set )
	{
		*(GetBuffer() + (bitCount >> 3)) |= (1 << (bitCount & 0x7));
	}
	else
	{
		*(GetBuffer() + (bitCount >> 3)) &= ~(1 << (bitCount & 0x7));
	}

	return true;
}

bool BitStream::TestBit(uint bitCount)
{
   	return (*(GetBuffer() + (bitCount >> 3)) & (1 << (bitCount & 0x7))) != 0;
}

bool BitStream::WriteFlag(bool val)
{
	if ( m_posBit + 1 > m_maxWriteBits )
	{
		if ( !resizeBits(1) )
		{
			return false;
		}
	}
	  
	if ( val )
	{
		*(GetBuffer() + (m_posBit >> 3)) |= (1 << (m_posBit & 0x7));
	}
	else
	{
		*(GetBuffer() + (m_posBit >> 3)) &= ~(1 << (m_posBit & 0x7));
	}

	m_posBit++;

	return (val);
}

bool 
BitStream::ReadFlag()
{
   if ( m_posBit > m_maxReadBits )
   {
      m_error = true;

      return false;
   }

   uint mask = 1 << (m_posBit & 0x7);

   bool ret = (*(GetBuffer() + (m_posBit >> 3)) & mask) != 0;

   m_posBit++;

   return ret;
}

bool BitStream::Write(const Buffer *theBuffer)
{
	K_ASSERT( theBuffer != 0 );

	uint size = theBuffer->GetSize();

	if ( size > 1023 )
	{
		return false;
	}

	K_ASSERT( size > 0 );

	WriteInt(size, 10);

	return Write( size, theBuffer->GetBuffer() );
}

bool BitStream::Read( Buffer *theBuffer )
{
	uint size = 0;

	ReadInt( size, 10 );

	theBuffer->TakeOwnership();
	theBuffer->Resize(size);

	return Read( size, theBuffer->GetBuffer() );
}

bool 
BitStream::resizeBits( uint newBits )
{
	K_ASSERT( (m_maxReadBits  >> 3) == m_size );
	K_ASSERT( (m_maxWriteBits >> 3) == m_size );

	uint newSize = ((m_maxWriteBits + newBits + 7) >> 3);

	if ( !Resize( newSize ) )
	{
		m_error = true;

		return false;
	}

	m_maxReadBits  = newSize << 3;
	m_maxWriteBits = newSize << 3;

   	return true;
}
 
} // gk
