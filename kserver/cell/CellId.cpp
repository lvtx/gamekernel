#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/CellId.h>

namespace gk {

CellId 
CellId::GetLevel( ushort level ) const
{
	K_ASSERT( 0 <= level && level <= LEVELS );

	CellId id;

	id.N = N;

	switch ( level )
	{
	case 0:
		id.L0 = L0;
		break;
	case 1:
		id.L0 = L0;
		id.L1 = L1;
		break;
	case 2:
		id.L0 = L0;
		id.L1 = L1;
		id.L2 = L2;
		break;
	}

	return id;
}

ushort 
CellId::GetOneLevel( ushort level ) const
{
	K_ASSERT( 0 <= level && level <= LEVELS );

	switch ( level )
	{
	case 0:
		return L0;
	case 1:
		return L1;
	case 2:
		return L2;
	}

	K_ASSERT( !_T("Invalid level") );

	return L2;
}

void 
CellId::SetOneLevel( ushort level, ushort id ) 
{
	K_ASSERT( 0 <= level && level <= LEVELS );
	K_ASSERT( id > 0 && id < 4096 );

	switch ( level )
	{
	case 0:
		L0 = id;
		break;
	case 1:
		L1 = id;
		break;
	case 2:
		L2 = id;
		break;
	}
}

ushort 
CellId::GetLevel() const
{
	if ( IsL0() ) { return 0; }
	if ( IsL1() ) { return 1; }

	return 2; 
}

bool 
CellId::IsValid() const
{
	return IsL0() || IsL1() || IsL2();
}

bool 
CellId::IsL0() const
{
	return ( L2 == 0 && L1 == 0 && L0 > 0 && N > 0 );
}

bool 
CellId::IsL1() const
{
	return ( L2 == 0 && L1 > 0 && L0 > 0 && N > 0 );
}

bool 
CellId::IsL2() const
{
	return ( L2 > 0 && L1 > 0 && L0 > 0 && N > 0 );
}

bool 
CellId::IsFinalBroadcast( ushort level ) const
{
	switch ( level )
	{
	case 0:
		return L0 == BROADCAST && L1 == 0 && L2 == 0;
	case 1:
		return L1 == BROADCAST && L2 == 0; 
	case 2:
		return L2 == BROADCAST;
	}

	K_ASSERT( !_T("Invalid level") );

	return false;
}

const tstring 
CellId::ToString() const
{
    TCHAR w[128];

	::memset( w, 0, sizeof(TCHAR)*128 );

    _sntprintf_s( w, 128, _T("%d-%d-%d-%d"), N, L0, L1, L2 );

    return w;
}

bool 
CellId::Pack( BitStream& bs )
{
	bs.WriteInt( N, 12 );	
	bs.WriteInt( L0, 12 );	
	bs.WriteInt( L1, 12 );	
	bs.WriteInt( L2, 12 );	

	return bs.IsValid();
}

bool 
CellId::Unpack( BitStream& bs )
{
	uint s = 0;
	bs.ReadInt( s, 12 );	
	N = (ushort)s;

	s = 0;
	bs.ReadInt( s, 12 );	
	L0 = (ushort)s;

	s = 0;
	bs.ReadInt( s, 12 );	
	L1 = (ushort)s;

	s = 0;
	bs.ReadInt( s, 12 );	
	L2 = (ushort)s;

	return bs.IsValid();
}

} // gk
