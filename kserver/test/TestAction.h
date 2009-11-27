#pragma once 

#include <kserver/cell/Action.h>

class TiXmlElement;

namespace gk {

class NetClient;
class DummyCell;

/** 
 * @class TestAction 
 *
 * To make a test, override load of this class to load configuration. 
 * Then override TestCreator to create this action.
 */
class TestAction : public Action
{
public:
	TestAction();
	virtual ~TestAction();

	bool Init( NetClient* client, uint idx, TiXmlElement* xe );

protected:
	virtual bool load( TiXmlElement* xe );

protected:
	uint 		m_idx;
	NetClient* 	m_client;
	DummyCell*  m_cell;
};

} // gk
