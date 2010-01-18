#pragma once 

namespace bt 
{

class Entity;
class BehaviorSelector;

/**
 * @class BehaviorDispatcher 
 *
 * Behavior 실행기에 해당한다. 
 */
class BehaviorDispatcher 
{
public:
	BehaviorDispatcher();
	~BehaviorDispatcher();

	/**
	 * 주인이 되는 엔티티와 동작 선택기를 갖고 시작한다. 
	 *
	 * @param owner 	주인이 되는 엔티티 
	 * @param selector 	동작 선택기
	 *
	 * @return 정상적이면 true
	 */
	bool Init( Entity* owner, BehaviorSelector* selector );

	/**
	 * 주기적인 처리를 한다. 
	 *
	 * @param tick 		지나간 시간값
	 */
	void Update( float tick );

	/**
	 * 메세지 전파
	 *
	 * @param m 		메세지. <소유권 이전 없음>
	 */
	void Message( Message* m );

	/**
	 * 종료한다
	 */
	void Fini();

private:
	BehaviorQueue 		m_queue;
	BehaviorSelector* 	m_selector;
};

} // namespace bt
