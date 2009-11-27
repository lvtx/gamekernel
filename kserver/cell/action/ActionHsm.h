#pragma once 

#include <kserver/cell/action/ActionState.h>

namespace gk {

/**
 * @class ActionHsm 
 *
 * @brief A HSM for actions
 */
class ActionHsm 
{
public:
	ActionHsm();
	virtual ~ActionHsm();

	/**
	 * @brief Initialize states (override init)
	 *
	 * @param owener The action 
	 * @return true if successful
	 */
	bool Init( Action* owner );

	/**
	 * @brief Called when a message is received
	 *
	 * @param m A message to handle
	 */
	void OnMessage( const MessagePtr m );

	/**
	 * @brief Called to give processing power
	 *
	 * @param tick The elapsed time
	 */
	void OnTick( uint tick );

	/**
	 * @brief Transit to the target state
	 *
	 * @param targetState The target state id to transit 
	 */
	void Tran( int targetState );

	/**
	 * @brief Clean up
	 */
	void Fini();	

	/**
	 * Returns current ActionState
	 */
	ActionState* GetCurrent();

protected:
	virtual bool init() = 0;

	ActionState* getRoot();
	ActionState* getCurrent();

protected:
	Action* 	 m_owner;
	ActionState  m_root;		
	ActionState* m_current;	
};

inline
ActionState* 
ActionHsm::GetCurrent()
{
	return getCurrent();
}

inline
ActionState* 
ActionHsm::getRoot()
{
	return &m_root;
}

inline
ActionState* 
ActionHsm::getCurrent()
{
	return m_current;
}

} // gk

