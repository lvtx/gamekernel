#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/sys/DateTime.h>

#include <time.h>

namespace gk {

#define LLABS(i) (((i)<0)?-(i):(i))


#define MAX_TIME_BUFFER_SIZE 128

// Constant array with months # of days of year
int DateTime::anMonthDayInYear[13] = {0, 31, 59, 90, 120, 151, 181, 
                                       212, 243, 273, 304, 334, 365};

// Static member for getting the current time
const DateTime 
DateTime::GetPresentTime()
{
    SYSTEMTIME systime;
    ::GetLocalTime(&systime);
    return DateTime(systime);
} // DateTime::GetPresentTime()

DateTime::DateTime(int nYear, int nMonth, int nDay,
                     int nHour, int nMinute, int nSecond, 
                     int nMilli , int nMicro ,int nNano)
                       // nMilli, nMicro & nNano default = 0
{
    _TimeFormat  SrcTime;
    SrcTime.nYear   = nYear;
    SrcTime.nMonth  = nMonth;
    SrcTime.nDay    = nDay;
    SrcTime.nHour   = nHour;
    SrcTime.nMinute = nMinute;
    SrcTime.nSecond = nSecond;
    SrcTime.nMilli  = nMilli;
    SrcTime.nMicro  = nMicro;
    SrcTime.nNano   = nNano;
    status_ = convertTimeToLongLong(SrcTime, time_) ? valid : invalid;
} 

const DateTime& 
DateTime::operator=(const SYSTEMTIME& systimeSrc)
{
    _TimeFormat  SrcTime;
    SrcTime.nYear   = systimeSrc.wYear;
    SrcTime.nMonth  = systimeSrc.wMonth;
    SrcTime.nDay    = systimeSrc.wDay;
    SrcTime.nHour   = systimeSrc.wHour;
    SrcTime.nMinute = systimeSrc.wMinute;
    SrcTime.nSecond = systimeSrc.wSecond;
    SrcTime.nMilli  = systimeSrc.wMilliseconds;
    SrcTime.nMicro  = 0;
    SrcTime.nNano  = 0;
    status_ = convertTimeToLongLong(SrcTime, time_) ? valid : invalid;
    return *this;
} 

const DateTime& 
DateTime::operator=(const FILETIME& filetimeSrc)
{
    SYSTEMTIME systime;

    if (FileTimeToSystemTime(&filetimeSrc, &systime)) {
        _TimeFormat  SrcTime;
        SrcTime.nYear   = systime.wYear;
        SrcTime.nMonth  = systime.wMonth;
        SrcTime.nDay    = systime.wDay;
        SrcTime.nHour   = systime.wHour;
        SrcTime.nMinute = systime.wMinute;
        SrcTime.nSecond = systime.wSecond;
        SrcTime.nMilli  = systime.wMilliseconds;
        SrcTime.nMicro  = 0;
        SrcTime.nNano  = 0;
        status_ = convertTimeToLongLong(SrcTime, time_) ? valid : invalid;
    }
    else {
        SetStatus(invalid);
    }

    return *this;
}

int 
DateTime::SetDateTime(int nYear, int nMonth, int nDay,
					   int nHour, int nMinute, int nSecond,                            
					   int nMilli, int nMicro, int nNano)  
{
    _TimeFormat  SrcTime;
    SrcTime.nYear   = nYear;
    SrcTime.nMonth  = nMonth;
    SrcTime.nDay    = nDay;
    SrcTime.nHour   = nHour;
    SrcTime.nMinute = nMinute;
    SrcTime.nSecond = nSecond;
    SrcTime.nMilli  = nMilli;
    SrcTime.nMicro  = nMicro;
    SrcTime.nNano   = nNano;
    return (status_ = convertTimeToLongLong(SrcTime, time_) ? valid : invalid) == valid;
} 

void 
DateTime::Reset()
{
	*this = GetPresentTime();
}


// HighTime helper function, static function
BOOL 
DateTime::convertTimeToLongLong(const _TimeFormat  &SrcTime,
								 LARGE_INTEGER &liDestTime)
{
	LARGE_INTEGER nDate;
    int iDays = SrcTime.nDay;
    UINT nHour = SrcTime.nHour;
    UINT nMinute = SrcTime.nMinute;
    UINT nSecond = SrcTime.nSecond;
    UINT nMilliSecond = SrcTime.nMilli;
    UINT nMicroSecond = SrcTime.nMicro;
    UINT nHundredsNano = (SrcTime.nNano +50) / 100;

    // Validate year and month
	if (SrcTime.nYear > 29000 || SrcTime.nYear < -29000 ||
        SrcTime.nMonth < 1 || SrcTime.nMonth > 12)
		return FALSE;

	//  Check for leap year
	BOOL bIsLeapYear = ((SrcTime.nYear & 3) == 0) &&
		                ((SrcTime.nYear % 100) != 0 || (SrcTime.nYear % 400) == 0);

    /*int nDaysInMonth = 
		anMonthDayInYear[SrcTime.nMonth] - anMonthDayInYear[SrcTime.nMonth-1] +
		((bIsLeapYear && SrcTime.nDay == 29 && SrcTime.nMonth == 2) ? 1 : 0);*/

    // Adjust time and frac time
    nMicroSecond += nHundredsNano / 10;
    nHundredsNano %= 10;
    nMilliSecond += nMicroSecond / 1000;
    nMicroSecond %= 1000;
    nSecond +=nMilliSecond / 1000;
    nMilliSecond %= 1000;
    nMinute += nSecond / 60;
    nSecond %= 60;
    nHour += nMinute / 60;
    nMinute %= 60;
    iDays += nHour / 24;
    nHour %= 24;

	//It is a valid date; make Jan 1, 1AD be 1
	nDate.QuadPart = SrcTime.nYear*365L + SrcTime.nYear/4 - SrcTime.nYear/100 + SrcTime.nYear/400 +
	                 anMonthDayInYear[SrcTime.nMonth-1] + iDays;

	//  If leap year and it's before March, subtract 1:
	if (SrcTime.nMonth <= 2 && bIsLeapYear)
		--nDate.QuadPart;

	//  Offset so that 01/01/1601 is 0
	nDate.QuadPart -= 584754L;

    // Change nDate to seconds
    nDate.QuadPart *= 86400L;
    nDate.QuadPart += (nHour * 3600L) + (nMinute * 60L) + nSecond;

    // Change nDate to hundreds of nanoseconds
    nDate.QuadPart *= 10000000L;
    nDate.QuadPart += (nMilliSecond * 10000L) + (nMicroSecond * 10L) + nHundredsNano;

	liDestTime = nDate;

    return TRUE;
}

BOOL 
DateTime::convertLongLongToTime(const LARGE_INTEGER &liSrcTime,
								 _TimeFormat  &DestTime)
{
    LARGE_INTEGER nTempTime;
	long nDaysAbsolute;     // Number of days since 1/1/0
	long nSecsInDay;        // Time in seconds since midnight
    long nMinutesInDay;     // Minutes in day

	long n400Years;         // Number of 400 year increments since 1/1/0
	long n400Century;       // Century within 400 year block (0,1,2 or 3)
	long n4Years;           // Number of 4 year increments since 1/1/0
	long n4Day;             // Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	long n4Yr;              // Year within 4 year block (0,1,2 or 3)
	BOOL bLeap4 = TRUE;     // TRUE if 4 year block includes leap year
    long nHNanosThisDay;
    long nMillisThisDay;

    nTempTime = liSrcTime;
    nHNanosThisDay      = (long)(nTempTime.QuadPart % 10000000L);
    nTempTime.QuadPart /= 10000000L;
    nSecsInDay          = (long)(nTempTime.QuadPart % 86400L);
    nTempTime.QuadPart /= 86400L;
    nDaysAbsolute       = (long)(nTempTime.QuadPart);
    nDaysAbsolute      += 584754L;	//  Add days from 1/1/0 to 01/01/1601

    // Calculate the day of week (sun=1, mon=2...)
    //   -1 because 1/1/0 is Sat.  +1 because we want 1-based
    DestTime.nDayOfWeek = (int)((nDaysAbsolute - 1) % 7L) + 1;


	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (long)(nDaysAbsolute / 146097L);

	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;

	// -1 because first century has extra day
	n400Century = (long)((nDaysAbsolute - 1) / 36524L);

	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

		// +1 because 1st 4 year increment has 1460 days
		n4Years = (long)((nDaysAbsolute + 1) / 1461L);

		if (n4Years != 0)
			n4Day = (long)((nDaysAbsolute + 1) % 1461L);
		else
		{
			bLeap4 = FALSE;
			n4Day = (long)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (long)(nDaysAbsolute / 1461L);
		n4Day = (long)(nDaysAbsolute % 1461L);
	}

	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;

		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}

	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	DestTime.nDayOfYear = (int)n4Day + 1;
	DestTime.nYear = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			DestTime.nMonth  = 2;
			DestTime.nDay = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}

	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;

	// Month number always >= n/32, so save some loop time */
	for (DestTime.nMonth = (n4Day >> 5) + 1;
		n4Day > anMonthDayInYear[DestTime.nMonth]; DestTime.nMonth++);

	DestTime.nDay = (int)(n4Day - anMonthDayInYear[DestTime.nMonth-1]);

