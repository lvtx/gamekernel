#pragma once 

#include <kcore/sys/SharedPointer.h>
#include <knet/message/Message.h>
#include <kserver/cell/action/ActionHsm.h>

namespace gk {

class Transaction;
class Cell;

/**
 * @class Action 
 *
 * An instance based action handler 
 */
class Action 
{
public:
	Action();
	virtual ~Action();

	/**
	 * Initialize Action
	 *
	 * @param cell A Cell under this action runs
	 */
	bool Init( Cell* cell );

	/**
	 * Tick with elapsed time of Cell::Run 
	 *
	 * @param tick Elapsed time from last call
	 */
	bool Run( uint tick );

	/**
	 * Handle a message. Reactive logic
	 *
	 * @param m Message 
	 */
	bool Run( MessagePtr m );

	/**
	 * Transaction is completed
	 */
	bool Completed( Transaction* trans );

	/**
	 * Clean up
	 */
	void Fini();

	/**
	 * Check finished to clean up 
	 */
	bool IsFinished() const;

	/**
	 * Set timeout 
	 * @param ms Miliseconds to timeout 
	 */
	void SetTimeout( uint ms );

	/**
	 * Get timeout
	 */
	uint GetTimeout() const;

protected:
	virtual bool init();
	virtual bool run( uint tick );
	virtual bool run( MessagePtr m );
	virtual bool completed( Transaction* trans );
	virtual void timeout();
	virtual void fini();

	void setFinished();

protected:
	Cell*		m_cell;
	bool		m_finished;
	uint		m_timeout; 		// set this to > 0 to check timeout
	uint		m_age;
	tstring		m_name;			// for debugging
	ActionHsm*	m_hsm;			// To use hsm, set this during init
};

typedef SharedPointer<Action> ActionPtr;

inline
bool 
Action::IsFinished() const 
{
	return m_finished;
}

inline
void 
Action::SetTimeout( uint ms )
{
	m_timeout = ms;
}

inline
uint 
Action::GetTimeout() const
{
	return m_timeout;
}

} // gk
