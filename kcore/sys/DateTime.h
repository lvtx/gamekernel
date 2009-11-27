#pragma once

#include <string>

namespace gk 
{
#define HIGH_DATETIME_ERROR (-1)
#define INVALID_DATETIME "Invalid datetime"
#define INVALID_SPANTIME "Invalid datetimespan"

class DateTime;
class DateTimeSpan;

class DateTime
{
public:
	enum DateTimeStatus {
		valid = 0,
		invalid = 1,
		null = 2,
	};

    static const DateTime GetPresentTime();
    
	DateTime();
	DateTime(const DateTime &dateSrc);
    DateTime(const LARGE_INTEGER dateSrc);
    DateTime(const LONGLONG dateSrc);
	DateTime(const SYSTEMTIME &systimeSrc);
	DateTime(const FILETIME &filetimeSrc);
	DateTime(int nYear, int nMonth, int nDay,
              int nHour, int nMinute, int nSecond, 
              int nMilli = 0, int nMicro = 0, int nNano = 0);

	const DateTime& operator=(const DateTime& dateSrc);
	const DateTime& operator=(const LARGE_INTEGER dateSrc);
	const DateTime& operator=(const LONGLONG dateSrc);

	const DateTime& operator=(const SYSTEMTIME& systimeSrc);
	const DateTime& operator=(const FILETIME& filetimeSrc);

    bool operator==(const DateTime& date) const;
    bool operator!=(const DateTime& date) const;
    bool operator<(const DateTime& date) const;
    bool operator>(const DateTime& date) const;
    bool operator<=(const DateTime& date) const;
    bool operator>=(const DateTime& date) const;

    // DateTime math
    DateTime operator+(const DateTimeSpan &dateSpan) const;
    DateTime operator-(const DateTimeSpan &dateSpan) const;
    const DateTime& operator+=(const DateTimeSpan dateSpan);
    const DateTime& operator-=(const DateTimeSpan dateSpan);

    // DateTimeSpan math
    DateTimeSpan operator-(const DateTime& date) const;

    operator SYSTEMTIME() const;
    operator FILETIME() const;
    operator LARGE_INTEGER() const {return time_;}
    operator LONGLONG() const {return time_.QuadPart;}

	void Reset();

    int SetDateTime(int nYear, int nMonth, int nDay,
                    int nHour, int nMinute, int nSecond, 
                    int nMilli = 0, int nMicro = 0, int nNano = 0);
	int SetDate(int nYear, int nMonth, int nDay);
	int SetTime(int nHour, int nMin, int nSec);
	int SetFracTime(int nMilli, int nMicro, int nNano);

    DateTimeStatus GetStatus() const {return status_;}
    void SetStatus(DateTimeStatus status) { status_ = status; }
	BOOL GetAsSystemTime(SYSTEMTIME& sysTime) const;

	int GetYear() const;
	int GetMonth() const;       // month of year (1 = Jan)
	int GetDay() const;         // day of month (0-31)
	int GetHour() const;        // hour in day (0-23)
	int GetMinute() const;      // minute in hour (0-59)
	int GetSecond() const;      // second in minute (0-59)
	int GetMilliSecond() const; // millisecond in minute (0-999)
	int GetMicroSecond() const; // microsecond in minute (0-999)
	int GetNanoSecond() const;  // nanosecond in minute (0-999), step of 100ns
    int GetDayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat
	int GetDayOfYear() const;   // days since start of year, Jan 1 = 1

	void ToDateString(const TCHAR* fmt, tstring& v) const;
	void ToTimeString(const TCHAR* fmt, tstring& v) const;

	LONGLONG GetRawTime() const;

	void SetRawTime(LONGLONG rt );

private:
    struct _TimeFormat {
        int  nYear;
        UINT nMonth;
        UINT nDay;
        UINT nHour;
        UINT nMinute;
        UINT nSecond;
        UINT nMilli;
        UINT nMicro;
        UINT nNano;
        UINT nDayOfYear;
        UINT nDayOfWeek;
    };
    static int anMonthDayInYear[13];
    static BOOL convertTimeToLongLong(const _TimeFormat  &SrcTime,
                                      LARGE_INTEGER &liDestTime);
    static BOOL convertLongLongToTime(const LARGE_INTEGER &liSrcTime,
                                       _TimeFormat  &DestTime);
    static void convertToStandardFormat(_TimeFormat &tmTempHigh, tm &tmTemp);

    friend class DateTimeSpan;

private:
	DateTimeStatus   status_;
	LARGE_INTEGER    time_;

}; //class DateTime

class DateTimeSpan
{
public:
    enum TimeSpanStatus {
       valid = 0,
       invalid = 1,
       null = 2,
    };

    DateTimeSpan();
    DateTimeSpan(const LONGLONG dateSrcSpan);
    DateTimeSpan(const LARGE_INTEGER dateSrcSpan);
    DateTimeSpan(const DateTimeSpan &dateSrcSpan);
	DateTimeSpan(long lDays, int nHours, int nMins, int nSecs,
                  int nMilli = 0, int nMicro = 0, int nNano = 0);

