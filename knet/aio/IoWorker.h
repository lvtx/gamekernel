#pragma once 

#include <kcore/sys/Thread.h>

namespace gk {
/** 
 * @class IoWorker
 */
class IoWorker : public Thread
{
public:
    IoWorker();

    ~IoWorker();

    /** 
     * Prepare for io and start thread
	 *
	 * @param port HANDLE to iocp port
     */
    bool Init( HANDLE port );

    /**
     * Thread function 
     */
    int Run();

    /** 
     * Stop thread and clean up
     */
    void Fini(); 

private:
    void handleRcZero( ulong bytes, ulong perIoKey, LPOVERLAPPED ov );
    void handleRcNonZero( ulong bytes, ulong perIoKey, LPOVERLAPPED ov );
    void helpStop();

private:
    HANDLE port_;
};

} // gk 

