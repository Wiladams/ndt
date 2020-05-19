#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
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




#ifdef __cplusplus
extern "C" {
#endif

// Some generic function signatures
typedef void (* KeyEventHandler)(const KeyEvent &e);
typedef void (* MouseEventHandler)(const MouseEvent &e);
typedef void (*JoystickEventHandler)(const JoystickEvent& e);

typedef void (* VOIDROUTINE)();


// Miscellaneous globals
EXPORT extern int gargc;
EXPORT extern char **gargv;


EXPORT extern int gFPS;   // Frames per second
EXPORT extern User32Window * gAppWindow;
EXPORT extern Surface * gAppSurface;


EXPORT extern UINT_PTR gAppTimerID;
EXPORT extern bool gLooping;
EXPORT extern bool gRunning;
EXPORT extern bool gIsLayered;

// Globals we expect the user to consume
EXPORT extern int displayWidth;
EXPORT extern int displayHeight;
EXPORT extern unsigned int displayDpi;

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
EXPORT void onFrame();
EXPORT void onLoop();
//EXPORT LRESULT onPaintHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//EXPORT LRESULT onKeyboardHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//EXPORT LRESULT onMouseHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// These should be implemented by a module to be loaded
EXPORT void draw();
EXPORT void preload();
EXPORT void setup();
EXPORT void update(double dt);

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

// Controlling the runtime
EXPORT void halt();
EXPORT void loop();
EXPORT void noLoop();

EXPORT void layered();
EXPORT void noLayered();
EXPORT bool isLayered();

EXPORT void rawInput();
EXPORT void noRawInput();

EXPORT void cursor();
EXPORT void noCursor();




EXPORT void setFrameRate(int newRate);
EXPORT void setWindowPosition(int x, int y);
EXPORT bool setCanvasSize(long aWidth, long aHeight);


#ifdef __cplusplus
}
#endif