    void SetStatus(TimeSpanStatus status) { status_ = status; }
    TimeSpanStatus GetStatus() const {return status_;}

	void SetHighTimeSpan(long lDays, int nHours, int nMins, int nSecs,
                                     int nMillis = 0, int nMicros = 0, int nNanos = 0);

	LONGLONG GetTotalDays() const;    // span in days (about -3.65e6 to 3.65e6)
	LONGLONG GetTotalHours() const;   // span in hours (about -8.77e7 to 8.77e6)
	LONGLONG GetTotalMinutes() const; // span in minutes (about -5.26e9 to 5.26e9)
	LONGLONG GetTotalSeconds() const; // span in seconds (about -3.16e11 to 3.16e11)
	LONGLONG GetTotalMilliSeconds() const; // span in milliseconds
	LONGLONG GetTotalMicroSeconds() const; // span in microseconds
	LONGLONG GetTotalNanoSeconds() const; // span in nanoseconds

	int GetDays() const;       // component days in span
	int GetHours() const;      // component hours in span (-23 to 23)
	int GetMinutes() const;    // component minutes in span (-59 to 59)
	int GetSeconds() const;    // component seconds in span (-59 to 59)
	int GetMilliSeconds() const; // component Milliseconds in span (-999 to 999)
	int GetMicroSeconds() const; // component Microseconds in span (-999 to 999)
	int GetNanoSeconds() const;  // component Nanoseconds in span (-900 to 900)

    bool IsNegative() const;

    // formatting
    LPSTR Format(LPSTR pszBuffer, int iBufferLen, LPCTSTR pFormat) const;

	const DateTimeSpan& operator=(LARGE_INTEGER dateSpanSrc);
	const DateTimeSpan& operator=(LONGLONG dateSpanSrc);
	const DateTimeSpan& operator=(const DateTimeSpan& dateSpanSrc);

    bool operator==(const DateTimeSpan& dateSpan) const;
	bool operator!=(const DateTimeSpan& dateSpan) const;
	bool operator<(const DateTimeSpan& dateSpan) const;
	bool operator>(const DateTimeSpan& dateSpan) const;
	bool operator<=(const DateTimeSpan& dateSpan) const;
	bool operator>=(const DateTimeSpan& dateSpan) const;

	// DateTimeSpan math
	DateTimeSpan operator+(const DateTimeSpan& dateSpan) const;
	DateTimeSpan operator-(const DateTimeSpan& dateSpan) const;
	const DateTimeSpan& operator+=(const DateTimeSpan dateSpan);
	const DateTimeSpan& operator-=(const DateTimeSpan dateSpan);
	DateTimeSpan operator-() const;

    operator LARGE_INTEGER() const {return span_;}
    operator LONGLONG() const {return span_.QuadPart;}

private:
    friend class DateTime;

	TimeSpanStatus	status_;
	LARGE_INTEGER   span_;
}; // class DateTimeSpan

inline 
DateTime::DateTime()
{
	status_ = valid;
	time_.QuadPart = 0;    // 1 Jan-1601, 00:00, 100ns clicks 

	*this = GetPresentTime();
} 

inline 
DateTime::DateTime(const DateTime &dateSrc)
{
	*this = dateSrc;
} 

inline 
DateTime::DateTime(const LARGE_INTEGER dateSrc)
{
	*this = dateSrc;
} 

inline 
DateTime::DateTime(const LONGLONG dateSrc)
{
	*this = dateSrc;
} 

inline 
DateTime::DateTime(const SYSTEMTIME &systimeSrc)
{
	*this = systimeSrc;
} 

inline 
DateTime::DateTime(const FILETIME &filetimeSrc)
{
	*this = filetimeSrc;
} 

inline 
DateTime::operator SYSTEMTIME() const
{
	SYSTEMTIME systime;
	GetAsSystemTime(systime);
	return systime;
} 

inline 
DateTime::operator FILETIME() const
{
	FILETIME filetime;
	filetime.dwLowDateTime = time_.LowPart;
	filetime.dwHighDateTime = time_.HighPart;
	return filetime;
} 

inline 
const DateTime& 
DateTime::operator=(const DateTime& dateSrc) 
{
	time_ = dateSrc.time_;
	status_ = dateSrc.status_;
	return *this;
} 

inline 
const DateTime& 
DateTime::operator=(const LARGE_INTEGER dateSrc)
{
	time_ = dateSrc;
	status_ = valid;
	return *this;
} 

inline 
const DateTime& 
DateTime::operator=(const LONGLONG dateSrc)
{
	time_.QuadPart = dateSrc;
	status_ = valid;
	return *this;
} 

inline 
int 
DateTime::SetDate(int nYear, int nMonth, int nDay)
{
	return SetDateTime(nYear, nMonth, nDay, 0, 0, 0, 0, 0, 0);
} 

inline 
int 
DateTime::SetTime(int nHour, int nMin, int nSec)
{
	return SetDateTime(1601, 1, 1, nHour, nMin, nSec, 0, 0, 0);
} 

inline 
int 
DateTime::SetFracTime(int nMilli, int nMicro, int nNano)
{
	return SetDateTime(1601, 1, 1, 0, 0, 0, nMilli, nMicro, nNano);
}

inline 
bool 
DateTime::operator==(const DateTime& date) const
{ 
	return (status_ == date.status_ && time_.QuadPart == date.time_.QuadPart); 
} 

inline 
bool 
DateTime::operator!=(const DateTime& date) const
{
	return (status_ != date.status_ || time_.QuadPart != date.time_.QuadPart); 
} 

inline 
bool 
DateTime::operator<(const DateTime& date) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(date.GetStatus() == valid);
	return (time_.QuadPart < date.time_.QuadPart); 
} 