DoTime:
	if (nSecsInDay == 0)
		DestTime.nHour = DestTime.nMinute = DestTime.nSecond = 0;
	else
	{
		DestTime.nSecond = (UINT)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		DestTime.nMinute = (UINT)nMinutesInDay % 60;
		DestTime.nHour   = (UINT)nMinutesInDay / 60;
	}

    if (nHNanosThisDay == 0)
		DestTime.nMilli = DestTime.nMicro = DestTime.nNano = 0;
	else
	{
		DestTime.nNano = (UINT)((nHNanosThisDay % 10L) * 100L);
		nMillisThisDay = nHNanosThisDay / 10L;
		DestTime.nMicro = (UINT)nMillisThisDay % 1000;
		DestTime.nMilli   = (UINT)nMillisThisDay / 1000;
	}

    return TRUE;
} 

void DateTime::convertToStandardFormat(_TimeFormat &tmTempHigh, tm &tmSrc)
{
    // Convert internal tm to format expected by runtimes (sfrtime, etc)
	tmSrc.tm_year = tmTempHigh.nYear-1900;  // year is based on 1900
	tmSrc.tm_mon  = tmTempHigh.nMonth-1;      // month of year is 0-based
	tmSrc.tm_wday = tmTempHigh.nDayOfWeek-1;     // day of week is 0-based
	tmSrc.tm_yday = tmTempHigh.nDayOfYear-1;     // day of year is 0-based
    tmSrc.tm_mday = tmTempHigh.nDay;
    tmSrc.tm_hour = tmTempHigh.nHour;
    tmSrc.tm_min = tmTempHigh.nMinute;
    tmSrc.tm_sec = tmTempHigh.nSecond;	
    tmSrc.tm_isdst = 0;
} 

