#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/action/ActionState.h>

#include <kserver/cell/action/ActionHsm.h>

namespace gk {

ActionState::ActionState()
: m_id( 0 )
, m_parent( 0 )
, m_hsm( 0 )
, m_targetId( 0 )
{

}

ActionState::~ActionState()
{
	Fini();
}

bool 
ActionState::Init( ActionHsm* hsm, int id, ActionState* parent )
{
	K_ASSERT( hsm != 0 );

	m_hsm		= hsm;
	m_id		= id;
	m_parent	= parent;

	if ( parent )
	{
		parent->attach( this );
	}

	m_targetId = 0;

	return true;
}

void 
ActionState::OnEnter()
{
	m_targetId	= 0;
}

ActionState* 
ActionState::OnMessage( MessagePtr m )
{
	return (ActionState*)0;
}

ActionState* 
ActionState::OnTick( uint /* age */ )
{
	return (ActionState*)0;
}

void 
ActionState::OnExit()
{
	// empty
}

void 
ActionState::Fini()
{
	StateList::iterator i( m_childs.begin() );
	StateList::iterator iEnd( m_childs.end() );

	for ( ; i != iEnd; ++i )
	{
		ActionState* state = *i;
		K_ASSERT( state != 0 );

		state->Fini(); 

		delete state;
	}

	m_childs.clear();
}

bool 
ActionState::HasState( int id ) const
{
	if ( id == m_id )
	{
		return true;
	}

	StateList::const_iterator i( m_childs.begin() );
	StateList::const_iterator iEnd( m_childs.end() );

	for ( ; i != iEnd; ++i )
	{
		const ActionState* child = *i;

		if ( child->HasState( id ) )
		{
			return true;
		}
	}

	return false;
}

int 
ActionState::GetId() const
{
	return m_id;
}

ActionState* 
ActionState::GetState( int id )
{
	if ( id == m_id )
	{
		return this;
	}

	if ( !HasState( id ) )
	{
		return (ActionState*)0;
	}

	StateList::iterator i( m_childs.begin() );
	StateList::iterator iEnd( m_childs.end() );

	for ( ; i != iEnd; ++i )
	{
		ActionState* child = *i;
		K_ASSERT( child != 0 );

		if ( child->HasState( id ) )
		{
			return child->GetState( id );
		}
	}

	return (ActionState*)0;
}

bool 
ActionState::GetTransitionPathTo( int target, Path& path ) 
{
	if ( HasState( target ) ) 
	{
		if ( target == m_id )
		{
			ActionState::Node n;

			n.state = this;
			n.code  = ActionState::Node::TARGET;

			path.push_back( n );

			return true;
		}
		else
		{
			ActionState::Node n;

			n.state = this;
			n.code  = ActionState::Node::NOOP; 

			path.push_back( n );

			return getChildPathTo( target, path );
		}
	}
	else
	{
		if ( m_parent != 0 )
		{
			ActionState::Node n;

			n.state = this;
			n.code  = ActionState::Node::EXIT;

			path.push_back( n );

			return m_parent->GetTransitionPathTo( target, path );
		}
	}

	return false; 
}

void 
ActionState::attach( ActionState* state )
{
	K_ASSERT( state != 0 );

	if ( HasState( state->GetId() ) )
	{
		return;
	}

	m_childs.push_back( state );
}

bool 
ActionState::getChildPathTo( int target, Path& path )
{
	StateList::iterator i( m_childs.begin() );
	StateList::iterator iEnd( m_childs.end() );

	for ( ; i != iEnd; ++i )
	{
		ActionState* child = *i;

		K_ASSERT( child != 0 );

		if ( child->HasState( target ) )
		{
			return child->GetTransitionPathTo( target, path );
		}
	}

	K_ASSERT( !_T("Should not reach here") );

	return false; // No child is in the path
}

} // gk
