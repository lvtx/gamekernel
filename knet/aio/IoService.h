#pragma once 

#include <kcore/base/Noncopyable.h>
#include <knet/aio/IoAgent.h>
#include <knet/aio/IoBlock.h>
#include <knet/aio/IoWorker.h>

namespace gk {

/** 
 * @class IoService
 */
class IoService : private Noncopyable
{
public:
    IoService();
    ~IoService();

    /** 
     * Setup IoService and start AsyncIoWorkers
	 *
	 * @return true when successful
     */
    bool Init();

    /** 
     * Bind async io for the connection 
	 *
	 * @param agent IoAgent to bind IO operation
	 * @return true if binding is successful
     */
    bool BindIo( IoAgent* agent );

    /**
     * Unbind async io for the connection
	 *
	 * @param agent IoAgent to bind IO operation
     */
    void UnbindIo( IoAgent* agent );

    /**
     * Begin send on the connection 
	 *
	 * @param agent IoAgent to start IO
     */
    bool BeginSend( IoAgent* agent );

    /**
     * Begin recv on the connection
	 *
	 * @param agent IoAgent to start IO
     */
    bool BeginRecv( IoAgent* agent );

    /** 
     * Finish IoWorkers and IoService
     */
    void Fini();

private:
    enum 
    { 
        MAX_WORKERS = 4
    };

    uint getProperWorkerCount() const;

private:
    HANDLE 		m_port;
    IoWorker 	m_workers[MAX_WORKERS];
    int 		m_workerCount;
};

} // gk 

