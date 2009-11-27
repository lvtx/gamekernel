#pragma once 

#include <kcore/mem/AllocatorAware.h>
#include <knet/cipher/Cipher.H>
#include <knet/NetSecurity.h>
#include <knet/socket/IpAddress.h>
#include <knet/group/NetGroupMember.h>

#include <hash_map>

namespace gk {

class TcpCommunicator;

/**
 * @class NetGroup 
 *
 * How network group with UDP works:
 *
 * [1] connectionId is used as a tag for UdpConnection on clients
 *
 * [2] client endpoint of the connectionId is used to find internal address.
 *     TCP/UDP endpoint can be used at the same time. 
 *     This ensures the hole is punched to server already for some NATs.
 *
 * [3] Client connects to other clients when Join message is received. 
 *     This starts up hole punching and address resolution with tag. 
 *     (Tag is the connection id on the same server, so it is unique in the group)
 *
 * [4] Supposing that just one group is used on client, the above flow works. 
 *     Extension to provide multiple groups will not be hard, but online games 
 *     don't require this.
 *
 * [5] While UDP connection is made, the connection to server is used to relay
 *     in UdpCommunicator.
 */
class NetGroup : public AllocatorAware
{
public:
	NetGroup();
	~NetGroup();

	/**
	 * Intialize NetGroup 
	 *
	 * @param communicator TcpCommunicator for tcp communication 
	 * @param id The id of this group
	 * @param sl The SecurityLevel of this group
	 * @return true if successful
	 */
	bool Init( TcpCommunicator* communicator, uint id, SecurityLevel sl );

	/**
	 * Joins a new tcp connection to the group
	 *
	 * @param connectionId The tcp connection id of the client
	 * @param ex External ipaddress of the client to join
	 * @param extra Extra parameter to send to client
	 * @return true if join successful
	 */
	bool Join( uint connectionId, const IpAddress& ex, const tstring& extra );

	/**
	 * Leaves the group
	 *
	 * @param connectionId The TCP connection id to leave
	 */
	void Leave( uint connectionId );

	/**
	 * Called when client is prepared 
	 *
	 * @param connectionId The tcp connection id
	 * @param in The internal address of the joined client 
	 */
	void OnPrepared( uint connectionId, const IpAddress& in );

	/**
	 * Finishes this group
	 *
	 * Clients are notified.
	 */
	void Fini();

	/**
	 * @return group id 
	 */
	uint GetId() const;

	/**
	 * @return remote connection id list
	 */
	const std::vector<uint> GetRemotes() const;

private:
	typedef stdext::hash_map<uint, NetGroupMember> MemberMap;

	void sendPrepare( const NetGroupMember& m );
	void sendJoin( const NetGroupMember& m );
	void sendLeave( const NetGroupMember& m );
	void sendDestroy();

private:
	TcpCommunicator*  	m_communicator;
	uint 				m_id;
	SecurityLevel 		m_securityLevel;
	MemberMap 			m_members;
	std::vector<uint> 	m_remotes;

	Cipher 				m_cipher;
};

inline
uint 
NetGroup::GetId() const
{
	return m_id;
}

inline
const std::vector<uint> 
NetGroup::GetRemotes() const
{
	return m_remotes;
}

} // gk
