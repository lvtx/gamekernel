#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/util/StringUtil.h>

namespace gk { 

void 
StringUtil::Split( const tstring& s, 
				   const tstring& delimiter, 
				   std::vector<tstring>& rv )
{
    tstring l = s;

    tstring::size_type pos = l.find_first_of( delimiter );

    while ( pos != tstring::npos )
    {
        rv.push_back( l.substr( 0, pos ) );

        l = l.substr( pos + 1 ); // how will this work?

        pos = l.find_first_of( delimiter );
    }

    if ( l.length() > 0 )
    {
        rv.push_back( l );
    }
}

void 
StringUtil::Trim( const tstring& s, tstring& r )
{
   tstring::size_type first = s.find_first_not_of( _T(" \t\r\n" ) );
   tstring::size_type last  = s.find_last_not_of( _T(" \t\r\n" ) );

   r = s.substr( first, last - first + 1 ); // can this work? test!
}

void 
StringUtil::Trim( tstring& s )
{
    tstring::size_type first = s.find_first_not_of( _T(" \t\r\n" ) );
    tstring::size_type last  = s.find_last_not_of( _T(" \t\r\n" ) );

    if ( first != 0 )
    {
        s.erase( 0, first );
    }

    if ( last != tstring::npos )
    {
        s.erase( last + 1 );
    }
}

int 
StringUtil::ToInteger( const tstring& s )
{
    return _tstoi( s.c_str() );
}

const tstring
StringUtil::FromInteger( int v )
{
	TCHAR c[128];

	::memset( c, 0, 128 );

	::_itot_s( v, c, 128, 10 );

	tstring o;

	o.append( c );

	return o;
}

double
StringUtil::ToDouble( const tstring& s )
{
    return _tstof( s.c_str() );
}

void 
StringUtil::ToLower( const tstring& s, tstring& out )
{
	TCHAR c[2];
	c[1] = _T('\0');

	const TCHAR* cs = s.c_str();

	for ( uint i=0; i<s.length(); ++i )
	{
		c[0] = (TCHAR)_totlower( cs[i] );

		out.append( c );
	}
}

void 
StringUtil::ToUpper( const tstring& s, tstring& out )
{
	TCHAR c[2];
	c[1] = _T('\0');

	const TCHAR* cs = s.c_str();

	for ( uint i=0; i<s.length(); ++i )
	{
		c[0] = (TCHAR)_totupper( cs[i] );

		out.append( c );
	}
}

void 
StringUtil::ConvertToSystemCode( const char* s, tstring& out )
{
	out.clear();

#if defined(_UNICODE) || defined(UNICODE) 

	LPWSTR np;

	int slen = std::min<int>( 256, (int)strlen( s ) );

	int wlen = MultiByteToWideChar(CP_ACP, 0, s, slen, NULL, NULL);

	np = (LPWSTR)_alloca( sizeof( WCHAR ) * ( wlen + 1 ) ); // 스택에서 할당

	::memset( np, 0, (wlen + 1 ) * sizeof( WCHAR ) );

	MultiByteToWideChar(CP_ACP, 0, s, slen, np, wlen);

	out.append( np );

#else
	out.append( s );
#endif 
}

void 
StringUtil::ConvertToMBCS( const tstring& s, std::string& out )
{
	out.clear();

#if defined(_UNICODE) || defined(UNICODE) 

	LPSTR np;

	int slen = std::min<int>( 256, (int)_tcslen( s.c_str() ) );

	int wlen = WideCharToMultiByte(CP_ACP, 0, s.c_str(), slen, NULL, 0, NULL, NULL);

	np = (LPSTR)_alloca( sizeof( CHAR ) * ( wlen + 1 ) );	// 스택에서 할당

	::memset( np, 0, (wlen + 1 ) * sizeof( CHAR ) );

	WideCharToMultiByte(CP_ACP, 0, s.c_str(), slen, np, wlen, NULL, NULL );

	out.append( np );
#else
	out.append( s );
#endif 

}

tstring 
StringUtil::ToString( int v )
{
	TCHAR buf[128];

	::memset( buf, 0, 128 * sizeof( TCHAR ) );

	_stprintf_s( buf, 128, _T("%d"), v );

	tstring s;

	s = buf;

	return s;
}

const tstring 
StringUtil::GetIp( const tstring& s )
{
    std::vector<tstring> v;

    StringUtil::Split( s, _T(":"), v );

    if ( v.size() < 2 )
    {
        return _T("null");
    }

    return v[0];
}

int 
StringUtil::GetPort( const tstring& s )
{
    std::vector<tstring> v;

    StringUtil::Split( s, _T(":"), v );

    if ( v.size() < 2 )
    {
        return 0;
    }

    return ::_ttoi( v[1].c_str() );   
}

} // gk
