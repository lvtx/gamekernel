#pragma once 

#include <kserver/test/TestClient.h>

class TiXmlElement;

namespace gk {

/** 
 * @class TestDriver 
 */
class TestDriver 
{
public:
	TestDriver();
	~TestDriver();

	bool Init( TestCreator* creator, uint testCount, TiXmlElement* xe );

	bool Run();

	void Fini();

private:
	typedef std::vector<TestClient*> TestList;

	TestList 	 	m_tests;
	TestCreator* 	m_creator;
	TiXmlElement*   m_xe;
};

} // gk
