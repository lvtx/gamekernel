#include "stdafx.h"

#include <kserver/serverbase.h>
#include <kserver/test/TestDriver.h>

#include <kcore/xml/tinyxml.h>

namespace gk {

TestDriver::TestDriver()
: m_creator( 0 )
, m_xe( 0 )
{
}

TestDriver::~TestDriver()
{
	Fini();
}

bool 
TestDriver::Init( TestCreator* creator, uint testCount, TiXmlElement* xe )
{
	K_ASSERT( creator != 0 );
	K_ASSERT( testCount > 0 );
	K_ASSERT( xe != 0 );

	m_creator = creator;
	m_xe 	  = xe;

	for ( uint i=0; i<testCount; ++i )
	{
		TestClient* tc = new TestClient;

		tc->Init( creator, i, xe );

		m_tests.push_back( tc );
	}

	return true;
}

bool 
TestDriver::Run()
{
	// check for tests and restart if finished

	uint idx = 0;
	
	TestList::iterator i( m_tests.begin() );
	TestList::iterator iEnd( m_tests.end() );

	for ( ; i != iEnd; ++i )
	{
		TestClient* tc = *i;

		if ( tc->IsFinished() )
		{
			tc->Fini();
			tc->Init( m_creator, idx, m_xe);
		}

		++idx;
	}

	m_tests.clear();

	return true;
}

void 
TestDriver::Fini()
{
	// clear all tests
	TestList::iterator i( m_tests.begin() );
	TestList::iterator iEnd( m_tests.end() );

	for ( ; i != iEnd; ++i )
	{
		TestClient* tc = *i;

		tc->Fini();

		delete tc;
	}

	m_tests.clear();

	m_creator = 0;
}

} // gk
