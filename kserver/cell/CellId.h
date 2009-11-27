#pragma once 

#include <knet/message/BitStream.h>

#include <vector>

namespace gk {

/**
 * @struct CellId 
 *
 * Responsibility:
 *  [1] All cell messages have CellId 
 *  [2] CellId is used to find target node 
 *  [3] A Cell has a CellId
 *  [4] 4 level cells are used. This will be sufficient from casual games to MMORPGs.
 */
struct CellId 
{
	static const ushort BROADCAST = 0x0FFF;
	static const ushort LEVELS    = 3;

	// 12 bits up to 4096
	ushort N; 		// Node
	ushort L0; 		// level 0
	ushort L1; 		// level 1
	ushort L2; 		// level 2

	CellId();
	CellId( ushort N, ushort L0, ushort L1 = 0, ushort L2 = 0 );

	CellId GetLevel( ushort level ) const;
	ushort GetLevel() const;

	ushort GetOneLevel( ushort level ) const;
	void SetOneLevel( ushort level, ushort id );

	bool IsValid() const;
	bool IsL0() const;
	bool IsL1() const;
	bool IsL2() const;

	/// Is the address broadcast for level and is that the final level?
	bool IsFinalBroadcast( ushort level ) const;

	/// This enables CellId can be used as a key for std::map
	bool operator<( const CellId& o ) const;

	/// comparison
	bool operator==( const CellId& o ) const;
	bool operator!=( const CellId& o ) const;

	/// convert to string for debugging
	const tstring ToString() const;

	bool Pack( BitStream& bs );
	bool Unpack( BitStream& bs );
};

typedef std::vector<CellId> CellIdList;

inline
CellId::CellId()
: N(0)
, L0(0)
, L1(0)
, L2(0)
{
}

inline
CellId::CellId( ushort N, ushort L0, ushort L1, ushort L2 )
{
	K_ASSERT( N  >= 0 && N  < 4096 );
	K_ASSERT( L0 >= 0 && L0 < 4096 );
	K_ASSERT( L1 >= 0 && L1 < 4096 );
	K_ASSERT( L2 >= 0 && L2 < 4096 );

	this->N = N; 
	this->L0 = L0;
	this->L1 = L1;
	this->L2 = L2;

	K_ASSERT( IsValid() );
}

inline
bool 
CellId::operator<( const CellId& o ) const
{
	if ( N < o.N ) 		return true;
	if ( L0 < o.L0 ) 	return true;
	if ( L1 < o.L1 ) 	return true;

	return L2 < o.L2;
}

inline
bool 
CellId::operator==( const CellId& o ) const
{
	return ( N == o.N && L0 == o.L0 && L1 == o.L1 && L2 == o.L2 );
}

inline
bool 
CellId::operator!=( const CellId& o ) const
{
	return !operator==( o );
}

} // gk