int DateTime::GetYear() const
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nYear;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetMonth() const       // month of year (1 = Jan)
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nMonth;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetDay() const         // day of month (0-31)
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nDay;
	else
		return HIGH_DATETIME_ERROR;
}

int DateTime::GetHour() const        // hour in day (0-23)
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nHour;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetMinute() const      // minute in hour (0-59)
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nMinute;
	else
		return HIGH_DATETIME_ERROR;

} 

int DateTime::GetSecond() const      // second in minute (0-59)
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nSecond;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetMilliSecond() const // millisecond in minute (0-999)
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nMilli;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetMicroSecond() const // microsecond in minute (0-999)
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nMicro;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetNanoSecond() const  // nanosecond in minute (0-999), step of 100ns
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nNano;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetDayOfWeek() const   // 1=Sun, 2=Mon, ..., 7=Sat
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nDayOfWeek;
	else
		return HIGH_DATETIME_ERROR;
} 

int DateTime::GetDayOfYear() const   // days since start of year, Jan 1 = 1
{
	_TimeFormat tmTemp;
	if (GetStatus() == valid && convertLongLongToTime(time_, tmTemp))
		return tmTemp.nDayOfYear;
	else
		return HIGH_DATETIME_ERROR;
} 

void
DateTime::ToDateString( const TCHAR* fmt, tstring& v ) const
{
    v.clear();

	_TimeFormat  SrcTime;
	SrcTime.nYear   = GetYear();
	SrcTime.nMonth  = GetMonth();
	SrcTime.nDay    = GetDay();
	SrcTime.nHour   = GetHour();
	SrcTime.nMinute = GetMinute();
	SrcTime.nSecond = GetSecond();
	SrcTime.nMilli  = GetMilliSecond();
	SrcTime.nMicro  = GetMicroSecond();
	SrcTime.nNano   = GetNanoSecond();

	struct tm t;

	TCHAR dest[128];

	memset( dest, 0, 128*sizeof(TCHAR) );

	convertToStandardFormat(SrcTime, t);
	
    _tcsftime( dest, 128, fmt, &t);

    v = dest;
}

