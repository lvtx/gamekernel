#include "stdafx.h"

#include <kcore/corebase.h>
#include <kcore/sys/Logger.h>

#include <kcore/util/StringUtil.h>

namespace gk {

Logger* 
Logger::Instance()
{
    static Logger instance;

    return &instance;
}

Logger::Logger()
: m_isConsolePrint( false )
{
#ifdef _DEBUG
    Enable( FT_DEBUG );
#endif
    Enable( FT_INFO );
    Enable( FT_WARN );
    Enable( FT_ERROR );
    Enable( FT_SERVICE );

	m_logFile.Open( _T("system") );

    Start();
}

Logger::~Logger()
{
	Fini();
}

void 
Logger::Enable( int feature )
{
    ScopedLock sl( m_memberLock );

    K_ASSERT( feature < FEATURE_LIMIT ); // max 128

    m_features.set( feature );
}

void 
Logger::Disable( int feature )
{
    ScopedLock sl( m_memberLock );

    K_ASSERT( feature < FEATURE_LIMIT ); // max 128

    m_features.set( feature, false );
}

bool 
Logger::IsEnabled( int feature )
{
    ScopedLock sl( m_memberLock );

    return m_features[feature];
}

void 
Logger::EnableConsole()
{
	ScopedLock sl( m_memberLock );

	m_isConsolePrint = true;
}

void 
Logger::DisableConsole()
{
	ScopedLock sl( m_memberLock );

	m_isConsolePrint = false;
}

void 
Logger::Put( const tstring& line )
{
    m_q.Put( line );
}

int 
Logger::Run()
{
    while ( IsRunning() )
    {
        tstring line;

        if ( !m_logFile.IsOpen() )
        {
            m_logFile.Open( _T("system") );
        }

        line.clear();

        if ( m_q.Get( line ) )
        {
			line.append( _T("\r\n") );

			std::string out;

			StringUtil::ConvertToMBCS( line, out );	

			m_logFile.Write( out );

			ScopedLock sl( m_memberLock );

			if ( m_isConsolePrint )
			{
				printf( "%s", out.c_str() );
			}

			out.clear();
			line.clear();
        }
        else
        {
            ::Sleep( 1 ); 
        }

		if ( m_flushTick.Elapsed() > 5000 )
		{
			m_logFile.Flush();

			m_flushTick.Reset();
		}
    }

    return 0;
}

void 
Logger::Fini()
{
	Stop();

	flush();
    
    cleanup();
}

void 
Logger::flush()
{
	// flush remaining lines

	tstring line;

    while ( m_q.Get( line ) )
    {
		line.append( _T("\r\n") );

		std::string out;

		StringUtil::ConvertToMBCS( line, out );	

		m_logFile.Write( out );

		line.clear();
	}
}

void 
Logger::cleanup()
{
    m_logFile.Close();
}

void 
LOG( int feature, const TCHAR* fmt, ... )
{
    K_ASSERT( feature > 0 );
    K_ASSERT( feature < FEATURE_LIMIT );

    if ( !Logger::Instance()->IsEnabled( feature ) )
	{
		return;
	}

    va_list args;

    TCHAR buf[4096];
    TCHAR head[128];

    ::memset( buf, 0, sizeof( TCHAR ) * 4096 );
    ::memset( head, 0, sizeof( TCHAR ) * 128 );

    SYSTEMTIME t;
    ::GetLocalTime( &t );

    TCHAR date[64];

    _sntprintf_s( date, _ARRAYSIZE(date)-1, 
                  _T("%02d:%02d:%02d"),
                  t.wHour, t.wMinute, t.wSecond );

    _sntprintf_s( head, _ARRAYSIZE(head)-1, 
                 _T("[%6x][%s][%d] "), 
                 ::GetCurrentThreadId(), 
                 date, 
				 feature );

    tstring s;

    s.append( head );

    va_start( args, fmt );

    _vsntprintf_s( buf, 4096, 4096, fmt, args );

    va_end( args );

    s.append( buf );

    Logger::Instance()->Put( s );

	s.clear(); 
}

} // gk
