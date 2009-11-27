#pragma once

typedef unsigned char	    byte;
typedef unsigned short	    ushort;
typedef unsigned long	    ulong;
typedef unsigned int	    uint;
typedef unsigned long       uint4;
typedef unsigned __int64    uint8;
typedef long long           longlong;

typedef long       int4;
typedef __int64    int8;

#ifdef _UNICODE
#define tstring std::wstring
#define tofstream std::wofstream
#define tifstream std::wifstream 
#define tostream  std::wostream
#define tistream  std::wifstream
#define tostringstream std::wostringstream
#else
#define tstring std::string
#define tofstream std::ofstream
#define tifstream std::ifstream
#define tostream  std::ostream
#define tistream  std::ifstream
#define tostringstream std::ostringstream
#endif

// use result instead of bool or int code 
#include <lax/kcore/base/Result.h>

