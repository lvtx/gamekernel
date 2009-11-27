#pragma once 

#include <kserver/cell/CellId.h>
#include <kserver/db/DbPool.h>

namespace gk 
{

/**
 *
 */
class Transaction 
{
public:
	enum 
	{
		  SUCCESS
		, ERROR_CONNECTION
		, ERROR_EXECUTE
	};

public: 
	Transaction( uint type );
	virtual ~Transaction();

	void SetCellId( const CellId& cellId );
	const CellId& GetCellId() const;

	void SetTxNo( uint no );
	uint GetTxNo() const;

	uint GetType() const;

	virtual void Execute( DbPool* pool ) = 0;

protected:
	CellId	m_cellId;
	uint	m_txNo;
	uint 	m_type;
	uint 	m_result;
};

inline
void 
Transaction::SetCellId( const CellId& cellId )
{
	m_cellId = cellId;
}

inline
const CellId& 
Transaction::GetCellId() const
{
	return m_cellId;
}

inline
void
Transaction::SetTxNo( uint no )
{
	m_txNo = no;
}

inline
uint
Transaction::GetTxNo() const
{
	return m_txNo;
}

inline
uint 
Transaction::GetType() const
{
	return m_type;
}

} // gk
