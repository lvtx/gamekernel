#pragma once 

#include <kcore/sys/Atomic.h>
#include <knet/NetClient.h>
#include <kserver/test/TestCreator.h>

class TiXmlElement;

namespace gk {

/**
 * @class TestClient 
 *
 */
class TestClient : public Thread, public MessageListener
{
public:
	TestClient();
	~TestClient();

	/**
	 * Initialize
	 */
	bool Init( TestCreator* creator, uint idx, TiXmlElement* xe );

	/**
	 * Thread::Run 
	 */
	int Run();

	/**
	 * Finish
	 */
	void Fini();

	/**
	 * MessageListener::Notify
	 */
	void Notify( MessagePtr m );

	/**
	 * Check whether TestAction is finished
	 */
	bool IsFinished() const;

private:
	NetClient  		m_client;
	TestAction* 	m_action; 		// a test action
	Atomic<bool> 	m_finished;
	MessageQ 		m_recvQ;
};

inline
bool 
TestClient::IsFinished() const
{
	return m_finished;
}

} // gk
