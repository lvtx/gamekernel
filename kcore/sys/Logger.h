#pragma once 

#include <kcore/sys/Thread.h>
#include <kcore/sys/Queue.h>
#include <kcore/sys/LogFile.h>
#include <kcore/sys/Tick.h>

#include <bitset>

namespace gk 
{

enum FEATURE 
{
      FT_DEBUG     = 1
	, FT_DEBUG_FLOW
    , FT_INFO
    , FT_WARN
    , FT_ERROR
    , FT_SERVICE
    , FEATURE_LIMIT = 64
};

/**
 * @class Logger
 *
 * Logs to a sys log file.
 */
class Logger : public Thread
{
public:
    static Logger* Instance();

    ~Logger();

	/**
	 * Enable the feature
	 *
	 * @param feature Index value of feature
	 */
    void Enable( int feature );

	/**
	 * Disable the feature
	 *
	 * @param feature Index value of feature
	 */
    void Disable( int feature );

	/**
	 * Check whether the feature is enabled
	 *
	 * @param feature Index value of feature
	 * @return true if the feature is enabled
	 */
    bool IsEnabled( int feature );

	/**
	 * Enable log to console
	 */
	void EnableConsole();

	/**
	 * Disable log to console
	 */
	void DisableConsole();

	/**
	 * Put a line to log
	 *
	 * @param line A string to log
	 */
    void Put( const tstring& line );

	/**
	 * Thread::Run
	 */
    int Run(); 

	/**
	 * Explicit finish 
	 */
	void Fini();

private:
    Logger();

	void flush();
    void cleanup();

private:
    Queue<tstring, Mutex>       m_q;
    LogFile                     m_logFile;

    std::bitset<FEATURE_LIMIT>  m_features;
    Mutex                       m_memberLock;

	bool						m_isConsolePrint; // 콘솔에 출력하는가?
	Tick						m_flushTick;
};

extern void LOG( int feature, const TCHAR* fmt, ... );

} // gk 

#define NS_LOG_RETURN_IF( cond, msg ) \
	if ( (cond) ) { kcore::LOG( kcore::FT_INFO, msg ); return; }

#define NS_LOG_RETURN_IF_NOT( cond, msg ) \
	if ( !(cond) ) { kcore::LOG( kcore::FT_INFO, msg ); return; }

#define NS_LOG_RETURN_VAL_IF( cond, v, msg ) \
	if ( (cond) ) { kcore::LOG( kcore::FT_INFO, msg ); return v; }

#define NS_LOG_RETURN_VAL_IF_NOT( cond, v, msg ) \
	if ( !(cond) ) { kcore::LOG( kcore::FT_INFO, msg ); return v; }


