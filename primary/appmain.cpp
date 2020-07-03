

#include "apphost.h"

#include "LayeredWindow.hpp"
#include "TimeTicker.hpp"
#include "joystick.h"
#include "stopwatch.hpp"


#include <cstdio>
#include <array>

// These are here to be universal

template <> template <> vec<3, int>  ::vec(const vec<3, float>& v) : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}
template <> template <> vec<3, float>::vec(const vec<3, int>& v) : x((float)v.x), y((float)v.y), z((float)v.z) {}
template <> template <> vec<2, int>  ::vec(const vec<2, float>& v) : x(int(v.x + .5f)), y(int(v.y + .5f)) {}
template <> template <> vec<2, float>::vec(const vec<2, int>& v) : x((float)v.x), y((float)v.y) {}



// Some function signatures
typedef LRESULT (*WinMSGObserver)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


// Application routines
static VOIDROUTINE gDrawHandler = nullptr;
static VOIDROUTINE gLoopHandler = nullptr;
static VOIDROUTINE gFrameHandler = nullptr;
static VOIDROUTINE gPreloadHandler = nullptr;
static VOIDROUTINE gSetupHandler = nullptr;
static VOIDROUTINE gPreSetupHandler = nullptr;
static PFNDOUBLE1 gUpdateHandler = nullptr;

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

// Joystick
static WinMSGObserver gJoystickHandler = nullptr;
static JoystickEventHandler gJoystickPressedHandler = nullptr;
static JoystickEventHandler gJoystickReleasedHandler = nullptr;
static JoystickEventHandler gJoystickMovedHandler = nullptr;
static JoystickEventHandler gJoystickMovedZHandler = nullptr;

// Touch
static WinMSGObserver gTouchHandler = nullptr;
static TouchEventHandler gTouchStartedHandler = nullptr;
static TouchEventHandler gTouchEndedHandler = nullptr;
static TouchEventHandler gTouchMovedHandler = nullptr;
static TouchEventHandler gTouchHoverHandler = nullptr;

// Pointer
static WinMSGObserver gPointerHandler = nullptr;

// Miscellaneous globals
int gargc;
char **gargv;

TimeTicker *gAppTicker = nullptr;
StopWatch appStopWatch;
double deltaTime = 0;
double gAppLastTime = 0;

int gFPS = 15;   // Frames per second
User32Window * gAppWindow = nullptr;
Surface * gAppSurface = nullptr;

UINT_PTR gAppTimerID = 0;
bool gLooping = true;
bool gRunning = true;
bool gIsLayered = false;

// Some globals friendly to the p5 environment
// Display Globals
int canvasWidth = 0;
int canvasHeight = 0;

int displayWidth = 0;
int displayHeight= 0;
unsigned int displayDpi = 0;

// Client Area globals
int clientLeft;
int clientTop;

// Keyboard globals
int keyCode = 0;
int keyChar = 0;

// Mouse Globals
bool mouseIsPressed = false;
int mouseX = 0;
int mouseY = 0;
int mouseDelta = 0;
int pmouseX = 0;
int pmouseY = 0;

// Raw Mouse input
int rawMouseX = 0;
int rawMouseY = 0;

// Joystick
static Joystick gJoystick1(JOYSTICKID1);
static Joystick gJoystick2(JOYSTICKID2);

//
//    https://docs.microsoft.com/en-us/windows/desktop/inputdev/using-raw-input
//
//Raw input utility functions
static const USHORT HID_MOUSE = 2;
static const USHORT HID_JOYSTICK = 4;
static const USHORT HID_GAMEPAD = 5;
static const USHORT HID_KEYBOARD = 6;

// Register for mouseand keyboard
void HID_RegisterDevice(HWND hTarget, USHORT usage, USHORT usagePage = 1)
{
    RAWINPUTDEVICE hid[1];

    hid[0].usUsagePage = usagePage;
    hid[0].usUsage = usage;
    hid[0].dwFlags = (RIDEV_DEVNOTIFY | RIDEV_INPUTSINK);
    hid[0].hwndTarget = hTarget;
    UINT uiNumDevices = 1;

    BOOL bResult = ::RegisterRawInputDevices(hid, uiNumDevices, sizeof(RAWINPUTDEVICE));
    //printf("HID_RegisterDevice: HWND: 0x%p,  %d  %d\n", hTarget, bResult, ::GetLastError());
}

