#pragma once 

#include <kcore/sys/Atomic.h>
#include <kcore/sys/Thread.h>
#include <knet/message/Message.h>

namespace gk 
{
class Node;

/**
 * @class AsyncWorker 
 *
 */
class AsyncWorker : public Thread
{
public:
	AsyncWorker();
	~AsyncWorker();

	/**
	 * Initialize
	 *
	 * @param node Node of this cluster
	 * @return true on success
	 */
	bool Init( Node* node );

	/**
	 * Thread::Run()
	 */
	int Run();

	/**
	 * Notified of a message
	 *
	 * @param m Message shared pointer
	 */
	void Notify( MessagePtr m );

	/**
	 * Stop thread and cleans up
	 */
	void Fini();

	/// Prototype for factory
	virtual AsyncWorker* Create() = 0;

protected:
	void helpStop();

	virtual bool init();
	virtual void onMessage( MessagePtr m );
	virtual void fini();

protected:
	Node* 	m_node;
	HANDLE 	m_port;
	MessageQ m_recvQ;

};

} // namespace gk
