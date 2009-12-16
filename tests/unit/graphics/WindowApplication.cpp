#include "stdafx.h"

#include "WindowApplication.h"

WindowApplication* WindowApplication::Instance = 0;


void WindowApplication::Setup( TCHAR* windowTitle, 
									  int positionX, 
									  int positionY, 
									  int width, 
									  int height )
{
	handle_			= 0;
	width_			= width;
	height_			= height;
	windowTitle_	= windowTitle;
	positionX_		= positionX;
	positionY_		= positionY;
	int centerX		= positionX_ + ( width / 2 );
    int centerY		= positionY_ + ( height / 2 );

    SetCursor( 0 );
    SetCursorPos( centerX, centerY );

	WindowApplication::Instance = this;
}

HWND WindowApplication::GetWindowHandle()
{
    return handle_;
}

#include <iostream>

LRESULT WINAPI ApplicationMsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    WindowApplication* app = WindowApplication::Instance;

    K_ASSERT( app != 0 );

	app->OnMessage( hWnd, msg, wParam, lParam );

	switch( msg )
    {
    case WM_DESTROY:
        {
            PostQuitMessage( 0 );
            return 0;
        }
	}

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

void 
WindowApplication::Run( int duration )
{
    TCHAR* windowClassName =  _T("Window Application");

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), 
					  CS_CLASSDC, 
					  ApplicationMsgProc, 
					  0L, 
					  0L,
        			  GetModuleHandle(NULL), 
					  NULL, 
					  NULL, 
					  NULL, 
					  NULL, 
					  windowClassName, 
					  NULL };

    RegisterClassEx( &wc );

    // Create the application's Window
    handle_ = CreateWindow( windowClassName, 
							windowTitle_, 
							WS_OVERLAPPEDWINDOW, 
							positionX_, 
							positionY_, 
							width_, 
							height_, 
							NULL, 
							NULL, 
							wc.hInstance, 
							NULL );

	if ( !init() ) goto END;

	ShowWindow( handle_, SW_SHOWDEFAULT );
	UpdateWindow( handle_ );

	int started = GetTickCount();

	{
		MSG msg;

		ZeroMemory( &msg, sizeof(msg) );

		while( msg.message!=WM_QUIT )
		{
			int elapsed = 0;

			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else
			{
				work();
			}

			elapsed = GetTickCount() - started;

			if ( duration > 0 && elapsed > duration )
			{
				break;
			}
		}		
	}

END:
	fini();
}

void WindowApplication::OnMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	onMessage( hWnd, msg, wParam, lParam );
}

bool WindowApplication::init()
{
	return true;
}

void WindowApplication::work()
{
}

void WindowApplication::onMessage( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	hWnd;
	msg;
	wParam;
	lParam;
}

void WindowApplication::fini()
{
}
