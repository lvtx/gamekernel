#pragma once 

#include <kserver/cell/CellId.h>
#include <map>

namespace gk {

/**
 * @class CellTree 
 *
 * Responsibility: 
 *  [1] Keeps the whole topology information 
 *  [2] Updates periodically or application provided point of update 
 *  [3] Not all level Cells will be updated. It can be decided by application.
 *      For example, Room Cells just need to report its status to Channel Cell.
 */
class CellTree 
{
public:
	struct CellNode 
	{
		typedef std::map<CellId, CellNode> CellNodeList;

		CellId  		id;
		bool 			up; 			// up/down state
		CellNodeList 	cells; 			// child cells

		CellNode()
		: id()
		, up( false )
		{
		}
	};

public:
	CellTree();
	~CellTree();

	/**
	 * @brief Initialize
	 *
	 * @return true if successful
	 */
	bool Init();

	/**
	 * @brief Add or update CellNode 
	 *
	 * @param cn A CellNode to add
	 */
	void Update( const CellNode& cn );

	/** 
	 * @brief Update Cell state
	 *
	 * @param id A cell to update
	 * @param up Is up?
	 */
	void UpdateState( const CellId& id, bool up );

	/** 
	 * @brief Remove cell 
	 */
	void Remove( const CellNode& cn );

	/**
	 * @brief Remove cell 
	 */
	void Remove( const CellId& id );

	/**
	 * @brief Find a CellNode 
	 *
	 * @param r The found cell
	 * @param id The cell id to find
	 * @return true if found
	 */
	bool Find( CellNode& r, const CellId& id );

	/**
	 * @brief Clean up
	 */
	void Fini();

private:
	void update( ushort level, CellNode& parent, const CellNode& cn );
	void remove( ushort level, CellNode& parent, const CellId& id );

	bool find( CellNode& r, ushort level, CellNode& parent, const CellId& id );

private:
	CellNode m_root; 	// dummy node to have child cells
};

} // gk
