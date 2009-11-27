#pragma once 

#include <kcore/sys/Thread.h>

#include <vector>

namespace gk {

class Cell;

/**
 * @class CellRunner 
 *
 * Responsibility:
 *  [1] Runs a Cell if asked 
 *  [2] Pause running a Cell if asked
 */
class CellRunner : public Thread 
{
public:
	CellRunner();
	~CellRunner();

	/**
	 * @brief Initialize a cell runner
	 *
	 * @param cell A Cell to run 
	 * @return true if successful
	 */
	bool Init( Cell* cell );

	/**
	 * @brief Make this run the cell
	 *
	 * @return true if successful
	 */
	bool MakeActive();

	/**
	 * @brief Do not run the cell
	 *
	 * @return true if successful
	 */
	bool MakePassive();

	/// Thread::Run
	int Run();

	/**
	 * @brief Cleans up 
	 */
	void Fini();

private:
	Cell*		 m_cell;
};

} // gk
