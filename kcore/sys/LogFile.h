#pragma once 

#include <kcore/sys/DateTime.h>

namespace gk 
{
/**
 * @class LogFile 
 *
 */
class LogFile 
{
public:
    LogFile();
    ~LogFile();

	/**
	 * Open log file with prefix. Appends date and time. 
	 *
	 * @param prefix for log file
	 * @return true when succeeds
	 */
    bool Open( const tstring& prefix );

	/**
	 * Write log to the file
	 *
	 * @param line string to write
	 */
	void Write( const std::string& line );

	/**
	 * Check whether log file is open
	 */
    bool IsOpen() const;

	/**
	 * Flush the content to log file
	 */
	void Flush();

	/**
	 * Close the log file
	 */
    void Close();

private:
    void makeFileName();
    void checkRotate();

private:
    tstring     m_prefix;
    tstring     m_filename;
	HANDLE		m_handle;
    DateTime    m_date;
};

inline
bool 
LogFile::IsOpen() const
{
    return m_handle != 0;
}

} // gk 
