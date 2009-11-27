#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/action/ActionHsm.h>

namespace gk {

ActionHsm::ActionHsm()
: m_owner( 0 )
, m_current( 0 )
{
}

ActionHsm::~ActionHsm()
{
	Fini();
}


bool 
ActionHsm::Init( Action* owner )
{
	m_owner   = owner;
	m_current = &m_root;
						
	return init();
}

void 
ActionHsm::OnMessage( const MessagePtr m )
{
	ActionState* state = m_current;

	while ( state != 0 ) // keep going to parent
	{
		state = state->OnMessage( m );
	}
}

void 
ActionHsm::OnTick( uint age )
{
	ActionState* state = m_current;

	while ( state != 0 ) // keep going to parent
	{
		state = state->OnTick( age );
	}
}

void 
ActionHsm::Tran( int targetState )
{
	if ( m_current == 0 )
	{
		return;
	}

	ActionState::Path path;

	bool rc = m_current->GetTransitionPathTo( targetState, path );

	if ( !rc )
	{
		return;
	}

	ActionState::Path::iterator i( path.begin() );
	ActionState::Path::iterator iEnd( path.end() );

	for ( ; i != iEnd; ++i )
	{
		ActionState::Node& n = *i;
		K_ASSERT( n.state != 0 );

		bool targetFound = 0;

		switch ( n.code )
		{
		case ActionState::Node::ENTER:
			{
				n.state->OnEnter();
			}
			break;
		case ActionState::Node::EXIT:
			{
				n.state->OnExit();
			}
			break;
		case ActionState::Node::TARGET:
			{
				targetFound = true;

				n.state->OnEnter();
				m_current = n.state;
			}
			break;
		case ActionState::Node::NOOP:		// NOOP
			{
			}
			break;
		default:
			break;
		}
	}
}

void 
ActionHsm::Fini()
{
	m_root.Fini(); // clean up
}
 
} // gk