void HID_UnregisterDevice(USHORT usage)
{
    RAWINPUTDEVICE hid{ 0 };
    hid.usUsagePage = 1;
    hid.usUsage = usage;
    hid.dwFlags = RIDEV_REMOVE;
    hid.hwndTarget = nullptr;
    UINT uiNumDevices = 1;

    BOOL bResult = ::RegisterRawInputDevices(&hid, 1, sizeof(RAWINPUTDEVICE));
}


// Controlling drawing
void fakeRedraw(void* param, int64_t tickCount)
{
    //printf("FAKE Redraw\n");
}

void forceRedraw(void* param, int64_t tickCount)
{
    if (gFrameHandler != nullptr) {
        gFrameHandler();
    }

    if (gDrawHandler != nullptr) {
        gDrawHandler();
    }

    if (!gIsLayered) {
        // if we're not layered, then do a regular
        // sort of WM_PAINT based drawing
        InvalidateRect(gAppWindow->getHandle(), NULL, 1);
    }
    else {
        if ((gAppSurface == nullptr)) {
            printf("forceRedraw, NULL PTRs\n");
            return;
        }

        gAppSurface->flush();
        LayeredWindowInfo lw(canvasWidth, canvasHeight);
        lw.display(gAppWindow->getHandle(), ((Surface *)gAppSurface)->getDC());
    }
}

/*
    Environment
*/
void show()
{
    gAppWindow->show();
}

void hide()
{
    gAppWindow->hide();
}

void cursor()
{
    int count = ShowCursor(1);
}

// Show the cursor, if there is one
// BUGBUG - we should be more robust here and
// check to see if there's even a mouse attached
// Then, decrement count enough times to make showing
// the cursor absolute rather than relative.
void noCursor()
{
    ShowCursor(0);
}

void setFrameRate(int newRate)
{
    gFPS = newRate;

    if (gAppTicker != nullptr) {
        gAppTicker->stop();
        delete gAppTicker;
    }

    /*
        int64_t interval = (int64_t)(1000.0 / gFPS);
        gAppTicker = new TimeTicker(interval, fakeRedraw, nullptr);

        if (gLooping) {
            gAppTicker->start();
        }
    */

    //printf("setFrameRate: %d\n", newRate);
    UINT_PTR nIDEvent = 5;
    BOOL bResult = KillTimer(gAppWindow->getHandle(), gAppTimerID);
    //printf("KillTimer: %d %lld\n", bResult, gAppTimerID);
    UINT uElapse = 1000 / gFPS;
    gAppTimerID = SetTimer(gAppWindow->getHandle(), nIDEvent, uElapse, nullptr);
    //printf("SetTimer: %lld\n", gAppTimerID);
}





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

            if (mouseIsPressed && (gMouseDraggedHandler != nullptr)) {
                e.activity = MOUSEDRAGGED;
                gMouseDraggedHandler(e);
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
            mouseDelta = e.delta;
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


// 
// Handling the joystick messages through the Windows
// Messaging method is very limited.  It will only 
// trigger for a limited set of buttons and axes movements
// This handler is here for a complete API.  If the user app
// wants to get more out of the joystick, it can access the
// joystick directly with 'gJoystick1' or 'gJoystick2' and
// call getPosition() at any time.  Typically during 'update()'
// or 'draw()'
LRESULT HandleJoystickEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;


    JoystickEvent e;

    // We can handle up to two joysticks using
    // this mechanism
    switch (msg) {
    case MM_JOY1BUTTONDOWN:
        gJoystick1.getPosition(e);
        e.activity = JOYPRESSED;
        if (gJoystickPressedHandler)
            gJoystickPressedHandler(e);
        break;

    case MM_JOY2BUTTONDOWN:
        gJoystick2.getPosition(e);
        e.activity = JOYPRESSED;
        if (gJoystickPressedHandler)
            gJoystickPressedHandler(e);
        break;

    case MM_JOY1BUTTONUP:
        gJoystick1.getPosition(e);
        e.activity = JOYRELEASED;
        if (gJoystickReleasedHandler != nullptr)
            gJoystickReleasedHandler(e);
        break;
    case MM_JOY2BUTTONUP:
        gJoystick2.getPosition(e);
        e.activity = JOYRELEASED;
        if (gJoystickReleasedHandler != nullptr)
            gJoystickReleasedHandler(e);
        break;

    case MM_JOY1MOVE:
        gJoystick1.getPosition(e);
        e.activity = JOYMOVED;
        if (gJoystickMovedHandler)
            gJoystickMovedHandler(e);
        break;
    case MM_JOY2MOVE:
        gJoystick2.getPosition(e);
        e.activity = JOYMOVED;
        if (gJoystickMovedHandler)
            gJoystickMovedHandler(e);
        break;

    case MM_JOY1ZMOVE:
        gJoystick1.getPosition(e);
        e.activity = JOYZMOVED;
        if (gJoystickMovedZHandler)
            gJoystickMovedZHandler(e);
    break;
    case MM_JOY2ZMOVE:
        gJoystick2.getPosition(e);
        e.activity = JOYZMOVED;
        if (gJoystickMovedZHandler)
            gJoystickMovedZHandler(e);
        break;
    }


    return res;
}

