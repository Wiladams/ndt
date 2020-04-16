#pragma once

#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <windowsx.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "bitbang.hpp"
#include "NativeWindow.hpp"
#include "PBDIBSection.hpp"



#include <stdio.h>

// EXPORT allows us to declare a function as exported
// this will make it possible to then lookup that 
// function in the .dll using GetProcAddress
// So, if that's needed, just put EXPORT at the front
// of a declaration.
#define EXPORT __declspec(dllexport)


// Basic type to encapsulate a mouse event
enum {
    KEYPRESSED,
    KEYRELEASED,
    KEYTYPED
};


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
typedef void (* VOIDROUTINE)();


// Miscellaneous globals
EXPORT extern int gargc;
EXPORT extern char **gargv;


EXPORT extern int gFPS;   // Frames per second
EXPORT extern User32Window * gAppWindow;
EXPORT extern PBDIBSection * gAppSurface;
EXPORT extern BLContext gAppDC;
EXPORT extern UINT_PTR gAppTimerID;
EXPORT extern bool gLooping;
EXPORT extern bool gRunning;
EXPORT extern bool gIsLayered;


EXPORT extern int keyCode;
EXPORT extern int keyChar;
EXPORT extern bool mouseIsPressed;
EXPORT extern int mouseX;
EXPORT extern int mouseY;
EXPORT extern int pmouseX;
EXPORT extern int pmouseY;




// These should be implemented by a module to be loaded
EXPORT void draw();
EXPORT void onLoop();
EXPORT void setup();

//EXPORT LRESULT onPaintHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//EXPORT LRESULT keyboardHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//EXPORT LRESULT mouseHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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


// Controlling the runtime
EXPORT void halt();
EXPORT void loop();
EXPORT void noLoop();

EXPORT void layered();
EXPORT void noLayered();
EXPORT void cursor();
EXPORT void noCursor();
EXPORT void setFrameRate(int newRate);
EXPORT void setWindowPosition(int x, int y);
EXPORT bool setCanvasSize(GRCOORD aWidth, GRCOORD aHeight);


#ifdef __cplusplus
}
#endif