void
DateTime::ToTimeString( const TCHAR* fmt, tstring& v ) const
{
    v.clear();

	_TimeFormat  SrcTime;
	SrcTime.nYear   = GetYear();
	SrcTime.nMonth  = GetMonth();
	SrcTime.nDay    = GetDay();
	SrcTime.nHour   = GetHour();
	SrcTime.nMinute = GetMinute();
	SrcTime.nSecond = GetSecond();
	SrcTime.nMilli  = GetMilliSecond();
	SrcTime.nMicro  = GetMicroSecond();
	SrcTime.nNano   = GetNanoSecond();

	struct tm t;

	TCHAR dest[128];

	memset( dest, 0, 128*sizeof(TCHAR) );

	convertToStandardFormat(SrcTime, t);

    _tcsftime( dest, 128, fmt, &t);

    v = dest;
}

LONGLONG 
DateTime::GetRawTime() const
{
	return (LONGLONG)time_.QuadPart;
}

void 
DateTime::SetRawTime(LONGLONG rt )
{
	time_.QuadPart = rt;
}

BOOL DateTime::GetAsSystemTime(SYSTEMTIME& sysTime) const
{
	BOOL bRetVal = FALSE;
	if (GetStatus() == valid)
	{
		_TimeFormat tmTemp;
		if (convertLongLongToTime(time_, tmTemp))
		{
			sysTime.wYear = (WORD) tmTemp.nYear;
			sysTime.wMonth = (WORD) tmTemp.nMonth;
			sysTime.wDayOfWeek = (WORD) (tmTemp.nDayOfWeek - 1);
			sysTime.wDay = (WORD) tmTemp.nDay;
			sysTime.wHour = (WORD) tmTemp.nHour;
			sysTime.wMinute = (WORD) tmTemp.nMinute;
			sysTime.wSecond = (WORD) tmTemp.nSecond;
			sysTime.wMilliseconds = (WORD)tmTemp.nMilli;

			bRetVal = TRUE;
		}
	}

	return bRetVal;
} 

// DateTime math
DateTime DateTime::operator+(const DateTimeSpan &dateSpan) const
{
	DateTime dateResult;    // Initializes status_ to valid

	// If either operand NULL, result NULL
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateResult.SetStatus(null);
		return dateResult;
	}

	// If either operand invalid, result invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateResult.SetStatus(invalid);
		return dateResult;
	}

	// Compute the actual date difference by adding underlying dates
	dateResult.time_.QuadPart = time_.QuadPart + dateSpan.span_.QuadPart;

	// Validate within range
	//dateResult.CheckRange();

	return dateResult;
} 

DateTime DateTime::operator-(const DateTimeSpan &dateSpan) const
{
	DateTime dateResult;    // Initializes status_ to valid

	// If either operand NULL, result NULL
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateResult.SetStatus(null);
		return dateResult;
	}

	// If either operand invalid, result invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateResult.SetStatus(invalid);
		return dateResult;
	}

	// Compute the actual date difference by adding underlying dates
	dateResult.time_.QuadPart = time_.QuadPart - dateSpan.span_.QuadPart;

	// Validate within range
	//dateResult.CheckRange();

	return dateResult;
} 


// DateTimeSpan math
DateTimeSpan DateTime::operator-(const DateTime& date) const
{
	DateTimeSpan spanResult;

	// If either operand NULL, result NULL
	if (GetStatus() == null || date.GetStatus() == null)
	{
		spanResult.SetStatus(DateTimeSpan::null);
		return spanResult;
	}

	// If either operand invalid, result invalid
	if (GetStatus() == invalid || date.GetStatus() == invalid)
	{
		spanResult.SetStatus(DateTimeSpan::invalid);
		return spanResult;
	}

    spanResult.span_.QuadPart = time_.QuadPart - date.time_.QuadPart;


	return spanResult;
} 

