#pragma once 

#include <kcore/sys/Tick.h>
#include <kcore/sys/FineTick.h>
#include <kserver/cell/Action.h>
#include <kserver/db/Transaction.h>

#include <hash_map>

namespace gk {

/**
 * @class TypeDispatcher 
 *
 * @brief Dispatches message or processing power to actions 
 *
 * When message is arrived, 
 *
 * [1] Hash map is searched for the type
 * [2] If an Action is found, then the message is dispatched to the action 
 * [3] If an Action is finished, it is checked and cleaned up later in Run()
 */
class TypeDispatcher 
{
public:
	TypeDispatcher();
	~TypeDispatcher();

	/**
	 * @brief Initialize 
	 */
	bool Init();

	/**
	 * @brief Run periodically
	 */
	void Run();

	/**
	 * @brief Subscribe for context
	 *
	 * @param type The type to handle
	 * @param action The action to dispatch
	 */
	void Subscribe( ushort type, ActionPtr action );

	/**
	 * @brief Dispatch Message
	 *
	 * @param m The message to dispatch
	 */
	void Dispatch( MessagePtr m );

	/**
	 * @brief Clean up 
	 */
	void Fini();

private:
	typedef std::vector<ActionPtr> ActionList;
	typedef stdext::hash_map<uint, ActionList> ActionMap;

	void processTick();
	void processCleanup();

private:
	ActionMap	m_actions;
	Tick 		m_tickCleanup;
	Tick		m_tick;
};

} // gk

