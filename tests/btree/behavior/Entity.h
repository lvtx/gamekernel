#pragma once 

#include <behavior/BehaviorDispatcher.h>
#include <behavior/BehaviorSelector.h>

namespace bt 
{

/**
 * @class Entity 
 *
 * 간략한 행동 클래스를 나타낸다.
 *
 * 내부에 BehaviorDispatcher를 하나 갖는다.
 */
class Entity 
{
public:
	Entity();
	virtual ~Entity();

	/**
	 * 동작을 선택할 수 있도록 하면서 초기화한다
	 *
	 * @param 	selector 
	 */
	virtual bool Init( BehaviorSelector* selector );

	/**
	 * 주기적인 처리를 한다. 
	 */
	virtual void Update( float tick );

	/**
	 * 메세지를 전파한다.
	 */
	virtual void Message( Message* m );

	/**
	 * 엔티티를 종료한다. 
	 */
	virtual void Fini();

protected:
	BehaviorDispatcher 	m_behaviorDispatcher;
	BehaviorSelector* 	m_selector;
};

} // namespace bt
