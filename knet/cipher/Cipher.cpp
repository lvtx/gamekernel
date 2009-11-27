#include "stdafx.h"

#pragma warning( disable: 4512)
#pragma warning( disable: 4100)
#pragma warning( disable: 4189)
#pragma warning( disable: 4127)
#pragma warning( disable: 4244)
#pragma warning( disable: 4516)
#pragma warning( disable: 4505) 
#pragma warning( disable: 4706) 

// it seems that code generation comes last. 
// so disable warning must be kept.

#include <kcore/corebase.h>
#include <knet/cipher/Cipher.h>

#include <kcore/sys/Logger.h>
#include <kcore/sys/DateTime.h>
#include <modes.h>
#include <aes.h>
#include <des.h>
#include <tea.h>
#include <randpool.h>

namespace gk
{
// A simple, but great implementation by June.
template<typename C>
class ConcreteCipherCodec : public AbstractCipherCodec
{
public:
	ConcreteCipherCodec( const byte *key, unsigned int length, const byte *iv )
		: enc_(key, length)
		  , dec_(key, length)
	{
		::memcpy( iv_, iv, C::BLOCKSIZE );
	}

	virtual ~ConcreteCipherCodec() {}

	bool Encrypt( const byte* inBuf, uint inLen, byte* outBuf, uint& outLen )
	{
		enc_.ProcessData(outBuf, inBuf, inLen);
		outLen = inLen;
		return true;
	}

	bool Decrypt( const byte* inBuf, uint inLen, byte* outBuf, uint& outLen )
	{
		dec_.ProcessData(outBuf, inBuf, inLen);
		outLen = inLen;
		return true;
	}

	uint GetBlockSize() { return C::BLOCKSIZE; }

	uint GetMandatoryBlockSizeEnc() { return enc_.MandatoryBlockSize(); }
	uint GetMandatoryBlockSizeDec() { return dec_.MandatoryBlockSize(); }

private:
	typename CryptoPP::ECB_Mode<C>::Encryption enc_;
	typename CryptoPP::ECB_Mode<C>::Decryption dec_;

	byte iv_[C::BLOCKSIZE];
};

} // namespace gk

using namespace gk;
using namespace CryptoPP;

Cipher::Cipher()
    : m_established(false)
    , m_algorithm(ALG_INVALID)
    , m_keyLength(0)
    , m_codec(0)
{
}

Cipher::~Cipher()
{
    Fini();
}

bool
Cipher::Init()
{
	byte temp[LEN_CHALLENGE];

	// TODO: use random something with high quality random in CryptoPP
	
	const unsigned int SEEDSIZE = 16;
	byte seed[ SEEDSIZE ];

	DateTime now;

	int y = now.GetYear();
	int d = now.GetDayOfYear();
	int h = now.GetHour();
	int m = now.GetSecond() + now.GetMinute() + now.GetMilliSecond();
		
	seed[0] = (byte)( ( y >> 24) & 0xFF );
	seed[1] = (byte)( ( y >> 16) & 0xFF );
	seed[2] = (byte)( ( y >> 8) & 0xFF );
	seed[3] = (byte)( ( y & 0xFF ) );
	seed[4] = (byte)( ( d >> 24) & 0xFF );
	seed[5] = (byte)( ( d >> 16) & 0xFF );
	seed[6] = (byte)( ( d >> 8) & 0xFF );
	seed[7] = (byte)( ( d & 0xFF ) );
	seed[8] = (byte)( ( h >> 24) & 0xFF );
	seed[9] = (byte)( ( h >> 16) & 0xFF );
	seed[10] = (byte)( ( h >> 8) & 0xFF );
	seed[11] = (byte)( ( h & 0xFF ) );
	seed[12] = (byte)( ( m >> 24) & 0xFF );
	seed[13] = (byte)( ( m >> 16) & 0xFF );
	seed[14] = (byte)( ( m >> 8) & 0xFF );
	seed[15] = (byte)( ( m & 0xFF ) );

	const unsigned int BLOCKSIZE = 128;

	RandomPool rng;
	rng.Put( seed, SEEDSIZE );

	for ( int i=0; i<2; ++i )
	{
		rng.GenerateBlock( (byte*)(temp + i*BLOCKSIZE), BLOCKSIZE );
	}
	
	return Init( temp, LEN_CHALLENGE, false );
}

bool 
Cipher::Init( const byte* challenge, uint length, bool obfuscated )
{    
	K_ASSERT( length == LEN_CHALLENGE );

	m_obfuscated = obfuscated;
	
	::memcpy( m_challeng, challenge, length );

    byte temp[LEN_KEYARRAY];

    initAlgorithm( challenge );
    initKeys( challenge, temp );
    initCodec( temp );

    return true;
}

void 
Cipher::Fini()
{
    if ( m_codec )
    {
        delete m_codec;
    }

    m_established 	= false;
    m_algorithm 	= ALG_INVALID;
    m_keyLength 	= 0;
    m_codec 		= 0;
}

