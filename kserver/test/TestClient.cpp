#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/test/TestClient.h>

#include <kcore/xml/tinyxml.h>

namespace gk {

TestClient::TestClient()
: m_action( 0 )
{
}

TestClient::~TestClient()
{
	Fini();
}

bool 
TestClient::Init( TestCreator* creator, uint idx, TiXmlElement* xe )
{
	m_client.Init( this );
	m_finished = false;

	m_action = creator->Create();

	bool rc = m_action->Init( &m_client, idx, xe );

	if ( !rc )
	{
		return false;
	}

	Start();

	return true;
}

int 
TestClient::Run()
{
	Tick tick;

	while ( IsRunning() )
	{
		m_action->Run( tick.Elapsed() );

		tick.Reset();

		MessagePtr m;

		while ( m_recvQ.Get( m ) )
		{
			m_action->Run( m );
		}

		if ( m_action->IsFinished() )
		{
			m_finished = true;
		}

		::Sleep( 10 );
	}

	return 0;
}

void 
TestClient::Fini()
{
	Stop();

	if ( m_action != 0 )
	{
		m_action->Fini();

		delete m_action;

		m_action = 0;
	}

	m_client.Fini();

	m_finished = true;
}

void 
TestClient::Notify( MessagePtr m )
{
	m_recvQ.Put( m );
}

} // gk
