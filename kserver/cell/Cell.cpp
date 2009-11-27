#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/cell/Cell.h>

#include <kserver/cell/CellFactory.h>

#include <kserver/message/CellMessageTypes.h>
#include <kserver/message/CellMessage.h>
#include <kserver/message/CmCellState.h>

#include <kserver/Node.h>

#include <kcore/util/StringUtil.h>
#include <kcore/xml/tinyxml.h>

namespace gk {

Cell::Cell()
: m_runner()
, m_mailbox()
, m_node( 0 )
, m_parent( 0 )
, m_id()
, m_celltree()
, m_cells()
, m_name()
, m_cls( _T("base") )
, m_processCount( 0 )
, m_updateId()
, m_tickUpdate()
, m_updateInterval( 2000 )
{
}

Cell::~Cell()
{
}

bool Cell::Init( Node* node, 
				 Cell* parent, 
				 const CellId& id, 
				 TiXmlElement* xcfg, 
				 bool active )	
{
	K_ASSERT( node != 0 );
	K_ASSERT( id.IsValid() );

	m_node 	 		= node;
	m_activeFlag  	= 0;
	m_id			= id;
	m_parent 		= parent;

	// default behavior is broadcasting cell state
	m_updateId 		= CellId( CellId::BROADCAST, 
			 				  CellId::BROADCAST, 
			 				  CellId::BROADCAST, 
			 				  CellId::BROADCAST );

	if ( m_parent == 0 )
	{
		if ( m_id.GetLevel() != 0 )
		{
			LOG( FT_ERROR, _T("Cell::Init> parent is null") );

			return false;
		}
	}

	if ( active )
	{
		MakeActive();
	}

	m_typeDispatcher.Init();
	m_ctxDispatcher.Init();
	m_transDispatcher.Init();

	if ( xcfg != 0 )
	{
		bool rc = loadConfig( xcfg );

		if ( !rc )
		{
			LOG( FT_ERROR, _T("Cell::Init> Failed to load config") );

			return false;
		}
	}

	m_runner.Init( this );

	m_processCount = 0;

	return true;
}

void 
Cell::Send( MessagePtr m )
{
	K_ASSERT( m->type > CELL_MESSAGE_BEGIN );

	CellMessage* cm = static_cast<CellMessage*>( m.Get() );
	
	K_ASSERT( cm->dst.IsValid() || cm->remote > 0 || !cm->remotes.empty() );

	cm->src = m_id;

	m_node->Send( m );

	++m_processCount;
}

void 
Cell::Notify( MessagePtr m )
{
	LOG( FT_DEBUG, _T("Cell::Notify> type %d rcvd"), m->type );

	// since a cell can be activated, we need to use a queue 
	m_mailbox.Put( m );
}

void 
Cell::Run()
{
	processMailbox();
	processCells();
	processCluster();
	processTrans();

	run();

	m_typeDispatcher.Run();
	m_ctxDispatcher.Run();
	m_transDispatcher.Run();

	processActivation(); // this must come last
}

void 
Cell::MakeActive()
{
	m_activeFlag = 1;
}

void 
Cell::MakePassive()
{
	m_activeFlag = 2;
}

void 
Cell::Completed( Transaction* trans )
{
	K_ASSERT( trans != 0 );

	m_transQ.Put( trans );
}

uint 
Cell::CreateUdpGroup( SecurityLevel sl )
{
	return m_node->CreateUdpGroup( sl );
}

void 
Cell::JoinUdpGroup( uint groupId, uint connectionId, const tstring& extra )
{
	m_node->JoinUdpGroup( groupId, connectionId, extra );
}

void 
Cell::LeaveUdpGroup( uint groupId, uint connectionId )
{
	m_node->LeaveUdpGroup( groupId, connectionId );
}

void 
Cell::DestroyUdpGroup( uint groupId )
{
	m_node->DestroyUdpGroup( groupId );
}

void 
Cell::Fini()
{
	// TODO: report parent with message

	fini();

	m_runner.Fini();
	m_typeDispatcher.Fini();
	m_ctxDispatcher.Fini();
	m_transDispatcher.Fini();
}

bool 
Cell::IsUp( const CellId& id )
{
	CellTree::CellNode n;

	if ( m_celltree.Find( n, id ) )
	{
		return n.up;
	}

	return false; // not found
}

void 
Cell::processActivation() 
{
	if ( m_activeFlag == (uint)1 )
	{
		m_activeFlag = 0;

		m_runner.MakeActive();

		return;
	}

	if ( m_activeFlag == (uint)2 )
	{
		m_activeFlag = 0;

		m_runner.MakePassive();
	}
}

void 
Cell::processMailbox()
{
	MessagePtr m;

	while ( m_mailbox.Get( m ) )
	{
		if ( m->type == NET_STATE_MESSAGE )
		{
			onNetState( m );

			continue;
		}
		// else

		K_ASSERT( m->type > CELL_MESSAGE_BEGIN );

		++m_processCount;

		CellMessage* cm = static_cast<CellMessage*>( m.Get() );

		K_ASSERT( cm->dst.GetLevel( m_id.GetLevel() ) == m_id || 
				cm->dst.GetOneLevel( m_id.GetLevel() ) == CellId::BROADCAST );

		switch ( cm->type )
		{
		case CM_CELL_STATE:
			onCellState( m );
			break;
		}

		if ( cm->dst == m_id || cm->dst.IsFinalBroadcast( m_id.GetLevel() ) )
		{
			m_typeDispatcher.Dispatch( m );
			m_ctxDispatcher.Dispatch( m );

			onMessage( m );
		}
		else
		{
			K_ASSERT( m_id.GetLevel() < 3 ); // since we have childs

			ushort targetLevel = m_id.GetLevel() + 1;
			ushort targetAddr  = cm->dst.GetOneLevel( targetLevel );

			bool handled = false;

			// search for exact match first
			CellList::iterator ei( m_cells.find( cm->dst ) );

			if ( ei != m_cells.end() )
			{
				Cell* cell = ei->second;

				cell->Notify( m );

				return;
			}

			// search for broadcast or intermediate cell
			// NOTE: for performance, intermediate cells must be small
			// For example, MMORPG can have 20 large cells, divided into 20 cells again.
			// This is 400 cells, so enough for most of RPG world.
			//
			// In casual game, A channel can be divided into 3 room runner cells 
			// and each room runner can have 1000 room cells.
			CellList::iterator i( m_cells.begin() );
			CellList::iterator iEnd( m_cells.end() );

			for ( ; i != iEnd; ++i )
			{
				Cell* cell = i->second;

				bool match = false;
				
				match = cell->GetId().GetOneLevel( targetLevel ) == targetAddr;
				match = match || ( targetAddr == CellId::BROADCAST );

				if ( match )
				{
					cell->Notify( m ); // slow propagation. fairness with active logic.

					handled = true;
				}
			}

			if ( !handled && m->type != CM_CELL_STATE )
			{
				LOG( FT_WARN, 
				 	 _T("Cell::onCelleMessage> Unhandled type %d dst Addr %d"), 
			     	 m->type, targetAddr );
			}
		}
	}
}

void 
Cell::processCells()
{
	CellList::iterator i( m_cells.begin() );	
	CellList::iterator iEnd( m_cells.end() );	

	for ( ; i != iEnd; ++i )
	{
		Cell* cell = i->second;
		K_ASSERT( cell != 0 );

		if ( !cell->IsActive() )
		{
			cell->Run(); // tick passive cells	
		}
	}
}

void 
Cell::processCluster() 
{
	K_ASSERT( m_updateInterval > 0 );

	if ( m_tickUpdate.Elapsed() < m_updateInterval )
	{
		return;
	}

	m_tickUpdate.Reset();

	CmCellState* cs = new CmCellState;

	cs->src = m_id;
	cs->dst = m_updateId;
	cs->up  = true;

	m_node->Send( MessagePtr( cs ) );

	LOG( FT_DEBUG, 
		 _T("Cell::processCluster> Cell %s state sent"), 
		 m_id.ToString().c_str() );
}

void 
Cell::processTrans()
{
	Transaction* trans = 0;

	while ( trans != 0 )
	{
		const CellId& id = trans->GetCellId();

		if ( id == m_id )
		{
			m_transDispatcher.Dispatch( trans );
		}
		else
		{
			K_ASSERT( m_id.GetLevel() < 3 ); // since we have childs

			ushort targetLevel = m_id.GetLevel() + 1;
			ushort targetAddr  = id.GetOneLevel( targetLevel );

			bool handled = false;

			// search for exact match first
			CellList::iterator ei( m_cells.find( id ) );

			if ( ei != m_cells.end() )
			{
				Cell* cell = ei->second;

				cell->Completed( trans );

				continue;
			}

			CellList::iterator i( m_cells.begin() );
			CellList::iterator iEnd( m_cells.end() );

			for ( ; i != iEnd; ++i )
			{
				Cell* cell = i->second;

				bool match = false;
				
				match = cell->GetId().GetOneLevel( targetLevel ) == targetAddr;

				if ( match )
				{
					cell->Completed( trans ); 
					handled = true;
				}
			}

			if ( !handled )
			{
				LOG( FT_WARN, 
				 	 _T("Cell::processTrans> Unhandled type %d cell Addr %d"), 
			     	 trans->GetType(), targetAddr );
			}
		}
	}
}

void 
Cell::onCellState( MessagePtr m )
{
	K_ASSERT( m.Get() != 0 );

	CmCellState* cs = static_cast<CmCellState*>( m.Get() );

	CellTree::CellNode n;

	n.id = cs->src;
	n.up = cs->up;

	m_celltree.Update( n );

	LOG( FT_DEBUG, 
		 _T("Cell::onCellState> %s updated"), 
		 n.id.ToString().c_str() );
}

void 
Cell::onNetState( MessagePtr m )
{
	m_typeDispatcher.Dispatch( m );	

	// notify to child cells
	CellList::iterator i( m_cells.begin() );
	CellList::iterator iEnd( m_cells.end() );

	for ( ; i != iEnd; ++i )
	{
		Cell* cell = i->second;

		cell->Notify( m );
	}
}

bool 
Cell::loadConfig( TiXmlElement* xcfg )
{
	K_ASSERT( xcfg != 0 );

	// xcfg points to cell

	TiXmlElement* xchilds = xcfg->FirstChildElement( "childs" );

	if ( xchilds != 0 )
	{
		loadCells( xchilds );
	}
	
	return init( xcfg ); // reading further options and initialize
}

bool 
Cell::loadCells( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	if ( m_id.GetLevel() >= 3 )
	{
		LOG( FT_ERROR, _T("Cell::loadCells> Level 3 cannot have childs") );

		return false;
	}

	TiXmlElement* xchild = xe->FirstChildElement( "cell" );

	while ( xchild != 0 )
	{
		loadCell( xchild );

		xchild = xe->NextSiblingElement( "cell" );
	}

	return true;
}

bool 
Cell::loadCell( TiXmlElement* xe )
{
	K_ASSERT( xe != 0 );

	const char* cls = xe->Attribute( "class" );

	if ( cls == 0 )
	{
		LOG( FT_ERROR, _T("Cell::loadCell> class not found") );

		return false;
	}

	Cell* cell = CellFactory::Instance()->Create( cls );

	if ( cell == 0 )
	{
		tstring wcls;

		StringUtil::ConvertToSystemCode( cls, wcls );

		LOG( FT_ERROR, _T("Cell::loadCell> class %s is invalid"), wcls.c_str() );

		return false;
	}

	int id = 0;

	xe->Attribute( "id", &id );

	if ( id <= 0 || id > 4096 )
	{
		LOG( FT_ERROR, _T("Cell::loadCell> id %d is invalid"), id );

		return false;
	}

	const char* active = xe->Attribute( "active" );

	bool bactive = false;

	if ( active != 0 )
	{
		if ( _stricmp( active, "yes" ) == 0 )
		{
			bactive = true;
		}
	}

	CellId cellId;

	cellId = m_id;
	cellId.SetOneLevel( m_id.GetLevel() + 1, (ushort)id );

	bool rc = cell->Init( m_node, this, cellId, xe, bactive );

	if ( !rc )
	{
		cell->Fini();

		LOG( FT_ERROR, _T("Cell::loadCell> Cell %d init failed"), id );

		delete cell;

		return false;
	}

	m_cells.insert( CellList::value_type( cell->GetId(), cell ) );

	LOG( FT_DEBUG, 
		 _T("Cell::loadCell> %d loaded"), 
		 id );

	return true;
}

bool 
Cell::init( TiXmlElement* /* root */  )
{
	return true;
}

void 
Cell::run()
{
}

void 
Cell::onMessage( MessagePtr /* m */ )
{
}

void 
Cell::fini()
{
}

} // gk
