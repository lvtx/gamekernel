#pragma once 

namespace bt 
{

class Behavior;

/**
 * @class BehaviorQueue
 */
class BehaviorQueue
{
public:
	BehaviorQueue();
	~BehaviorQueue();

	/**
	 * 큐를 초기화 한다. 
	 *
	 * @return 정상이면 true
	 */
	bool Init();

	/**
	 * 큐에 behavior를 추가한다
	 *
	 * @param behavior 		동작 <소유권 이전 없음>
	 */
	void Put( Behavior* behavior );

	/**
	 * 다음 behavior를 시작하는 등의 작업을 한다. 주기적으로 불림
	 */
	void Update();

	/**
	 * 종료한다. 
	 */
	void Fini();

private:
	// heap에서 swap만 생기기 때문에 빠르다	
	// [1] BehaviorComparison은 binary_function으로 구현한다. true일 때 더 높음
	typedef std::priority_queue< vector<Behavior*, BehaviorComparison> > PriorityQueue;

	PriorityQueue m_q;
};

} // namespace bt
