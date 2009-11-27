#pragma once 

#include <kcore/mem/AllocatorAware.h>
#include <knet/message/Message.h>

namespace gk 
{

class Action;
class ActionHsm;	

/**
 * @class ActionState 
 *
 * @brief State for Action HSM 
 */
class ActionState : public AllocatorAware
{
public:

	/**
	 * HSM node to rember patch
	 */
	struct Node 
	{
		enum Code
		{
			  NOOP = 1		// nothing to do
			, EXIT			// exit the state
			, ENTER			// enter the state
			, TARGET		// target state
		};
		
		Code			code;
		ActionState*	state;
	};

	typedef std::vector<Node> Path;

public:
	ActionState();
	virtual ~ActionState();

	/**
	 * @brief Initialize state 
	 *
	 * @param hsm The HSM 
	 * @param id The id 
	 * @param parent The parent state
	 * @return true if successful
	 */
	virtual bool Init( ActionHsm* hsm, int id, ActionState* parent );

	/**
	 * @brief Called when enter 
	 */
	virtual void OnEnter();

	/**
	 * @brief Called when a message arrives
	 *
	 * @param m The message
	 * @return The parent state or 0. 
	 */
	virtual ActionState* OnMessage( MessagePtr m );

	/**
	 * @brief Called periodically
	 *
	 * @param tick Elapsed time in milliseconds
	 * @return The parent state or 0 
	 */
	virtual ActionState* OnTick( uint tick );

	/**
	 * @brief Called when exit this state
	 */
	virtual void OnExit();

	/**
	 * @brief Finishes 
	 */
	virtual void Fini();

	/**
	 * @brief Check whether there is id child state including me
	 *
	 * @param id The state id to find 
	 * @return true if a state is found
	 */
	bool HasState( int id ) const;
	
	/**
	 * @return The state id
	 */
	int GetId() const;

	/**
	 * @brief Find a child state or me with id 
	 *
	 * @return The state with id
	 */
	ActionState* GetState( int id );

	/**
	 * @brief Get the transition path to th target state
	 *
	 * @param target The target state id to transit from here or from parent
	 * @param path The path to the target 
	 * @return true if target state is found somewhere i.e. path is not empty
	 */
	bool GetTransitionPathTo( int target, Path& path );

protected:
	typedef std::vector<ActionState*> StateList;

	void attach( ActionState* state );
	bool getChildPathTo( int target, Path& path );

protected:
	ActionHsm*	 	m_hsm;
	int		  		m_id;
	ActionState*    m_parent;
	StateList 		m_childs;
	int				m_targetId;		
};
} // gk

