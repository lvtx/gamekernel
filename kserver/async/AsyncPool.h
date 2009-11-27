#pragma once 

#include <kserver/async/AsyncGroup.h>
#include <kcore/sys/ScopedLock.h>

#include <hash_map>

namespace gk 
{
/**
 * @class AsyncPool 
 */
class AsyncPool 
{
public:
	AsyncPool();
	~AsyncPool();

	/**
	 * Initialize pool 
	 *
	 * @return true on success
	 */
	bool Init( Node* node, bool luaMode );

	/**
	 * Create AsyncGroup 
	 */
	bool CreateGroup( const tstring& cls, const tstring& name, uint count );

	/**
	 * Notify a message to a group
	 */
	void Notify( const tstring& name, MessagePtr m );

	/**
	 * Destroy a group 
	 */
	void DestroyGroup( const tstring& name );

	/**
	 * Cleanup all groups
	 */
	void Fini();

private:
	typedef stdext::hash_map<tstring, AsyncGroup*> GroupMap;

	Node* 	 m_node;
	bool	 m_luaMode;
	GroupMap m_groups;
	Mutex 	 m_lock;
};

} // namespace gk