LRESULT HandleTouchEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    //TOUCHINPUT pInputs[10];

    // cInputs could be set to a maximum value (10) and
    // we could reuse the same allocated array each time
    // rather than allocating a new one each time.
    //printf("wm_touch_event 0.0: %d\n", wParam);
    int cInputs = LOWORD(wParam);
    int cbSize = sizeof(TOUCHINPUT);

    //printf("wm_touch_event 1.0: %d\n", cInputs);
    PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];

    // 0 == failure?
    BOOL bResult = GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, cbSize);

    if (bResult == 0) {
        auto err = ::GetLastError();
        printf("getTouchInputInfo, ERROR: %d %d\n", bResult, err);

        return 0;
    }

    // construct an even for each item
    for (int i = 0; i < cInputs; i++) {
        POINT PT;
        PT.x = pInputs[i].x / 100;
        PT.y = pInputs[i].y / 100;
        //print("wm_touch_event 4.1: ", PT.x, PT.y)
        auto bResult = ::ScreenToClient(hwnd, &PT);
        //printf("wm_touch_event 4.2: ", bResult, PT.x, PT.y)
        TouchEvent e;
        e.id = pInputs[i].dwID;
        e.x = PT.x;
        e.y = PT.y;
        e.rawX = pInputs[i].x;
        e.rawY = pInputs[i].y;


        if ((pInputs[i].dwMask & TOUCHINPUTMASKF_CONTACTAREA) != 0) {
            e.rawWidth = pInputs[i].cxContact;
            e.rawHeight = pInputs[i].cyContact;
            e.w = e.rawWidth / 100;
            e.h = e.rawHeight / 100;
        }

        // switch based on activity
        if (pInputs[i].dwFlags & TOUCHEVENTF_DOWN) {
            if (gTouchStartedHandler)
                gTouchStartedHandler(e);
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_UP) {
            if (gTouchEndedHandler)
                gTouchEndedHandler(e);
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE) {
            if (gTouchMovedHandler)
                gTouchMovedHandler(e);
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_INRANGE) {
            if (gTouchHoverHandler)
                gTouchHoverHandler(e);
        }

    }
    delete[] pInputs;

    if (!CloseTouchInputHandle((HTOUCHINPUT)lParam))
    {
        // error handling
    }

    //print("wm_touch_event 5.0: ", bResult)

    return res;
}

LRESULT HandlePointerEvent(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

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
    int DestWidth = canvasWidth;
    int DestHeight = canvasHeight;
    int xSrc = 0;
    int ySrc = 0;
    int SrcWidth = canvasWidth;
    int SrcHeight = canvasHeight;

    BITMAPINFO info = gAppSurface->getBitmapInfo();
    
    // Make sure we sync all current drawing
    gAppSurface->flush();

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
    gJoystickHandler = HandleJoystickEvent;
    gTouchHandler = HandleTouchEvent;
    gPointerHandler = HandlePointerEvent;
    gPaintHandler = HandlePaintEvent;

    // The user can specify their own handlers for io and
    // painting.  If they don't specify a handler, then use
    // the one that are inbuilt.
    WinMSGObserver handler = (WinMSGObserver)GetProcAddress(hInst, "onPaint");
    if (handler != nullptr) {
        gPaintHandler = handler;
    }

    handler = (WinMSGObserver)GetProcAddress(hInst, "keyboardHandler");
    if (handler != nullptr) {
        gKeyboardHandler = handler;
    }

    handler = (WinMSGObserver)GetProcAddress(hInst, "mouseHandler");
    if (handler != nullptr) {
        gMouseHandler = handler;
    }

    handler = (WinMSGObserver)GetProcAddress(hInst, "joystickHandler");
    if (handler != nullptr) {
        gJoystickHandler = handler;
    }

    handler = (WinMSGObserver)GetProcAddress(hInst, "touchHandler");
    if (handler != nullptr) {
        gTouchHandler = handler;
    }

    handler = (WinMSGObserver)GetProcAddress(hInst, "handlePointer");
    if (handler != nullptr) {
        gPointerHandler = handler;
    }

    // Get the general app routines
    gPreloadHandler = (VOIDROUTINE)GetProcAddress(hInst, "preload");
    gSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "setup");
    gPreSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "presetup");
    gDrawHandler = (VOIDROUTINE)GetProcAddress(hInst, "draw");
    gUpdateHandler = (PFNDOUBLE1)GetProcAddress(hInst, "update");
    gFrameHandler = (VOIDROUTINE)GetProcAddress(hInst, "onFrame");



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

    // Joystick
    gJoystickPressedHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyPressed");
    gJoystickReleasedHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyReleased");
    gJoystickMovedHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyMoved");
    gJoystickMovedZHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyMovedZ");

    // Touch event routines
    gTouchStartedHandler = (TouchEventHandler)GetProcAddress(hInst, "touchStarted");
    gTouchEndedHandler = (TouchEventHandler)GetProcAddress(hInst, "touchEnded");
    gTouchMovedHandler = (TouchEventHandler)GetProcAddress(hInst, "touchMoved");
    gTouchHoverHandler = (TouchEventHandler)GetProcAddress(hInst, "touchHover");


    // Timer
    UINT_PTR nIDEvent = 5;
    setFrameRate(gFPS);
}


