#pragma once 

#include "WindowApplication.h"
#include <gtest/gtest.h>

TEST( TestWinApp, testOne ) {
	WindowApplication app;

	int started = GetTickCount();

	app.Setup( _T("Test App"), 0, 0, 1024, 768 );
	app.Run( 500 );

	ASSERT_TRUE( GetTickCount() - started >= 500 );
}