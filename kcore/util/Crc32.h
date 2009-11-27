#pragma once

namespace gk { 

/**
 * @class Crc32
 *
 * @brief Calculates crc32 
 */
class Crc32
{
public:
    Crc32();
    ~Crc32();

    /**
	 * Gets hash value for the text
	 *
	 * @param src Source bytes to calculate CRC32
	 * @param len The length of source bytes
	 * @return crc value 
     */
    uint Calculate( const byte* src, uint len );
};

} // gk
