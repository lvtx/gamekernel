#pragma once

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#define WIN32_LEAN_AND_MEAN	

#include <winsock2.h>
#include <windows.h>

#include <mswsock.h> // SIO_CONNECTION_RESET
#include <tchar.h>

