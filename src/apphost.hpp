#pragma once

#include <stdio.h>

#include "bitbang.hpp"
#include "w32.hpp"
#include "NativeWindow.hpp"
#include "PixelBufferRGBA32.hpp"
#include "PBDIBSection.hpp"
#include "DrawingContext.hpp"
#include "LayeredWindow.hpp"


// Basic type to encapsulate a mouse event
enum {
    KEYPRESSED,
    KEYRELEASED,
    KEYTYPED
};

#define VK_ESCAPE         0x1B
static const int VK_UP = 38;
static const int VK_DOWN = 40;

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


// Some globals to set event handlers
typedef LRESULT (*WinMSGObserver)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef void (* KeyEventHandler)(const KeyEvent &e);
typedef void (* MouseEventHandler)(const MouseEvent &e);
typedef void (* VOIDROUTINE)();

// Application routines
static VOIDROUTINE gDrawHandler = nullptr;
static VOIDROUTINE gLoopHandler = nullptr;
static VOIDROUTINE gPreloadHandler = nullptr;
static VOIDROUTINE gSetupHandler = nullptr;
static VOIDROUTINE gPreSetupHandler = nullptr;


// Painting
static WinMSGObserver gPaintHandler = nullptr;

// Keyboard
static WinMSGObserver gKeyboardHandler = nullptr;
static KeyEventHandler gKeyPressedHandler = nullptr;
static KeyEventHandler gKeyReleasedHandler = nullptr;
static KeyEventHandler gKeyTypedHandler = nullptr;

// Mouse
static WinMSGObserver gMouseHandler = nullptr;
static MouseEventHandler gMouseMovedHandler = nullptr;
static MouseEventHandler gMouseClickedHandler = nullptr;
static MouseEventHandler gMousePressedHandler = nullptr;
static MouseEventHandler gMouseReleasedHandler = nullptr;
static MouseEventHandler gMouseWheelHandler = nullptr;
static MouseEventHandler gMouseDraggedHandler = nullptr;

// Touch
static WinMSGObserver gTouchHandler = nullptr;

// Miscellaneous globals
static int gargc;
static char **gargv;

static int gFPS = 15;   // Frames per second
static User32Window * gAppWindow = nullptr;
static PBDIBSection * gAppSurface = nullptr;
static DrawingContext * gAppDC = nullptr;
static UINT_PTR gAppTimerID = 0;
static bool gLooping = true;
static bool gRunning = true;
static bool gIsLayered = false;
static LayeredWindowInfo *gLayeredWindow = nullptr;

static int keyCode = 0;
static int keyChar = 0;


LRESULT HandleKeyboardEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    KeyEvent e;
    e.keyCode = (int)wParam;
    e.repeatCount =LOWORD(lParam);  // 0 - 15
    e.scanCode = ((lParam & 0xff0000) >> 16);        // 16 - 23
    e.isExtended = (lParam & 0x1000000) != 0;    // 24
    e.wasDown = (lParam & 0x40000000) != 0;    // 30

//printf("HandleKeyboardEvent: %04x\n", msg);

    switch(msg) {

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            e.activity = KEYPRESSED;
            keyCode = e.keyCode;
            if (gKeyPressedHandler) {
                gKeyPressedHandler(e);
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            e.activity = KEYRELEASED;
            keyCode = e.keyCode;
            if (gKeyReleasedHandler) {
                gKeyReleasedHandler(e);
            }
            break;
        case WM_CHAR:
        case WM_SYSCHAR:
            keyChar = (int)wParam;
            e.activity = KEYTYPED;

            if (gKeyTypedHandler) {
                gKeyTypedHandler(e);
            }
            break;
    }

    return res;
}

static bool mouseIsPressed = false;
static int mouseX = 0;
static int mouseY = 0;
static int pmouseX = 0;
static int pmouseY = 0;

