#pragma once 

#include <kcore/sys/Atomic.h>
#include <kserver/db/DbPool.h>
#include <kserver/db/TransactionRunner.h>

class TiXmlElement;

namespace gk {

class Node;

/**
 * @class TransactionServer 
 *
 * Role:
 *  Starts transaction runners
 *  Give transaction to proper runner 
 *  Clean up
 */
class TransactionServer 
{
public:
	TransactionServer();
	~TransactionServer();

	/**
	 * Initialize. Take ownership of pool 
	 */
	bool Init( Node* node, DbPool* pool );

	uint Prepare( Transaction* tx );

	void Request( Transaction* tx );

	void Fini();

private:
	enum 
	{
		RUNNER_COUNT = 5
	};

private:
	Node* 			  m_node;
	DbPool* 		  m_pool;

	TransactionRunner m_runners[RUNNER_COUNT];
	Atomic<uint>	  m_nextTxNo; 	// next transaction no
};

} // gk

#define REQUEST_TRANSACTION( tx ) \
	gk::TransactionServer::Instance()->Request( (tx) )
