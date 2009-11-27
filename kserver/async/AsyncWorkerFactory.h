#pragma once 

#include <kserver/async/AsyncWorker.h>

#include <hash_map>

namespace gk 
{

/**
 * @class AsyncWorkerFactory 
 *
 */
class AsyncWorkerFactory 
{
private:
	AsyncWorkerFactory();

public:
	static AsyncWorkerFactory* Instance();
	~AsyncWorkerFactory();

	/**
	 * Register a factory prototype
	 */
	void Register( const tstring& cls, AsyncWorker* proto );

	/**
	 * Create an AsyncWorker with cls
	 */
	AsyncWorker* Create( const tstring& cls );
	AsyncWorker* Create( const char* cls );

private:
	typedef stdext::hash_map<tstring, AsyncWorker*> AsyncWorkerMap;

	void cleanup();

private:
	AsyncWorkerMap 	m_workers;
	Mutex			m_lock;

};

} // namespace gk

#define REGISTER_WORKER( cls, w ) \
AsyncWorkerFactory::Instance()->Register( (cls), (w) )