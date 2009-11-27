#pragma once 

#include <kserver/message/CellMessage.h>

namespace gk {

/**
 * @struct CmCellState 
 *
 * @brief Cell state message
 */
struct CmCellState : public CellMessage 
{
	// src is the cell who sent this message
	
	bool up; 

	CmCellState()
	: up( false )
	{
		type = CM_CELL_STATE;
	}

	bool Pack( BitStream& bs )
	{
		CellMessage::Pack( bs );

		bs.Write( up );

		return bs.IsValid();
	}

	bool Unpack( BitStream& bs )
	{
		CellMessage::Unpack( bs );

		bs.Read( up );

		return bs.IsValid();
	}

	Message* Create() 
	{
		return new CmCellState;
	}
};

} // gk

