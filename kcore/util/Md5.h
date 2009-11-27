#pragma once

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

#include <string>

namespace gk {

/**
 * @class Md5
 *
 * Calculates MD5 hash 
 */
class Md5
{
public:
	Md5();

	/**
	 * Calculates MD5 
	 *
	 * @param src String to calculate hash 
	 * @return MD5 hash string
	 */
	const tstring& Calculate(const tstring& src );

	/**
	 * Calculates MD5 
	 *
	 * @param src Bytes to calculate hash 
	 * @param len The byte length
	 * @return MD5 hash string
	 */
	const tstring& Calculate(const byte* src, uint len);

	/**
	 * Get current hash 
	 *
	 * @return The current calculated hash
	 */
	const tstring& GetHash() const { return m_hash; }

	/**
	 * Get current hash
	 * @return The current calculated hash
	 */
	const byte* GetRawHash() const { return m_rawHash; }

private:
	tstring	m_hash;
	byte    m_rawHash[16];
};

} // gk