void DateTimeSpan::SetHighTimeSpan(long lDays, int nHours, int nMinutes, int nSeconds,
                                    int nMillis, int nMicros, int nNanos)   
                                      // Milli, Micro & nano, default = 0
{

    int nHundredsNanos;
    
    if (nNanos >= 0)
        nHundredsNanos = (nNanos+50) / 100;
    else
        nHundredsNanos = (nNanos-50) / 100;

    nMicros += nHundredsNanos / 10;
    nHundredsNanos %= 10;
    nMillis += nMicros / 1000;
    nMicros %= 1000;
    nSeconds +=nMillis / 1000;
    nMillis %= 1000;
    nMinutes += nSeconds / 60;
    nSeconds %= 60;
    nHours += nMinutes / 60;
    nMinutes %= 60;
    lDays += nHours / 24;
    nHours %= 24;

    span_.QuadPart = lDays;
    span_.QuadPart *= 86400L;
    span_.QuadPart += (nHours * 3600L) +
                         (nMinutes * 60)  +
                         nSeconds;
    span_.QuadPart *= 10000000L;
    span_.QuadPart += (nMillis * 10000L) +
                         (nMicros * 10L) +
                         nHundredsNanos;

    SetStatus(valid);
} 

LONGLONG DateTimeSpan::GetTotalDays() const    // span in days (about -3.65e6 to 3.65e6)
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    liTemp /= 86400L;
    return liTemp;
} 

LONGLONG DateTimeSpan::GetTotalHours() const   // span in hours (about -8.77e7 to 8.77e6)
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    liTemp /= 3600L;
    return liTemp;
} 

LONGLONG DateTimeSpan::GetTotalMinutes() const // span in minutes (about -5.26e9 to 5.26e9)
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    liTemp /= 60L;
    return liTemp;
} 

LONGLONG DateTimeSpan::GetTotalSeconds() const // span in seconds (about -3.16e11 to 3.16e11)
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    return liTemp;
} 

LONGLONG DateTimeSpan::GetTotalMilliSeconds() const // span in milliseconds
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000L;
    return liTemp;
} 

LONGLONG DateTimeSpan::GetTotalMicroSeconds() const // span in microseconds
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10L;
    return liTemp;
} 

LONGLONG DateTimeSpan::GetTotalNanoSeconds() const // span in nanoseconds
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart * 100L;
    return liTemp;
} 

int DateTimeSpan::GetDays() const       // component days in span
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    liTemp = (liTemp / 86400L);
    return (int)liTemp;
} 

int DateTimeSpan::GetHours() const      // component hours in span (-23 to 23)
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    liTemp = (liTemp % 86400L) / 3600;
    return (int)liTemp;
} 

int DateTimeSpan::GetMinutes() const    // component minutes in span (-59 to 59)
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    liTemp = (liTemp % 3600L) / 60;
    return (int)liTemp;
} 

int DateTimeSpan::GetSeconds() const    // component seconds in span (-59 to 59)
{
    LONGLONG liTemp;
    liTemp = span_.QuadPart / 10000000L;
    liTemp = (liTemp % 60L);
    return (int)liTemp;
} 

int DateTimeSpan::GetMilliSeconds() const // component Milliseconds in span (-999 to 999)
{
    LONGLONG liTemp;
    liTemp = (span_.QuadPart % 10000000L) / 10000L;
    return (int)liTemp;
} 

int DateTimeSpan::GetMicroSeconds() const // component Microseconds in span (-999 to 999)
{
    LONGLONG liTemp;
    liTemp = (span_.QuadPart % 10000L) / 10L;
    return (int)liTemp;
} 

int DateTimeSpan::GetNanoSeconds() const  // component Nanoseconds in span (-900 to 900)
{
    LONGLONG liTemp;
    liTemp = (span_.QuadPart % 10) * 100L;
    return (int)liTemp;
} 


// DateTimeSpan math
DateTimeSpan DateTimeSpan::operator+(const DateTimeSpan& dateSpan) const
{
	DateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Add spans and validate within legal range
	dateSpanTemp.span_.QuadPart = span_.QuadPart + dateSpan.span_.QuadPart;

	return dateSpanTemp;
} // DateTimeSpan::operator+()

DateTimeSpan DateTimeSpan::operator-(const DateTimeSpan& dateSpan) const
{
	DateTimeSpan dateSpanTemp;

	// If either operand Null, result Null
	if (GetStatus() == null || dateSpan.GetStatus() == null)
	{
		dateSpanTemp.SetStatus(null);
		return dateSpanTemp;
	}

	// If either operand Invalid, result Invalid
	if (GetStatus() == invalid || dateSpan.GetStatus() == invalid)
	{
		dateSpanTemp.SetStatus(invalid);
		return dateSpanTemp;
	}

	// Add spans and validate within legal range
	dateSpanTemp.span_.QuadPart = span_.QuadPart - dateSpan.span_.QuadPart;

	return dateSpanTemp;

} // DateTimeSpan::operator-()

} // gk
