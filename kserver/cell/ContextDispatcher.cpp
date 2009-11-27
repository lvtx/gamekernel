#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/ContextDispatcher.h>

namespace gk {

ContextDispatcher::ContextDispatcher()
{
}

ContextDispatcher::~ContextDispatcher()
{
}

bool 
ContextDispatcher::Init()
{
	m_tick.Reset();
	m_tickCleanup.Reset();

	return true;
}

void 
ContextDispatcher::Run()
{
	processTick();
	processCleanup();

	m_tick.Reset();
}

void 
ContextDispatcher::Subscribe( const tstring& ctx, ActionPtr action )
{
	K_ASSERT( !ctx.empty() );
	K_ASSERT( action.Get() != 0 );

	ActionMap::iterator i( m_actions.find( ctx ) );

	if ( i == m_actions.end() )
	{
		ActionList lst;

		lst.push_back( action );

		m_actions.insert( ActionMap::value_type( ctx, lst ) );
	}
	else
	{
		ActionList& lst = i->second;

		lst.push_back( action );
	}
}

void 
ContextDispatcher::Dispatch( MessagePtr m )
{
	ActionMap::iterator i( m_actions.find( m->contextKey ) );

	if ( i != m_actions.end() )
	{
		ActionList& lst = i->second;

		ActionList::iterator ai( lst.begin() );

		for ( ; ai != lst.end(); ++ai )
		{
			ActionPtr action = *ai;

			K_ASSERT( action.Get() != 0 );

			if ( !action->IsFinished() )
			{
				action->Run( m );
			}
		}
	}
}

void 
ContextDispatcher::Fini()
{
	ActionMap::iterator i( m_actions.begin() );

	for ( ; i != m_actions.end(); ++i )
	{
		ActionList& lst = i->second;

		ActionList::iterator ai( lst.begin() );

		for ( ; ai != lst.end(); ++ai )
		{
			ActionPtr action = *ai;

			action->Fini();
		}

		lst.clear();
	}

	m_actions.clear();
}

void 
ContextDispatcher::processTick()
{
	uint ms = (uint)( m_tick.Elapsed() * 1000 );

	ActionMap::iterator i( m_actions.begin() );

	for ( ; i != m_actions.end(); ++i )
	{
		ActionList& lst = i->second;

		ActionList::iterator ai( lst.begin() );

		for ( ; ai != lst.end(); ++ai )
		{
			ActionPtr action = *ai;

			if ( !action->IsFinished() )
			{
				if ( !action->Run( ms ) )
				{
					action->Fini();
				}
			}
		}
	}
}

void 
ContextDispatcher::processCleanup()
{
	if ( m_tickCleanup.Elapsed() < 1000 ) // every second
	{
		return;
	}

	m_tickCleanup.Reset();

	ActionMap::iterator i( m_actions.begin() );

	for ( ; i != m_actions.end(); ++i )
	{
		ActionList& lst = i->second;

		ActionList::iterator ai( lst.begin() );

		for ( ; ai != lst.end();  )
		{
			ActionPtr action = *ai;

			if ( action->IsFinished() )
			{
				ai = lst.erase( ai );
			}
			else
			{
				++ai;
			}
		}

		// keep empty list. not a big deal.
	}
}

} // gk

