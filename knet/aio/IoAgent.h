#pragma once 

#include <knet/aio/IoBlock.h>

namespace gk {

/**
 * @class IoAgent 
 *
 * IoAgent interface which can work with IoService. 
 */
class IoAgent 
{
public:
	virtual ~IoAgent() {}

	/**
	 * Get handle from this agent
	 *
	 * @return HANDLE to the IO
	 */
	virtual HANDLE RequestHandle() = 0;

	/**
	 * Called on IO error
	 *
	 * @param ec Error code
	 * @param io IoBlock being processed
	 */
	virtual void OnIoError( int ec, IoBlock* io ) = 0;

	/**
	 * Requests send 
	 *
	 * @return true if successful
	 */
	virtual bool RequestSend(); 

	/**
	 * Requests recv 
	 *
	 * @return true if successful
	 */
	virtual bool RequestRecv();

	/**
	 * Called when send completed
	 *
	 * @param io IoBlock being processed
	 */
	virtual void OnSendCompleted( IoBlock* io );

	/**
	 * Called when send completed, but not complete
	 *
	 * @param io IoBlock being processed
	 * @param bytesSent The length of bytes sent at last operation
	 */
	virtual void OnSendCompleted( IoBlock* io, uint bytesSent );

	/**
	 * Called when recv completed
	 *
	 * @param io IoBlock being processed
	 */
	virtual void OnRecvCompleted( IoBlock* io );

	/**
	 * Not used. 
	 */
	virtual void OnAcceptCompleted( IoBlock* io );
};

inline
bool 
IoAgent::RequestSend()
{
	K_ASSERT( !_T("IoAgent::RequestSend> Not implemented") );

	return false;
}

inline
bool 
IoAgent::RequestRecv()
{
	K_ASSERT( !_T("IoAgent::RequestRecv> Not implemented") );

	return false;
}

inline
void 
IoAgent::OnSendCompleted( IoBlock* )
{
	K_ASSERT( !_T("IoAgent::OnSendCompleted> Not implemented") );
}

inline
void 
IoAgent::OnSendCompleted( IoBlock* , uint )
{
	K_ASSERT( !_T("IoAgent::OnSendCompleted> Not implemented") );
}

inline
void 
IoAgent::OnRecvCompleted( IoBlock*  )
{
	K_ASSERT( !_T("IoAgent::OnRecvCompleted> Not implemented") );
}

inline
void
IoAgent::OnAcceptCompleted( IoBlock*  )
{
	K_ASSERT( !_T("IoAgent::OnAcceptCompleted> Not implemented") );
}

} // gk 
