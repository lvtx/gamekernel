#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/sys/LogFile.h>
#include <kcore/util//StringUtil.h>

namespace gk {

LogFile::LogFile()
: m_handle( 0 )
{
}

LogFile::~LogFile()
{
    Close();
}

bool 
LogFile::Open( const tstring& prefix )
{
    m_prefix = prefix;

    makeFileName();

	m_handle = ::CreateFile( m_filename.c_str(), 
		                     GENERIC_WRITE, 
							 FILE_SHARE_READ | FILE_SHARE_WRITE, 
							 NULL, 
							 CREATE_ALWAYS, 
							 FILE_ATTRIBUTE_NORMAL, 
							 NULL );

    if ( m_handle == INVALID_HANDLE_VALUE )
	{
		return false;
	}

	return true;
}

void 
LogFile::Write( const std::string& out )
{
	checkRotate();

    if ( !IsOpen() )
	{
		return;
	}

	DWORD len = 0;

	::WriteFile( m_handle, out.c_str(), out.length(), &len, NULL );

}

void 
LogFile::Flush()
{
	// empty. no buffering 
}

void 
LogFile::Close()
{
    if ( m_handle != INVALID_HANDLE_VALUE )
	{
		::CloseHandle( m_handle );

		m_handle = 0;
	}
}

void
LogFile::makeFileName()
{
    m_filename.clear();

    m_date = DateTime::GetPresentTime(); // 현재 날짜와 시간   

    TCHAR tmp[1024];
    ::memset( tmp, 0, 1024*sizeof(TCHAR) );

    tstring date;

    m_date.ToDateString( _T("%Y%m%d_%H"), date );


	TCHAR path[4096] = {0,};
	GetCurrentDirectory(4096, path);

	_tcscat_s( path, 4096, _T("\\log") );
	
	CreateDirectory(path, NULL);

    _sntprintf_s( tmp, _ARRAYSIZE(tmp)-1, 
                 _T("%s\\%s-%s.log"),                       // system-20080903.log
                 path, m_prefix.c_str(), date.c_str() );

	m_filename = tmp;
}

void 
LogFile::checkRotate() 
{
    // date check

    if ( m_date.GetDay() != DateTime::GetPresentTime().GetDay() )
    {
        Close();

		m_handle = 0;

        Open( m_prefix ); // 새로 열면 된다
    }
}

} // gk
