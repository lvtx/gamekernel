#pragma once 

#include <kcore/sys/Queue.h>
#include <kserver/db/DbPool.h>
#include <kserver/db/Transaction.h>

namespace gk { 

class Node;

/**
 * @class TransactionRunner 
 *
 * Run requested transactions
 */
class TransactionRunner : public Thread 
{
public:
	TransactionRunner();
	~TransactionRunner();

	/**
	 * @brief Initialize runner
	 *
	 */
	bool Init( Node* node, DbPool* pool );

	/**
	 * Request transaction 
	 */
	void Request( Transaction* tx );

	/**
	 * Thread::Run
	 */
	int Run();

	/**
	 * Clean up
	 */
	void Fini();

	/**
	 * Get outstanding transaction count 
	 */
	uint GetTransactionCount() const;

private:
	typedef Queue<Transaction*, Mutex> TransQ;

	void processTransQ();
	void processTrans( Transaction* tx );

private:
	Node* 			m_node;
	DbPool* 		m_pool;

	TransQ 			m_transQ;
	TransQ			m_resultQ;
	Atomic<uint>	m_transCount; 
};

} // gk
