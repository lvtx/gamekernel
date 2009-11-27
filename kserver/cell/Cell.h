#pragma once 

#include <kcore/sys/Atomic.h>
#include <kcore/sys/Tick.h>
#include <knet/NetSecurity.h>
#include <kserver/cell/CellRunner.h>
#include <kserver/cell/CellTree.h>
#include <kserver/cell/ContextDispatcher.h>
#include <kserver/cell/TypeDispatcher.h>
#include <kserver/cell/TransDispatcher.h>

#include <vector>

class TiXmlElement;

namespace gk
{
class Node;
class Cell;

typedef std::map<CellId, Cell*> CellList;

/**
 * @class Cell
 *
 * Responsibility:
 *  - Starts a cell 
 *  - Active or Passive decided by parent cell or by subclass
 *  - Handles CellTree messages
 *  - Handles Object messages (migration, replication, persistence)
 */
class Cell 
{
public:
	Cell();
	virtual ~Cell();

	/** 
	 * @brief Initialize with local parent 
	 *
	 * @param node Node under this cell is made
	 * @param parent The parent Cell
	 * @param id The cell id
	 * @param xcfg The configuration point 
	 * @param active If true, a thread is used to run myself
	 * @return true if successful
	 */
	bool Init( Node* node, 
			   Cell* parent, 
			   const CellId& id, 
			   TiXmlElement* xcfg = 0, 
			   bool active = false );	

	/**
	 * @brief Send interface to add src address
	 *
	 * @param m A message to send. 
	 */
	void Send( MessagePtr m );

	/**
	 * @brief Receive a message 
	 *
	 * Called by node or other to send to this cell 
	 *
	 * @param m A message to recv
	 */
	void Notify( MessagePtr m );

	/**
	 * Run 
	 */
	void Run();

	/**
	 * Awake CellRunner
	 */
	void MakeActive();

	/**
	 * Make CellRunner sleep and runs under Node thread
	 */
	void MakePassive();

	/**
	 * Called back when a transaction is completed 
	 */
	void Completed( Transaction* trans );

	/// udp {
	uint CreateUdpGroup( SecurityLevel sl = SECURITY0 );
	void JoinUdpGroup( uint groupId, uint connectionId, const tstring& extra );
	void LeaveUdpGroup( uint groupId, uint connectionId );
	void DestroyUdpGroup( uint groupId );
	/// }
	
	/**
	 * Cleanup
	 */
	void Fini();

	/**
	 * Returns up or down state
	 */
	bool IsUp( const CellId& id );

	/**
	 * Is this cell active
	 */
	bool IsActive() const;

	/**
	 * Set target cell to update my state
	 */
	void SetUpdateId( const CellId& id );

	/**
	 * Clear process count in CellRunner
	 */
	void ResetProcessCount();

	/**
	 * Get process count to yield
	 */
	uint GetProcessCount() const;

	/**
	 * Get cell id 
	 */
	const CellId& GetId() const;

	/**
	 * Get class
	 */
	const tstring& GetCls() const;

	Node* GetNode();

	/**
	 * Factory prototype 
	 */
	virtual Cell* Create() = 0;

protected:
	typedef Queue<Transaction*, Mutex> TransQ;
	
	void processActivation();
	void processMailbox();
	void processCells();
	void processCluster();
	void processTrans();

	void onCellState( MessagePtr m );
	void onNetState( MessagePtr m );

	bool loadConfig( TiXmlElement* xcfg );
	bool loadCells( TiXmlElement* xe );
	bool loadCell( TiXmlElement* xe );

	virtual bool init( TiXmlElement* root );
	virtual void run();
	virtual void onMessage( MessagePtr m );
	virtual void fini();

protected:
	CellRunner 		m_runner;
	MessageQ 		m_mailbox; 		// mailbox
	Atomic<uint> 	m_activeFlag; 	// 0 - none, 1 - make active, 2 - make passive
	Node*			m_node;			// The node
	Cell*			m_parent;		// local parent
	CellId			m_id;			// my cell id
	CellTree   		m_celltree; 	// the cell tree information synchronized 
	CellList 		m_cells; 		// child cells

	tstring			m_name;			// name for human readable name
	tstring 		m_cls; 			// cell class
	uint 			m_processCount;
	CellId 			m_updateId; 	// cell state update id (broadcast or specific cell)
	Tick 			m_tickUpdate;
	uint 			m_updateInterval;
	TransQ 			m_transQ;

	TypeDispatcher 		m_typeDispatcher;
	ContextDispatcher 	m_ctxDispatcher;
	TransDispatcher 	m_transDispatcher;
};

inline
void 
Cell::SetUpdateId( const CellId& id )
{
	K_ASSERT( id.IsValid() );

	m_updateId = id;
}

inline
void 
Cell::ResetProcessCount()
{
	m_processCount = 0;
}

inline
uint 
Cell::GetProcessCount() const
{
	return m_processCount;
}

inline
bool 
Cell::IsActive() const
{
	return m_runner.IsRunning();
}

inline
const CellId& 
Cell::GetId() const
{
	return m_id;
}

inline
const tstring& 
Cell::GetCls() const
{
	return m_cls;
}

inline
Node* 
Cell::GetNode()
{
	K_ASSERT( m_node != 0 );

	return m_node;
}

} // gk

