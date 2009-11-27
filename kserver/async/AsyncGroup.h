#pragma once 

#include <kserver/async/AsyncWorker.h>

#include <vector>

namespace gk 
{
/**
 * @class AsyncGrouop 
 */
class AsyncGroup
{
public:
	AsyncGroup();
	~AsyncGroup();

	bool Init( Node* node, const tstring& name, const tstring& cls, uint count, bool luaMode );

	void Notify( MessagePtr m );

	void Fini();

private:
	typedef std::vector<AsyncWorker*> WorkerList;

	bool		m_luaMode;
	WorkerList  m_workers;
	tstring 	m_name;

};

} // namespace gk