LRESULT HandleMouseEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    MouseEvent e;

    e.x = GET_X_LPARAM(lParam);
    e.y = GET_Y_LPARAM(lParam);

    //printf("mouseHandler: 0x%04x  %d %d\n", msg, mouseX, mouseY);

    e.control = (wParam & MK_CONTROL) != 0;
    e.shift = (wParam& MK_SHIFT) != 0;
    e.lbutton = (wParam & MK_LBUTTON) != 0;
    e.rbutton = (wParam & MK_RBUTTON) != 0;
    e.mbutton = (wParam & MK_MBUTTON) != 0;
    e.xbutton1 = (wParam & MK_XBUTTON1) != 0;
    e.xbutton2 = (wParam & MK_XBUTTON2) != 0;
    

    // assign new mouse position
    // BUGBUG - having these globals here might not be a good idea
    // maybe they should be application specific
    // assign previous mouse position
    pmouseX = mouseX;
    pmouseY = mouseY;
    mouseX = e.x;
    mouseY = e.y;
    mouseIsPressed = e.lbutton || e.rbutton || e.mbutton;

    switch(msg) {
        case WM_LBUTTONDBLCLK:
	    case WM_MBUTTONDBLCLK:
	    case WM_RBUTTONDBLCLK:
            break;

        case WM_MOUSEMOVE:
            e.activity = MOUSEMOVED;

            if (gMouseMovedHandler != nullptr) {
                gMouseMovedHandler(e);
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            e.activity = MOUSEPRESSED;

            if (gMousePressedHandler != nullptr) {
                gMousePressedHandler(e);
            }
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            e.activity = MOUSERELEASED;

            // call mouseReleased()
            if (gMouseReleasedHandler != nullptr) {
                gMouseReleasedHandler(e);
            }
            if (gMouseClickedHandler != nullptr) {
                gMouseClickedHandler(e);
            }
            break;
        case WM_MOUSEWHEEL:
            e.activity = MOUSEWHEEL;
            e.delta = GET_WHEEL_DELTA_WPARAM(wParam);

            if (gMouseWheelHandler != nullptr) {
                gMouseWheelHandler(e);
            }
            break;
        case WM_MOUSELEAVE:
        //print("WM_MOUSELEAVE")
        default:
            
        break;
    }

    return res;
}

LRESULT HandleTouchEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    // cInputs could be set to a maximum value (10) and
    // we could reuse the same allocated array each time
    // rather than allocating a new one each time.
    //print("wm_touch_event 0.0: ", wparam)
    int cInputs = LOWORD(wParam);
        
        //print("wm_touch_event 1.0: ", cInputs)
        
        TOUCHINPUT *pInputs = {new TOUCHINPUT[cInputs]{}};
        int cbSize = sizeof("TOUCHINPUT");

        //print("wm_touch_event 2.0: ", pInputs, cbSize)
        BOOL bResult = GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs,cbSize);
        //print("wm_touch_event 3.0: ", bResult)

        if (bResult == 0) {
            return 0; // C.GetLastError()
        }
        //print("wm_touch_event 4.0: ", bResult)
/*
        -- Construct an event with all the given information
        local events = {}

        POINT PT;
        for (int i=0; i<cInputs; i++) {
            PT.x = pInputs[i].x/100;
            PT.y = pInputs[i].y/100;
            //print("wm_touch_event 4.1: ", PT.x, PT.y)
            local bResult = C.ScreenToClient(hwnd, PT)
            --print("wm_touch_event 4.2: ", bResult, PT.x, PT.y)
            local event = {
                ID = pInputs[i].dwID;
                x = PT.x;
                y = PT.y;
                rawX = pInputs[i].x;
                rawY = pInputs[i].y;
            }

            if band(pInputs[i].dwMask, C.TOUCHINPUTMASKF_CONTACTAREA) ~= 0 then
                event.rawWidth = pInputs[i].cxContact;
                event.rawHeight = pInputs[i].cyContact;
                event.width = event.rawWidth/100;
                event.height = event.rawHeight/100;
            end

            table.insert(events, event)
        }
        //print("wm_touch_event 5.0: ", bResult)

        return events
*/
    return res;
}

LRESULT HandlePaintEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //printf("HandlePaintEvent\n");

    LRESULT res = 0;
    PAINTSTRUCT ps;

	HDC hdc = BeginPaint(hWnd, &ps);
        
    int xDest = 0;
    int yDest = 0;
    int DestWidth = gAppSurface->getWidth();
    int DestHeight = gAppSurface->getHeight();
    int xSrc = 0;
    int ySrc = 0;
    int SrcWidth = gAppSurface->getWidth();
    int SrcHeight = gAppSurface->getHeight();

    BITMAPINFO info = gAppSurface->getBitmapInfo();

    int pResult = StretchDIBits(hdc,
        xDest,yDest,
        DestWidth,DestHeight,
        xSrc,ySrc,
        SrcWidth, SrcHeight,
        gAppSurface->getData(),&info,
        DIB_RGB_COLORS,
        SRCCOPY);
        
	EndPaint(hWnd, &ps);


    return res;
}


#ifdef __cplusplus
extern "C" {
#endif


// These should be implemented by a module to be loaded
EXPORT void draw();
EXPORT void onLoop();
EXPORT void setup();

EXPORT LRESULT onPaintHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// IO Event Handlers
EXPORT LRESULT keyboardHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
EXPORT void keyPressed(const KeyEvent &e);
EXPORT void keyReleased(const KeyEvent &e);
EXPORT void keyTyped(const KeyEvent &e);

EXPORT LRESULT mouseHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
EXPORT void mouseClicked(const MouseEvent &e);
EXPORT void mouseDragged(const MouseEvent &e);
EXPORT void mouseMoved(const MouseEvent &e);
EXPORT void mousePressed(const MouseEvent &e);
EXPORT void mouseReleased(const MouseEvent &e);
EXPORT void mouseWheel(const MouseEvent &e);

#ifdef __cplusplus
}
#endif

