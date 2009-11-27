#pragma once

#include <string>
#include <vector>

namespace gk 
{
/**
 * @class StringUtil
 *
 */
class StringUtil
{
public:
    /** 
     * Splits s with delimiter and put result into rv 
     */
    static void Split( const tstring& s, 
                       const tstring& delimiter, 
                       std::vector<tstring>& rv );

    /**
     * Trim white spaces from s and put result into r
     */
    static void Trim( const tstring& s, tstring& r );
    static void Trim( tstring& s );

	/**
	 * Convert from string to an integer
	 */
    static int ToInteger( const tstring& s );

	/**
	 * Convert from string to a double
	 */
    static double ToDouble( const tstring& s );

	/**
	 * Convert to a string from integer
	 */
	static const tstring FromInteger( int v );

	/**
	 * Convert to lower characters
	 */
	static void ToLower( const tstring& s, tstring& out );

	/**
	 * Convert to upper characters
	 */
	static void ToUpper( const tstring& s, tstring& out );

	/**
	 * Convert to system code (UNICODE or MBCS)
	 */
	static void ConvertToSystemCode( const char* s, tstring& out );

	/**
	 * Convert to MBCS code
	 */
	static void ConvertToMBCS( const tstring& s, std::string& out );

	/**
	 * Convert to a string from integer
	 */
	static tstring ToString( int v );

	/**
	 * Get ip part from ip:port string
	 */
    static const tstring GetIp( const tstring& s );

	/**
	 * Get port part from ip:port string 
	 */
    static int GetPort( const tstring& s );
};

} // gk 

