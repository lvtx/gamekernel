#pragma once 

namespace gk 
{

/**
 * @struct IoBlock 
 *
 * IOCP overlapped block for asynchronous io
 */
struct IoBlock : public OVERLAPPED
{
    enum OpCode
    {
        OP_READ = 1, 
        OP_WRITE
    };

    WSABUF      buf;            ///< Async buffer. Must come first
    OpCode      op;             ///< operation
    ulong       transferred;    ///< transferred length 
    ulong       totalLen;       ///< total length of bytes
    sockaddr_in remote;         ///< address for UDP
    uint        remoteLen;      ///< remote address length
    void*       extra;          ///< save extra data to process. IoBlock at current impl.

    IoBlock()
        : op(), 
		  transferred( 0 ), 
          totalLen( 0 ), 
		  remote(), 
		  remoteLen( 0 ), 
		  extra( 0 )
    {
        buf.buf = 0;
        buf.len = 0;

        ::memset( (void*)&remote, 0, sizeof( sockaddr_in ) );
    }

	~IoBlock()
	{
	}
};

} // gk 
