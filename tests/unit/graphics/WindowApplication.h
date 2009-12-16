#pragma once

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>


/**
 * @class WindowApplication
 *
 * A skeleton class to test graphics features. 
 *
 */
class WindowApplication
{
public:
	static WindowApplication* Instance;

    void Setup( TCHAR* windowTitle, int positionX, int positionY, int width, int height );

    void Run( int duration = 0 );

	void OnMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

    HWND GetWindowHandle();

protected:
	virtual bool init();
	virtual void work();
	virtual void onMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	virtual void fini();

protected:
    HWND 	handle_;
    TCHAR* 	windowTitle_;
    int 	positionX_;
    int 	positionY_;
    int 	width_;
    int 	height_;
};