void setFrameRate(int newRate)
{
    gFPS = newRate;
    //printf("setFrameRate: %d\n", newRate);
    UINT_PTR nIDEvent = 5;
    BOOL bResult = KillTimer(gAppWindow->getHandle(), gAppTimerID);
    //printf("KillTimer: %d %lld\n", bResult, gAppTimerID);
    UINT uElapse = 1000 / gFPS;
    gAppTimerID = SetTimer(gAppWindow->getHandle(), nIDEvent, uElapse, nullptr);
    //printf("SetTimer: %lld\n", gAppTimerID);

}

// Controlling drawing
void forceRedraw()
{
    if (gDrawHandler != nullptr) {
        gDrawHandler();
    }

    if (!gIsLayered) {
        // if we're not layered, then do a regular
        // sort of WM_PAINT based drawing
        InvalidateRect(gAppWindow->getHandle(), NULL, 1);
    } else {
        if ((gAppSurface == nullptr)){
            printf("forceRedraw, NULL PTRs\n");
            return ;
        }

        LayeredWindowInfo lw(gAppSurface->getWidth(), gAppSurface->getHeight());
        lw.display(gAppWindow->getHandle(), gAppSurface->getDC());
    }
}

// Setup the routines that will handle
// keyboard and mouse events
void setupHandlers()
{
    // we're going to look within our own module
    // to find handler functions
    HMODULE hInst = GetModuleHandleA(NULL);

    // Start with our default handlers
    gKeyboardHandler = HandleKeyboardEvent;
    gMouseHandler = HandleMouseEvent;
    gTouchHandler = HandleTouchEvent;
    gPaintHandler = HandlePaintEvent;

    // Get the general app routines
    gPreloadHandler = (VOIDROUTINE)GetProcAddress(hInst, "preload");
    gSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "setup");
    gPreSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "presetup");
    gDrawHandler = (VOIDROUTINE)GetProcAddress(hInst, "draw");
    gLoopHandler = (VOIDROUTINE)GetProcAddress(hInst, "onLoop");

    // The user can specify their own handlers for io and
    // painting
    WinMSGObserver handler = (WinMSGObserver)GetProcAddress(hInst, "onPaint");
    if (handler != nullptr) {
            gPaintHandler = handler;
    }


    handler = (WinMSGObserver)GetProcAddress(hInst, "keyboardHandler");
    if (handler != nullptr) {
                    printf("key handler: %p\n", handler);
            gKeyboardHandler = handler;
    }

    handler = (WinMSGObserver)GetProcAddress(hInst, "mouseHandler");
    if (handler != nullptr) {
            gMouseHandler = handler;
    }

    handler = (WinMSGObserver)GetProcAddress(hInst, "touchHandler");
    if (handler != nullptr) {
            gTouchHandler = handler;
    }

    //printf("mouseHandler: %p\n", gMouseHandler);
    // If the user implements various event handlers, they will 
    // be called automatically
    gMouseMovedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseMoved");
    gMouseClickedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseClicked");
    gMousePressedHandler = (MouseEventHandler)GetProcAddress(hInst, "mousePressed");
    gMouseReleasedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseReleased");
    gMouseWheelHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseWheel");
    gMouseDraggedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseDragged");

    // Keyboard event handling
    gKeyPressedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyPressed");
    gKeyReleasedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyReleased");
    gKeyTypedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyTyped");

    // Touch event handling

    // Timer
    UINT_PTR nIDEvent = 5;
    setFrameRate(gFPS);
}