inline 
bool 
DateTime::operator>(const DateTime& date) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(date.GetStatus() == valid);
	return (time_.QuadPart > date.time_.QuadPart); 
} 

inline 
bool 
DateTime::operator<=(const DateTime& date) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(date.GetStatus() == valid);
	return (time_.QuadPart <= date.time_.QuadPart); 
} 

inline 
bool 
DateTime::operator>=(const DateTime& date) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(date.GetStatus() == valid);
	return (time_.QuadPart >= date.time_.QuadPart); 
} 

inline 
const DateTime& 
DateTime::operator+=(const DateTimeSpan dateSpan)
{
	*this = *this + dateSpan; 
	return *this;
} 

inline 
const DateTime& 
DateTime::operator-=(const DateTimeSpan dateSpan)
{
	*this = *this - dateSpan; 
	return *this;
} 

inline
DateTimeSpan::DateTimeSpan()
{
	status_ = valid;
	span_.QuadPart = 0;
}

inline 
DateTimeSpan::DateTimeSpan(const LARGE_INTEGER dateSrcSpan)
{
	*this = dateSrcSpan;
} 

inline 
DateTimeSpan::DateTimeSpan(const LONGLONG dateSrcSpan)
{
	*this = dateSrcSpan;
} 

inline 
DateTimeSpan::DateTimeSpan(const DateTimeSpan &dateSrcSpan)
{
	*this = dateSrcSpan;
} 

inline 
DateTimeSpan::DateTimeSpan(long lDays, int nHours, int nMins, int nSecs,
						   int nMilli, int nMicro, int nNano)   // Milli, Micro & nano, default = 0
{
	SetHighTimeSpan(lDays, nHours, nMins, nSecs,
		nMilli, nMicro, nNano);
} 

inline 
bool 
DateTimeSpan::IsNegative() const
{
	return (span_.QuadPart < 0);
} 

inline 
const DateTimeSpan& 
DateTimeSpan::operator=(LARGE_INTEGER dateSpanSrc)
{
	span_ = dateSpanSrc;
	status_ = valid;
	return *this;
}

inline 
const DateTimeSpan& 
DateTimeSpan::operator=(LONGLONG dateSpanSrc)
{
	span_.QuadPart = dateSpanSrc;
	status_ = valid;
	return *this;
} 

inline 
const DateTimeSpan& 
DateTimeSpan::operator=(const DateTimeSpan& dateSpanSrc)
{
	span_ = dateSpanSrc.span_;
	status_ = dateSpanSrc.status_;
	return *this;
}

inline 
bool 
DateTimeSpan::operator==(const DateTimeSpan& dateSpan) const
{
	return (status_ == dateSpan.status_ &&
		span_.QuadPart == dateSpan.span_.QuadPart);
} 

inline 
bool 
DateTimeSpan::operator!=(const DateTimeSpan& dateSpan) const
{
	return (status_ != dateSpan.status_ ||
		span_.QuadPart != dateSpan.span_.QuadPart);
}

inline 
bool 
DateTimeSpan::operator<(const DateTimeSpan& dateSpan) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(dateSpan.GetStatus() == valid);
	return span_.QuadPart < dateSpan.span_.QuadPart;
}

inline 
bool 
DateTimeSpan::operator>(const DateTimeSpan& dateSpan) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(dateSpan.GetStatus() == valid);
	return span_.QuadPart > dateSpan.span_.QuadPart;
}

inline 
bool 
DateTimeSpan::operator<=(const DateTimeSpan& dateSpan) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(dateSpan.GetStatus() == valid);
	return span_.QuadPart <= dateSpan.span_.QuadPart;
}

inline 
bool 
DateTimeSpan::operator>=(const DateTimeSpan& dateSpan) const
{
	K_ASSERT(GetStatus() == valid);
	K_ASSERT(dateSpan.GetStatus() == valid);
	return span_.QuadPart >= dateSpan.span_.QuadPart;
} 

inline 
const DateTimeSpan& 
DateTimeSpan::operator+=(const DateTimeSpan dateSpan)
{
	*this = *this + dateSpan; 
	return *this;
}

inline 
const DateTimeSpan& 
DateTimeSpan::operator-=(const DateTimeSpan dateSpan)
{
	*this = *this - dateSpan; 
	return *this;
}

inline 
DateTimeSpan 
DateTimeSpan::operator-() const
{
	return -this->span_.QuadPart;
}

} // gk 

