#pragma once 

#include <kserver/test/TestAction.h>

namespace gk {

/** 
 * @class TestCreator 
 */
class TestCreator 
{
public:
	TestCreator();
	virtual ~TestCreator();

	/**
	 * Override this to create a test action with ActionHsm 
	 */
	virtual TestAction* Create() = 0;
};

} // gk