LRESULT MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //printf("MSG: 0x%04x\n", msg);
    LRESULT res = 0;
	PAINTSTRUCT ps;
	HDC hdc;


    if (msg == WM_DESTROY) {
        // By doing a PostQuitMessage(), a 
        // WM_QUIT message will eventually find its way into the
        // message queue.
        PostQuitMessage(0);
        return 0;
    } else if ((msg >= WM_MOUSEFIRST) && (msg <= WM_MOUSELAST)) {
        // Handle all mouse messages
		if (gMouseHandler != nullptr) {
			return gMouseHandler(hWnd, msg, wParam, lParam);
		}
    } else if ((msg >= WM_KEYFIRST) && (msg <= WM_KEYLAST)) {
        // Handle all keyboard messages
        if (gKeyboardHandler != nullptr) {
            gKeyboardHandler(hWnd, msg, wParam, lParam);
        }
    } else if (msg == WM_TOUCH) {
        // Handle touch specific messages
        if (gTouchHandler != nullptr) {
            gTouchHandler(hWnd, msg, wParam, lParam);
        }
            //res = TouchActivity(hwnd, msg, wparam, lparam)
    } else if (msg == WM_ERASEBKGND) {
        //printf("WM_ERASEBKGND\n");
        HDC hdc = HDC(wParam); 
        if (gPaintHandler != nullptr) {
            gPaintHandler(hWnd, msg, wParam, lParam);
        }

        // return non-zero indicating we erase the background
        res = 1;
    } else if (msg == WM_TIMER) {
        forceRedraw();
    } else if (msg == WM_PAINT) {
        //printf("WM_PAINT\n");
        if (gPaintHandler != nullptr) {
            gPaintHandler(hWnd, msg, wParam, lParam);
        } else
        {
            /* code */
            res = DefWindowProcA(hWnd, msg, wParam, lParam);
        }
    } else {
        res = DefWindowProcA(hWnd, msg, wParam, lParam);
    }

    return res;
}



// Controlling the runtime
void halt() {
    PostQuitMessage(0);
}

// turn looping on
void loop() {
    gLooping = true;
    setFrameRate(gFPS);
}

// turn looping off
// we still need to process the windows events
// but we stop calling draw() on a timer
void noLoop() {
    gLooping = false;

    // turn off timer
    // so drawing is not called in event loop
    BOOL bResult = KillTimer(gAppWindow->getHandle(), gAppTimerID);

    //printf("noLoop: %d  %Id\n", bResult, gAppTimerID);
}

void layered()
{
    gAppWindow->setExtendedStyle(WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);
    gIsLayered = true;
}

void noLayered()
{
    gAppWindow->clearExtendedStyle(WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);
    gIsLayered = false;
}

// A basic Windows event loop
void run()
{
    // Make sure we have all the event handlers connected
    setupHandlers();

    if (gPreloadHandler != nullptr) {
        gPreloadHandler();
    }

    // Call a setup routine if the user specified one
    if (gSetupHandler != nullptr) {
        gSetupHandler();
    }

    // Do a typical Windows message pump
    MSG msg;
    BOOL res;

    while (true) {
        //printf("run(), looping: %d\n", gLooping);

        if (gLooping && (gLoopHandler != nullptr)) {
            gLoopHandler();
        }

        // we use peekmessage, so we don't stall on a GetMessage
        //while (C.PeekMessageA(msg, nil, 0, 0, C.PM_REMOVE) ~= 0) do
        BOOL bResult = PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        
        if (bResult > 0) {
            // If we see a quit message, it's time to stop the program
            if (msg.message == WM_QUIT) {
                break;
            }

            res = TranslateMessage(&msg);
            res = DispatchMessageA(&msg);
        }
    }
}


void setWindowPosition(int x, int y)
{
    gAppWindow->moveTo(x, y);
}

bool setCanvasSize(size_t aWidth, size_t aHeight)
{
    // Create new drawing surface
    if (gAppSurface != nullptr) {
        // Delete old one if it exists
        delete gAppSurface;
    }
    //gAppSurface = new PixelBufferRGBA32(aWidth,aHeight);
    gAppSurface = new PBDIBSection(aWidth, aHeight);
    gAppSurface->setAllPixels(PixRGBA(0x00000000));


    // create new layering blitter
    if (gLayeredWindow != nullptr) {
        delete gLayeredWindow;
    }
    gLayeredWindow = new LayeredWindowInfo(aWidth, aHeight);

    // Delete old drawing context if it exists
    if (gAppDC != nullptr) {
        delete gAppDC;
    }

    // Create a new one
    gAppDC = new DrawingContext(*gAppSurface);

    return true;
}

// do any initialization that needs to occur 
// in the very beginning
void prolog()
{
    // Initialize Windows networking
    uint16_t version = MAKEWORD(2,2);
    WSADATA lpWSAData;
    int res = WSAStartup(version, &lpWSAData);
}

// Do whatever cleanup needs to be done before
// exiting application
void epilog()
{
    WSACleanup();
}


/*
    The 'main()' function is in here to ensure that compiling
    this header will result in an executable file.

    The code for the user just needs to implement the 'setup()'
    and other functions.
*/
// Declare some standard Window Kinds we'll be using
User32WindowClass gAppWindowKind("appwindow", CS_GLOBALCLASS | CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW, MsgHandler);


int main(int argc, char **argv)
{
    gargc = argc;
    gargv = argv;
    
    prolog();

    // set the canvas a default size to start
    // but don't show it
    setCanvasSize(320, 240);

    gAppWindow = gAppWindowKind.createWindow("Application Window", 320, 240);
    
    run();

    // do any cleanup after all is done
    epilog();

    return 0;
}