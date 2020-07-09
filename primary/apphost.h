#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <windowsx.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>

#include "bitbang.hpp"
#include "NativeWindow.hpp"
#include "Surface.h"
#include "joystick.h"


#include <stdio.h>

// EXPORT allows us to declare a function as exported
// this will make it possible to then lookup that 
// function in the .dll using GetProcAddress
// So, if that's needed, just put EXPORT at the front
// of a declaration.
#define EXPORT __declspec(dllexport)


// Basic type to encapsulate a mouse event
enum {
    MOUSEMOVED,
    MOUSEPRESSED,
    MOUSERELEASED,
    MOUSECLICKED,
    MOUSEDRAGGED,
    MOUSEWHEEL,
    MOUSEENTERED,
    MOUSELEFT
};

struct MouseEvent {
    int activity;
    int x;
    int y;
    int delta;
    bool control;
    bool shift;
    bool lbutton;
    bool rbutton;
    bool mbutton;
    bool xbutton1;
    bool xbutton2;
};

enum {
    KEYPRESSED,
    KEYRELEASED,
    KEYTYPED
};

struct KeyEvent {
    int activity;
    int keyCode;        // wparam
    int repeatCount;    // 0 - 15
    int scanCode;       // 16 - 23
    bool isExtended;    // 24
    bool wasDown;       // 30
};


enum {
    TOUCH_DOWN,
    TOUCH_UP,
    TOUCH_MOVE,
    TOUCH_HOVER,
};

struct TouchEvent {
    //HANDLE device;
    int id;
    int activity;
    int rawX;
    int rawY;
    int rawWidth;
    int rawHeight;

    int x;
    int y;
    int w;
    int h;
};

struct PointerEvent {
    int id;
    int x;
    int y;
};

#ifdef __cplusplus
extern "C" {
#endif

// Some generic function signatures
typedef void (* KeyEventHandler)(const KeyEvent &e);
typedef void (* MouseEventHandler)(const MouseEvent &e);
typedef void (*JoystickEventHandler)(const JoystickEvent& e);
typedef void (*TouchEventHandler)(const TouchEvent& e);
typedef void (*PointerEventHandler)(const PointerEvent& e);

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
EXPORT extern bool gIsLayered;

// Globals we expect the user to consume
EXPORT extern int displayWidth;
EXPORT extern int displayHeight;
EXPORT extern unsigned int displayDpi;
EXPORT extern int canvasWidth;
EXPORT extern int canvasHeight;

EXPORT extern int clientLeft;
EXPORT extern int clientTop;

// Keyboard Globals
EXPORT extern int keyCode;
EXPORT extern int keyChar;

// Mouse Globals
EXPORT extern bool mouseIsPressed;
EXPORT extern int mouseX;
EXPORT extern int mouseY;
EXPORT extern int mouseDelta;
EXPORT extern int pmouseX;
EXPORT extern int pmouseY;

// Location of mouse in screen coordinates
EXPORT extern int rawMouseX;
EXPORT extern int rawMouseY;


// These are typically implemented by
// an app framework
EXPORT void onFrame();  // Per frame
EXPORT void onLoop();   // each time through event loop

// These should be implemented by a module to be loaded
EXPORT void draw();
EXPORT void preload();
EXPORT void setup();
EXPORT void update(const double dt);

// IO Event Handlers
EXPORT void keyPressed(const KeyEvent &e);
EXPORT void keyReleased(const KeyEvent &e);
EXPORT void keyTyped(const KeyEvent &e);


EXPORT void mouseClicked(const MouseEvent &e);
EXPORT void mouseDragged(const MouseEvent &e);
EXPORT void mouseMoved(const MouseEvent &e);
EXPORT void mousePressed(const MouseEvent &e);
EXPORT void mouseReleased(const MouseEvent &e);
EXPORT void mouseWheel(const MouseEvent &e);

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
