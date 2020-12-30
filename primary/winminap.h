#pragma once

/*
	The primary benefit of the apphost.h/appmain.cpp combination is to turn
	windows specifics into simpler concepts.

	Typical Windows messages, such as mouse and keyboard, and turned into
	topics, that other applications can subscribe to.

	A 'main()' is provided, so the application can implement a 'setup()'
	instead, and not have to worry about whether it is a console or Window
	target.

	All other aspects of the application are up to the application environment,
	but at least all the Win32 specific stuff is wrapped up.
*/

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#include <windows.h>

#include <windowsx.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>

#if defined(_WIN32) || defined(_WIN64)
#define NDT_IS_WINDOWS
#endif

#if defined(_MSC_VER)
#define NDT_IS_MSVC
#endif

#if BUILD_AS_DLL
#define APP_API		__declspec(dllexport)
#else
//  #define APP_API		__declspec(dllimport)
#define APP_API
#endif

#define APP_EXPORT		__declspec(dllexport)
//#define APP_EXPORT

#define APP_INLINE  static inline
#define APP_EXTERN  extern


#include "pubsub.h"





#include <stdio.h>
#include <string>
#include <memory>


struct WinMessage 
{
	HWND hWnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
};


#ifdef __cplusplus
extern "C" {
#endif

	typedef void (*VOIDROUTINE)();
	typedef void (*PFNDOUBLE1)(const double param);

	// Miscellaneous globals
	APP_EXPORT extern int gargc;
	APP_EXPORT extern char** gargv;

	// The various 'onxxx' routines are meant to be implemented by
	// application environment code.  If they are implemented
	// the ndt runtime will load them in and call them at appropriate times
	// if they are not implemented, they simply won't be called.
	APP_EXPORT void onLoad();	// upon loading application
	APP_EXPORT void onUnload();	// last thing before application is really stopped

	APP_EXPORT void onLoop();	// called each time through application main loop


	// The following routines are part of the runtime, and are called
	// by the implementing application.
	// The control the lifetime of the environment, creation of primary window
	// and whether various parts of the IO system are present
	APP_EXPORT void halt();

#ifdef __cplusplus
}
#endif

using WinMsgTopic = Topic<WinMessage>;

APP_EXPORT void subscribe(WinMsgTopic::Subscriber s);





