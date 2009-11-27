#pragma once 

#include <kcore/sys/Tick.h>
#include <kcore/sys/FineTick.h>
#include <kserver/cell/Action.h>

#include <hash_map>

namespace gk {

/**
 * @class ContextDispatcher 
 *
 */
class ContextDispatcher 
{
public:
	ContextDispatcher();
	~ContextDispatcher();

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
	 * @param ctx The context to handle
	 * @param action The action to dispatch
	 */
	void Subscribe( const tstring& ctx, ActionPtr action );

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
	typedef stdext::hash_map<tstring, ActionList> ActionMap;

	void processTick();
	void processCleanup();

private:
	ActionMap m_actions;
	Tick 	  m_tickCleanup;
	FineTick  m_tick;
};

} // gk


