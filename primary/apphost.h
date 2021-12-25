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
#include "NativeWindow.hpp"
#include "Surface.h"
#include "joystick.h"
#include "uievent.h"
#include "Network.hpp"

#include <stdio.h>
#include <string>
#include <memory>

// EXPORT allows us to declare a function as exported
// this will make it possible to then lookup that 
// function in the .dll using GetProcAddress
// So, if that's needed, just put EXPORT at the front
// of a declaration.
//#define EXPORT __declspec(dllexport)


#ifdef __cplusplus
extern "C" {
#endif


typedef void (* VOIDROUTINE)();
typedef void (*PFNDOUBLE1)(const double param);

// Miscellaneous globals
APP_EXPORT extern int gargc;
APP_EXPORT extern char **gargv;


APP_EXPORT extern User32Window * gAppWindow;
APP_EXPORT extern std::shared_ptr<Surface> gAppSurface;


//EXPORT extern bool gRunning;


// Globals we expect the user to consume
APP_EXPORT extern int displayWidth;
APP_EXPORT extern int displayHeight;
APP_EXPORT extern unsigned int systemDpi;
APP_EXPORT extern unsigned int systemPpi;

APP_EXPORT extern int canvasWidth;
APP_EXPORT extern int canvasHeight;



// The various 'onxxx' routines are meant to be implemented by
// application environment code.  If they are implemented
// the ndt runtime will load them in and call them at appropriate times
// if they are not implemented, they simply won't be called.
APP_EXPORT void onLoad();	// upon loading application
APP_EXPORT void onUnload();

APP_EXPORT void onLoop();	// called each time through application main loop


// The following routines are part of the ndt runtime, and are called
// by the implementing application.
// The control the lifetime of the environment, creation of primary window
// and whether various parts of the IO system are present

APP_EXPORT void showAppWindow();
APP_EXPORT void halt();

//EXPORT void forceRedraw(void* param, int64_t tickCount);
APP_EXPORT void screenRefresh();

APP_EXPORT void layered();
APP_EXPORT void noLayered();
APP_EXPORT bool isLayered();

APP_EXPORT void rawInput();
APP_EXPORT void noRawInput();

APP_EXPORT void joystick();
APP_EXPORT void noJoystick();

// Touch routines apps can implement
APP_EXPORT bool touch();
APP_EXPORT bool noTouch();
APP_EXPORT bool isTouch();

// Turn on/off file drop handling
APP_EXPORT bool dropFiles();
APP_EXPORT bool noDropFiles();

APP_EXPORT void cursor();
APP_EXPORT void noCursor();

APP_EXPORT void show();
APP_EXPORT void hide();



APP_EXPORT void setWindowPosition(int x, int y);
APP_EXPORT bool setCanvasSize(long aWidth, long aHeight);


#ifdef __cplusplus
}
#endif

// Make Topic publishers available
using SignalEventTopic = Topic<intptr_t>;


// Doing C++ pub/sub
using MouseEventTopic = Topic<MouseEvent&>;
using KeyboardEventTopic = Topic<KeyboardEvent&>;
using JoystickEventTopic = Topic<JoystickEvent&>;
using FileDropEventTopic = Topic<FileDropEvent&>;
using TouchEventTopic = Topic<TouchEvent&>;
using PointerEventTopic = Topic<PointerEvent&>;


APP_EXPORT void subscribe(SignalEventTopic::Subscriber s);
APP_EXPORT void subscribe(MouseEventTopic::Subscriber s);
APP_EXPORT void subscribe(KeyboardEventTopic::Subscriber s);
APP_EXPORT void subscribe(JoystickEventTopic::Subscriber s);
APP_EXPORT void subscribe(FileDropEventTopic::Subscriber s);
APP_EXPORT void subscribe(TouchEventTopic::Subscriber s);
APP_EXPORT void subscribe(PointerEventTopic::Subscriber s);
