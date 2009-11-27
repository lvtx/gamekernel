#pragma once 

#include <kcore/sys/Thread.h>
#include <kcore/sys/Queue.h>
#include <knet/socket/IpAddress.h>

namespace gk {

class TcpCommunicator;

/** 
 * @class Connector
 *
 * Connector to connect to remote host. 
 * 
 * ConnectEx is not used. Thread approach is much simpler. 
 */
class Connector : public Thread
{
public:
	Connector();
	virtual ~Connector();

	/**
	 * Initialize a connector
	 *
	 * @param communicator The TcpCommunicator to notify
	 * @return true if successful
	 */
	bool Init( TcpCommunicator* communicator );

	/**
	 * Thread::Run
	 */
	int Run();

	/**
	 * Request to connect to remote
	 *
	 * @param remote The ip:port to connect 
	 */
	void Connect( const IpAddress& remote );

	/**
	 * Stop and clean up
	 */
	void Fini();

private:
	typedef Queue<IpAddress, Mutex> ReqQ;

	TcpCommunicator* m_communicator;
	ReqQ 	m_reqQ;
};

} // gk

