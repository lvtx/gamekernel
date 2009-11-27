#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/CellTree.h>

namespace gk {

CellTree::CellTree()
: m_root()
{
}

CellTree::~CellTree()
{
}

bool 
CellTree::Init()
{
	return true;
}

void 
CellTree::Update( const CellNode& cn )
{
	K_ASSERT( cn.id.IsValid() );

	update( 0, m_root, cn );
}

void 
CellTree::UpdateState( const CellId& id, bool up )
{
	CellNode cn;

	if ( find( cn, 0, m_root, id ) )
	{
		cn.up = up;

		Update( cn );
	}
}

void 
CellTree::Remove( const CellNode& cn )
{
	Remove( cn.id );
}

void 
CellTree::Remove( const CellId& id )
{
	remove( 0, m_root, id );
}

bool
CellTree::Find( CellNode& r, const CellId& id )
{
	return find( r, 0, m_root, id );
}

void 
CellTree::Fini()
{
	m_root.cells.clear();
}

void 
CellTree::update( ushort level, CellNode& parent, const CellNode& cn )
{
	if ( level >= CellId::LEVELS )
	{
		return;
	}

	if ( cn.id.GetLevel() < level )
	{
		return;
	}

	CellNode::CellNodeList::iterator i( parent.cells.find( cn.id.GetLevel( level ) ) );

	if ( i == parent.cells.end() )
	{
		CellNode newNode;

		newNode = cn;
		newNode.id = cn.id.GetLevel( level );

		parent.cells.insert( CellNode::CellNodeList::value_type( newNode.id, newNode ) );

		update( ++level, newNode, cn );
	}
	else
	{
		CellNode& existNode = i->second;

		if ( existNode.id == cn.id )
		{
			existNode.up = cn.up; 	// replace information only

			return; 				// we found it
		}

		update( ++level, existNode, cn ); 
	}
}

void 
CellTree::remove( ushort level, CellNode& parent, const CellId& id )
{
	if ( level >= CellId::LEVELS )
	{
		return;
	}

	if ( id.GetLevel() < level )
	{
		return;
	}
	
	CellNode::CellNodeList::iterator i( parent.cells.find( id.GetLevel( level ) ) );

	if ( i == parent.cells.end() )
	{
		return; // not found
	}

	CellNode& existNode = i->second;

	if ( existNode.id == id )
	{
		parent.cells.erase( i );

		return; // we found it
	}

	remove( ++level, existNode, id ); // recursion
}

bool
CellTree::find( CellNode& r, ushort level, CellNode& parent, const CellId& id )
{
	if ( level >= CellId::LEVELS )
	{
		return false;
	}

	if ( id.GetLevel() < level )
	{
		return false;
	}
	
	CellNode::CellNodeList::iterator i( parent.cells.find( id.GetLevel( level ) ) );

	if ( i == parent.cells.end() )
	{
		return false;
	}

	CellNode& existNode = i->second;

	if ( existNode.id == id )
	{
		r = existNode;
		
		return true;
	}

	return find( r, ++level, existNode, id ); // recursion
}

} // gk
