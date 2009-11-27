#pragma once

#include <kcore/sys/Thread.h>
#include <knet/socket/Socket.h>
#include <knet/message/Message.h>
#include <knet/NetSecurity.h>

namespace gk { 

class TcpCommunicator;

/**
 * @class Acceptor
 *
 * Accepts client connections. 
 *
 * AcceptEx is not used. Thread approach is much simpler. 
 * 
 */
class Acceptor : public Thread
{
public:
    Acceptor();
    virtual ~Acceptor();

	/**
	 * Initialize an Acceptor
	 *
	 * @param communicator The tcp communicator
	 * @param socket The socket listening
	 * @param sl The security level
	 * @return true if successful
	 */
    bool Init( TcpCommunicator* communicator, Socket* socket, SecurityLevel sl = SECURITY0 );

	/**
	 * Thread::Run
	 */
    int Run();

	/**
	 * Stop and clean up
	 */
    void Fini();

private:
    void helpStop();

private:
	TcpCommunicator* m_communicator;
	Socket*          m_socket;    
	SecurityLevel    m_sl;
};

} // gk