LRESULT CALLBACK MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //printf("MSG: 0x%04x\n", msg);
    LRESULT res = 0;

    if (msg == WM_INPUT) {
        //printf("WM_INPUT\n");
        bool inBackground = GET_RAWINPUT_CODE_WPARAM(wParam) == 1;
        HRAWINPUT inputHandle = (HRAWINPUT)lParam;
        UINT uiCommand = RID_INPUT;
        UINT cbSize;

        // First, find out how much space will be needed
        UINT size = ::GetRawInputData((HRAWINPUT)lParam, uiCommand, nullptr, &cbSize, sizeof(RAWINPUTHEADER));


        // Allocate space, and try it again
        std::vector<uint8_t> buff(cbSize, 0);
        size = ::GetRawInputData((HRAWINPUT)lParam, uiCommand, buff.data(), &cbSize, sizeof(RAWINPUTHEADER));
        //printf("WM_INPUT: %d - %d\n", cbSize, size);
        if (size == cbSize) {
            RAWINPUT* raw = (RAWINPUT*)buff.data();

            // See what we got
            //printf("RAWINPUT: %d\n", raw->header.dwType);

            switch (raw->header.dwType) {
                case RIM_TYPEMOUSE: {
                    rawMouseX = raw->data.mouse.lLastX;
                    rawMouseY = raw->data.mouse.lLastY;
                    //printf("RAWMOUSE: %d %d\n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
                }
                break;

                    case RIM_TYPEKEYBOARD: {

                    }
                                         break;
                    }
                }
    } else if (msg == WM_DESTROY) {
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
    } else if ((msg >= MM_JOY1MOVE) && (msg <= MM_JOY2BUTTONUP)) {
        //printf("MM_JOYxxx: %p\n", gJoystickHandler);
        if (gJoystickHandler != nullptr) {
            gJoystickHandler(hWnd, msg, wParam, lParam);
        }
    }
    else if (msg == WM_TOUCH) {
        // Handle touch specific messages
        if (gTouchHandler != nullptr) {
            res = gTouchHandler(hWnd, msg, wParam, lParam);
        }

    } else if ((msg >= WM_NCPOINTERUPDATE) && (msg <= WM_POINTERROUTEDRELEASED)){
        if (gPointerHandler != nullptr) {
            res = gPointerHandler(hWnd, msg, wParam, lParam);
        }
    } else if (msg == WM_ERASEBKGND) {
        //printf("WM_ERASEBKGND\n");
        if (gPaintHandler != nullptr) {
            gPaintHandler(hWnd, msg, wParam, lParam);
        }

        // return non-zero indicating we erase the background
        res = 1;
    } else if (msg == WM_TIMER) {
        forceRedraw(nullptr, 0);
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

void rawInput()
{
    HWND localWindow = gAppWindow->getHandle();

    HID_RegisterDevice(gAppWindow->getHandle(), HID_MOUSE);
    HID_RegisterDevice(gAppWindow->getHandle(), HID_KEYBOARD);
}

void noRawInput()
{
    // unregister devices
    HID_UnregisterDevice(HID_MOUSE);
    HID_UnregisterDevice(HID_KEYBOARD);
}

void joystick()
{
    gJoystick1.attachToWindow(gAppWindow->getHandle());
    gJoystick2.attachToWindow(gAppWindow->getHandle());
}

void noJoystick()
{
    gJoystick1.detachFromWindow();
    gJoystick2.detachFromWindow();
}

// Turning Touch input on and off
bool touch()
{
    BOOL bResult = RegisterTouchWindow(gAppWindow->getHandle(), 0);
    return (bResult != 0);
}

bool noTouch()
{
    BOOL bResult = UnregisterTouchWindow(gAppWindow->getHandle());
    return (bResult != 0);
}

bool isTouch()
{
    ULONG flags = 0;
    BOOL bResult = IsTouchWindow(gAppWindow->getHandle(), &flags);
    return (bResult != 0);
}

//
// Window management
//
static LONG gLastWindowStyle=0;

void layered()
{
    gAppWindow->setExtendedStyle(WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);
    gLastWindowStyle = gAppWindow->setWindowStyle(WS_POPUP);

    gIsLayered = true;
}

void noLayered()
{
    gAppWindow->clearExtendedStyle(WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);
    gAppWindow->setWindowStyle(gLastWindowStyle);

    gIsLayered = false;
}

bool isLayered()
{
    return gIsLayered;
}

void setWindowPosition(int x, int y)
{
    gAppWindow->moveTo(x, y);
}

bool setCanvasSize(long aWidth, long aHeight)
{
    // Create new drawing surface
    if (gAppSurface != nullptr) {
        // Delete old one if it exists
        delete gAppSurface;
    }

    gAppSurface = new Surface(aWidth, aHeight);
    canvasWidth = aWidth;
    canvasHeight = aHeight;

    return true;
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

    // do drawing at least once in case
    // the user calls noLoop() within 
    // the setup() routine
    forceRedraw(nullptr, 0);


    // Do a typical Windows message pump
    MSG msg;
    LRESULT res;

    gAppWindow->show();
    deltaTime = appStopWatch.seconds();

    while (true) {
        if (gUpdateHandler != nullptr) {

            double currentTime = appStopWatch.seconds();
            deltaTime = currentTime- gAppLastTime;
            gAppLastTime = currentTime;
            gUpdateHandler(deltaTime);
        }

        // dealing with loop(), noLoop(), typically 'draw()'
        //if (gLooping && (gLoopHandler != nullptr)) {
        //    gLoopHandler();
        //}

        // we use peekmessage, so we don't stall on a GetMessage
        // should probably throw a wait here
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


/*
    The 'main()' function is in here to ensure that compiling
    this header will result in an executable file.

    The code for the user just needs to implement the 'setup()'
    and other functions.
*/
// Declare some standard Window Kinds we'll be using
User32WindowClass gAppWindowKind("appwindow", CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, MsgHandler);


// do any initialization that needs to occur 
// in the very beginning
bool prolog()
{
    // Initialize Windows networking
    // BUGBUG - decide whether or not we care about WSAStartup failing
    uint16_t version = MAKEWORD(2,2);
    WSADATA lpWSAData;
    int res = WSAStartup(version, &lpWSAData);


    // Get the screen size
    //First thing to do is let the system know we are
    // going to be DPI aware
    DPI_AWARENESS_CONTEXT oldContext = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    displayDpi = ::GetDpiForSystem();
    displayWidth = ::GetSystemMetrics(SM_CXSCREEN);
    displayHeight = ::GetSystemMetrics(SM_CYSCREEN);
    //printf("appmain.prolog, width: %d  height: %d  DPI: %d\n", displayWidth, displayHeight, displayDpi);

    // set the canvas a default size to start
    // but don't show it
    setCanvasSize(320, 240);

    gAppWindow = gAppWindowKind.createWindow("Application Window", 320, 240);

    // Get client area
    RECT cRect;
    GetClientRect(gAppWindow->getHandle(), &cRect);
    clientLeft = cRect.left;
    clientTop = cRect.top;

    return true;
}

// Do whatever cleanup needs to be done before
// exiting application
void epilog()
{
    WSACleanup();
}


/*
    Why on Earth are there two 'main' routines in here?
    the first one 'main()' will be called when the code is compiled
    as a console application.

    the second one 'WinMain' will be called when the code is compiled
    as a WINDOWS subsystem.  by having both, we kill two birds with 
    one stone.
*/
int ndtRun()
{
    if (!prolog()) {
        printf("error in prolog\n");
        return -1;
    }

    run();

    // do any cleanup after all is done
    epilog();

    return 0;
}

int main(int argc, char **argv)
{
    gargc = argc;
    gargv = argv;
    
    return ndtRun();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmdLine, int nShowCmd)
{
    // BUGBUG, need to parse those command line arguments
    //gargc = argc;
    //gargv = argv;

    return ndtRun();
}