bool 
Cipher::Encrypt( BitStream& bs )
{
	K_ASSERT( IsEstablished() );

    if ( !IsEstablished() )
	{
        return false;
	}

	// ECB mode requires that the input buffer length is aligned to the block size
    uint x = bs.GetBytePosition() % m_codec->GetBlockSize();

    if ( x )
    {
        byte padding[64] = { 0x00, };
        bs.Write( (ushort)(m_codec->GetBlockSize() - x), padding );
    }
    
    const byte* in      = bs.GetBuffer();
    uint        inLen   = bs.GetBytePosition();
    byte*       out     = bs.GetBuffer();
    uint        outLen  = inLen;

    try
    {
	    if ( !m_codec->Encrypt(in, inLen, out, outLen) )
        {
            return false;
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool 
Cipher::Decrypt( BitStream& bs )
{
	K_ASSERT( IsEstablished() );

    if ( !IsEstablished() )
	{
        return false;
	}

	const byte* in      = bs.GetBuffer();
    uint        inLen   = bs.GetBytePosition();
    byte*       out     = bs.GetBuffer();
    uint        outLen  = inLen;

    try
    {     
        int remainder = inLen % m_codec->GetBlockSize();

        inLen = inLen - remainder;
        outLen = inLen;

        if ( !m_codec->Decrypt(in, inLen, out, outLen) )
        {
            return false;
        }
    }
    catch (...)
    {
        return false;
    }

	return true;
}

bool 
Cipher::Decrypt( BitStream& bs, uint skip, uint len )
{
	K_ASSERT( IsEstablished() );

    if ( !IsEstablished() )
	{
        return false;
	}

	K_ASSERT( skip <= len );

	const byte* in      = (const byte*)( bs.GetBuffer() + skip );
    uint        inLen   = len;
    byte*       out     = (byte*)( bs.GetBuffer() + skip );
    uint        outLen  = inLen;

    try
    {     
        int remainder = inLen % m_codec->GetBlockSize();

        inLen = inLen - remainder;
        outLen = inLen;

        if ( !m_codec->Decrypt(in, inLen, out, outLen) )
        {
            return false;
        }
    }
    catch (...)
    {
        return false;
    }

	return true;
}

void 
Cipher::initAlgorithm( const byte* challenge )
{
    K_ASSERT( m_algorithm == ALG_INVALID );

    int a = 0;
    int i = (int)challenge[challenge[9] % LEN_CHALLENGE];
    i %= 4;

    for ( ; i < LEN_CHALLENGE; i += 4)
    {
        a ^= (int)challenge[i];

		if ( m_obfuscated )
		{
			// add very long code which runs very fast 
			// and very difficult to analyze
		}
    }

    int b = (a & 0x0f0) >> 4;
    a = ((a << 4) & 0x0f0) | b;
    a %= (ALG_END - 1);
    ++a;

    LOG( FT_DEBUG, _T("Cipher::initAlgorithm> Selected Cryptographic Algorithm %d"), a);

	m_algorithm = (Algorithm)a;
}

void 
Cipher::initKeys( const byte* challenge, byte* temp )
{
    allocateKeyBuffer();

    K_ASSERT( m_keyLength > 0 );

    int i, j, k, l;

    for (i = 0; i < 128; ++i)
    {
        temp[i] = challenge[challenge[i] % LEN_CHALLENGE];
    }

	if ( m_obfuscated )
	{
		// add very long code which runs very fast 
		// and very difficult to analyze
		// obfuscated assembly routines which cannot be optimized away 
		// will be useful
	}
    
    int a = (m_algorithm % 7) + 3;

    for (i = 0; i < 4; ++i)
    {
        k = 0;
        for (j = 127; j >= 0; --j)
        {
            k ^= temp[j];
            l = k % a;

            if (k % 1)
            {
                if (temp[j] <= (255 - (l * 2))) { temp[j] = (byte)(temp[j] + l); }
            }
            else
            {
                if (temp[j] >= (l * 2)) { temp[j] = (byte)(temp[j] - l); }
            }
        }
    }

    for (i = 0; i < 64; ++i)
    {
        j = 64 + (temp[i] % 64);
        byte t = temp[i];
        temp[i] = temp[j];
        temp[j] = t;

		LOG( FT_DEBUG_FLOW, _T("Cipher::initKey> %d %x"), i, temp[i] );
    }

    ::memcpy( m_key, temp, m_keyLength );
}

void 
Cipher::allocateKeyBuffer()
{
    K_ASSERT( m_algorithm != ALG_INVALID );
    K_ASSERT( m_keyLength == 0 );

    switch ( m_algorithm )
    {
    case ALG_AES:
        m_keyLength = AES::DEFAULT_KEYLENGTH;
        break;
    case ALG_DES:
        m_keyLength = DES::DEFAULT_KEYLENGTH;
        break;
    case ALG_TEA:
        m_keyLength = TEA::DEFAULT_KEYLENGTH;
        break;
    default:
        K_ASSERT( !_T("Should never reach here - invalid cipher algorithm") );
    }

	::memset( m_key, 0, LEN_KEYARRAY );

	K_ASSERT( m_keyLength <= LEN_KEYARRAY );
}

void 
Cipher::initCodec( const byte* temp )
{
    K_ASSERT( m_algorithm != ALG_INVALID );
    K_ASSERT( m_keyLength > 0 );
    K_ASSERT( !m_codec );

    byte iv[LEN_KEYARRAY];

    for (int i = 0; i < LEN_KEYARRAY; ++i)
    {
        iv[i] = temp[(LEN_KEYARRAY - 1) - i];
    }

	if ( m_obfuscated )
	{
		// add very long code which runs very fast 
		// and very difficult to analyze
	}

	switch (m_algorithm)
	{
	case ALG_AES:
		m_codec = new ConcreteCipherCodec<AES>( m_key, m_keyLength, iv );
		break;
	case ALG_DES:
		m_codec = new ConcreteCipherCodec<DES>( m_key, m_keyLength, iv );
		break;
	case ALG_TEA:
		m_codec = new ConcreteCipherCodec<TEA>( m_key, m_keyLength, iv );
		break;
	default:
		K_ASSERT( !_T("Should never reach here - invalid cipher algorithm") );
	}
}

uint
Cipher::GetBlockSize()
{
    return ( m_codec ? m_codec->GetBlockSize() : 0  );
}

