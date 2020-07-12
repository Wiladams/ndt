#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <windowsx.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>

#include "bitbang.h"
#include "NativeWindow.hpp"
#include "Surface.h"
#include "joystick.h"
#include "uievent.h"

#include <stdio.h>
#include <string>

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


EXPORT extern int gFPS;   // Frames per second
EXPORT extern User32Window * gAppWindow;
EXPORT extern Surface * gAppSurface;


//EXPORT extern UINT_PTR gAppTimerID;
EXPORT extern bool gLooping;
EXPORT extern bool gRunning;


// Globals we expect the user to consume
EXPORT extern int displayWidth;
EXPORT extern int displayHeight;
EXPORT extern unsigned int displayDpi;
EXPORT extern int canvasWidth;
EXPORT extern int canvasHeight;

EXPORT extern int clientLeft;
EXPORT extern int clientTop;

// Location of mouse in screen coordinates
EXPORT extern int rawMouseX;
EXPORT extern int rawMouseY;


// These are typically implemented by
// an app framework
EXPORT void onFrame();  // Per frame
EXPORT void onLoop();   // each time through event loop
EXPORT void onLoad();	// upon loading application

// These should be implemented by a module to be loaded
EXPORT void draw();
EXPORT void preload();
EXPORT void setup();
EXPORT void update(const double dt);

EXPORT void joyPressed(const JoystickEvent& e);
EXPORT void joyReleased(const JoystickEvent& e);
EXPORT void joyMoved(const JoystickEvent& e);
EXPORT void joyMovedZ(const JoystickEvent& e);

// Touch Events
EXPORT void touchStarted(const TouchEvent& e);
EXPORT void touchEnded(const TouchEvent& e);
EXPORT void touchMoved(const TouchEvent& e);
EXPORT void touchHover(const TouchEvent& e);

// Pointer events
EXPORT LRESULT handlePointer(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

EXPORT void fileDrop(const FileDropEvent& e);

// Controlling the runtime
EXPORT void halt();
EXPORT void loop();
EXPORT void noLoop();
EXPORT void forceRedraw(void* param, int64_t tickCount);

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


EXPORT void setFrameRate(int newRate);
EXPORT void setWindowPosition(int x, int y);
EXPORT bool setCanvasSize(long aWidth, long aHeight);


#ifdef __cplusplus
}
#endif
