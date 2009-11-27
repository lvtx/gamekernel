#pragma once

#include <kcore/sys/ScopedLock.h>
#include <kserver/cell/Cell.h>

#include <hash_map>

namespace gk 
{

/**
 * @class CellFactory 
 *
 * A factory for cells
 */
class CellFactory
{
public:
	static CellFactory* Instance();

	~CellFactory();

	/**
	 * @brief Register a prototyp for cls
	 *
	 * @param cls Class name 
	 * @param prototyp Cell 
	 */
	void Register( const tstring& cls, Cell* prototype );

	/**
	 * @brief Creates a cell from a prototype 
	 *
	 * @return The created cell. 0 if protype is not found
	 */
	Cell* Create( const tstring& cls );

	/**
	 * @brief Creates a cell from a prototype 
	 *
	 * @return The created cell. 0 if protype is not found
	 */
	Cell* Create( const char* cls );

private:
	typedef stdext::hash_map<tstring, Cell*> CellMap;

	CellFactory();

	void cleanup();

private:
	CellMap 	m_cells;
	Mutex		m_lock;
};

} // gk 

#define REGISTER_CELL( cls, m ) \
	gk::CellFactory::Instance()->Register( (cls), (m) )

