#pragma once 

#include <kcore/mem/AllocatorAware.h>
#include <knet/message/BitStream.h>

namespace gk 
{
class AbstractCipherCodec;

/**
 * @class Cipher
 * 
 * @brief Thin wrapper for cryptographic implementations.
 *
 * By adding/removing algorithms, client hackers will spend more time.
 * By adding long running client side dummy code, it will get more complicated. 
 */
class Cipher
{
public:
	// Add more algorithms when it is required to add more algorithm selection 
	enum Algorithm
	{
		  ALG_BEGIN = 0
		, ALG_INVALID = ALG_BEGIN
		, ALG_TEA       // TEA
		, ALG_AES       // AES (Rijndael)
		, ALG_DES       // DES
		, ALG_END
   	};

	enum 
	{
		  LEN_CHALLENGE = 256
		, LEN_KEYARRAY = 128
	};

public:
	Cipher();
	~Cipher();

	/**
	 * Init on server side to build a challenge message 
	 */
	bool Init();

	/**
	 * Initialize the cipher context according to the given challenge vector.
	 */
	bool Init( const byte* challenge, uint len, bool obfuscated );

	/**
	 * Encrypt a bit stream.
	 *
	 * The bitStream parameter is used for both input/output.
	 * When this is called, bitStream contains the plaintext message.
	 * But when the function returns, it contains the ciphertext message.
	 *
	 * @param bitStream [in,out] plaintext(ciphertext on return) bit stream
	 * 
	 * @returns true if the decryption has been completed successful, false otherwise
	 */
	bool Encrypt( BitStream& bitStream );

	/**
	 * Decrypt a bit stream according to the given cipher context.
	 *
	 * The bitStream parameter is used for both input/output.
	 * When this is called, bitStream contains the ciphertext message.
	 * But when the function returns, it contains the plaintext message.
	 *
	 * @param bitStream [in,out] ciphertext(plaintext on return) bit stream
	 * 
	 * @returns true if the decryption has been completed successful, false otherwise
	 */
	bool Decrypt( BitStream& bitStream );
	bool Decrypt( BitStream& bitStream, uint skip, uint len );

	/**
	 * Clean up
	 */
	void Fini();

	void SetEstablished( bool flag = true ) { m_established = flag; }
	bool IsEstablished()                    { return m_established; }

	Algorithm GetAlgorithm()                { return m_algorithm; }

	const byte* GetChallenge() 				{ return m_challeng; }
	const byte* GetEncryptionKey()			{ return m_key; }
	const int GetEncryptionKeyLength()		{ return m_keyLength; }

	// For debugging
	uint GetBlockSize();

private:
	void initAlgorithm( const byte* challenge );
	void initKeys( const byte* challenge, byte* keyArray );
	void allocateKeyBuffer();
	void initCodec( const byte* keyArray );

private:
	bool 			m_established;
	Algorithm       m_algorithm;
	unsigned int    m_keyLength;
	byte           	m_key[LEN_KEYARRAY];
	byte 			m_challeng[LEN_CHALLENGE];
	bool 			m_obfuscated;

	AbstractCipherCodec* m_codec;
};

class AbstractCipherCodec : public AllocatorAware
{
public:
	virtual ~AbstractCipherCodec() {}

	virtual bool Encrypt(const byte* inBuf, uint inLen, byte* outBuf, uint& outLen) = 0;
	virtual bool Decrypt(const byte* inBuf, uint inLen, byte* outBuf, uint& outLen) = 0;

	virtual uint GetBlockSize() = 0;
	virtual uint GetMandatoryBlockSizeEnc() = 0;
	virtual uint GetMandatoryBlockSizeDec() = 0;
};

} // gk

#if defined(_DEBUG)
#pragma comment(lib, "cryptlib")
#else
#pragma comment(lib, "cryptlib")
#endif 


