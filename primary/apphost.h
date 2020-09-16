#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <windowsx.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>

#include "appconfig.h"
#include "bitbang.h"
#include "NativeWindow.hpp"
#include "Surface.h"
#include "joystick.h"
#include "uievent.h"
#include "pubsub.h"

#include <stdio.h>
#include <string>
#include <memory>

// EXPORT allows us to declare a function as exported
// this will make it possible to then lookup that 
// function in the .dll using GetProcAddress
// So, if that's needed, just put EXPORT at the front
// of a declaration.
#define EXPORT __declspec(dllexport)


#ifdef __cplusplus
extern "C" {
#endif


typedef void (* VOIDROUTINE)();
typedef void (*PFNDOUBLE1)(const double param);

// Miscellaneous globals
EXPORT extern int gargc;
EXPORT extern char **gargv;


EXPORT extern User32Window * gAppWindow;
EXPORT extern std::shared_ptr<Surface> gAppSurface;


EXPORT extern bool gRunning;


// Globals we expect the user to consume
EXPORT extern int displayWidth;
EXPORT extern int displayHeight;
EXPORT extern unsigned int systemDpi;

EXPORT extern int canvasWidth;
EXPORT extern int canvasHeight;



// These are typically implemented by
// an app framework
EXPORT void onLoad();	// upon loading application
EXPORT void onUnload();


// Controlling the runtime
EXPORT void showAppWindow();
EXPORT void halt();

//EXPORT void forceRedraw(void* param, int64_t tickCount);
EXPORT void screenRefresh();

EXPORT void layered();
EXPORT void noLayered();
EXPORT bool isLayered();

EXPORT void rawInput();
EXPORT void noRawInput();

EXPORT void joystick();
EXPORT void noJoystick();

// Touch routines apps can implement
EXPORT bool touch();
EXPORT bool noTouch();
EXPORT bool isTouch();

// Turn on/off file drop handling
EXPORT bool dropFiles();
EXPORT bool noDropFiles();

EXPORT void cursor();
EXPORT void noCursor();

EXPORT void show();
EXPORT void hide();



EXPORT void setWindowPosition(int x, int y);
EXPORT bool setCanvasSize(long aWidth, long aHeight);


#ifdef __cplusplus
}
#endif

// Make Topic publishers available
using SignalEventTopic = Topic<intptr_t>;

EXPORT void subscribe(SignalEventTopic::Subscriber s);
EXPORT void subscribe(MouseEventTopic::Subscriber s);
EXPORT void subscribe(KeyboardEventTopic::Subscriber s);
EXPORT void subscribe(JoystickEventTopic::Subscriber s);
EXPORT void subscribe(FileDropEventTopic::Subscriber s);
EXPORT void subscribe(TouchEventTopic::Subscriber s);
EXPORT void subscribe(PointerEventTopic::Subscriber s);
