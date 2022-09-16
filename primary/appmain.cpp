
/*
    The primary function of this file is to act as a bridge between 
    the Windows environment, and our desired, fairly platform independent
    application environment.

    All you need to setup a Windows application is this file, and the accompanying headers.
    It will operate either in console, or Windows mode.

    This file deals with user input (mouse, keyboard, pointer, joystick, touch)
    initiating a pub/sub system for applications to subscribe to.

    The design is meant to be the smallest tightest bare essentials of Windows code
    necessary to write fairly decent applications.
*/

#include "apphost.h"

#include "LayeredWindow.hpp"
#include "joystick.h"
#include "fonthandler.hpp"

#include <shellapi.h>   // for drag-drop support

#include <cstdio>
#include <array>
#include <iostream>
#include <memory>

// Some function signatures
// WinMSGObserver - Function signature for Win32 message handler
typedef LRESULT (*WinMSGObserver)(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define LODWORD(ull) ((DWORD)((ULONGLONG)(ull) & 0x00000000ffffffff))
#define HIDWORD(ull) ((DWORD)((((ULONGLONG)(ull))>>32) & 0x00000000ffffffff))

// Application routines
// appmain looks for this routine in the compiled application
static VOIDROUTINE gOnloadHandler = nullptr;
static VOIDROUTINE gOnUnloadHandler = nullptr;

static VOIDROUTINE gOnLoopHandler = nullptr;

// Painting
static WinMSGObserver gPaintHandler = nullptr;

// Topics applications can subscribe to
SignalEventTopic gSignalEventTopic;
KeyboardEventTopic gKeyboardEventTopic;
MouseEventTopic gMouseEventTopic;
JoystickEventTopic gJoystickEventTopic;
FileDropEventTopic gFileDropEventTopic;
TouchEventTopic gTouchEventTopic;
PointerEventTopic gPointerEventTopic;
GestureEventTopic gGestureEventTopic;

// Miscellaneous globals
int gargc;
char **gargv;


User32Window * gAppWindow = nullptr;
std::shared_ptr<Surface>  gAppSurface = nullptr;
std::shared_ptr<FontHandler> gFontHandler = nullptr;


bool gIsLayered = false;

// Some globals friendly to the p5 environment
// Display Globals
int canvasWidth = 0;
int canvasHeight = 0;
Pixel* canvasPixels = nullptr;
size_t canvasStride = 0;

int displayWidth = 0;
int displayHeight= 0;
unsigned int systemDpi = 96;    // 96 == px measurement
unsigned int systemPpi = 192;   // starting pixel density

// Client Area globals
int clientLeft;
int clientTop;

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
// This function is called at any time to display whatever is in the app 
// window to the actual screen
void screenRefresh()
{
    if ((gAppSurface == nullptr)) {
        printf("forceRedraw, NULL PTRs\n");
        return;
    }

    if (!gIsLayered) {
        // if we're not layered, then do a regular
        // sort of WM_PAINT based drawing
        InvalidateRect(gAppWindow->getHandle(), NULL, 1);
    }
    else {
        LayeredWindowInfo lw(canvasWidth, canvasHeight);
        lw.display(gAppWindow->getHandle(), gAppSurface->getDC());
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
    int count = ::ShowCursor(1);
}

// Show the cursor, if there is one
// BUGBUG - we should be more robust here and
// check to see if there's even a mouse attached
// Then, decrement count enough times to make showing
// the cursor absolute rather than relative.
void noCursor()
{
    ::ShowCursor(0);
}



/*
    Turn Windows keyboard messages into keyevents that can 
    more easily be handled at the application level
*/
LRESULT HandleKeyboardMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    KeyboardEvent e;
    e.keyCode = (int)wParam;
    e.repeatCount =LOWORD(lParam);  // 0 - 15
    e.scanCode = ((lParam & 0xff0000) >> 16);        // 16 - 23
    e.isExtended = (lParam & 0x1000000) != 0;    // 24
    e.wasDown = (lParam & 0x40000000) != 0;    // 30

//printf("HandleKeyboardMessage: %04x\n", msg);

    switch(msg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            e.activity = KEYPRESSED;

            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            e.activity = KEYRELEASED;

            break;
        case WM_CHAR:
        case WM_SYSCHAR:
            e.activity = KEYTYPED;
            break;
    }
    
    // publish keyboard event
    gKeyboardEventTopic.notify(e);

    return res;
}


/*
    Turn Windows mouse messages into mouse events which can
    be dispatched by the application.
*/
LRESULT HandleMouseMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{   
    LRESULT res = 0;
    MouseEvent e;

    e.x = GET_X_LPARAM(lParam);
    e.y = GET_Y_LPARAM(lParam);

    e.control = (wParam & MK_CONTROL) != 0;
    e.shift = (wParam& MK_SHIFT) != 0;
    e.lbutton = (wParam & MK_LBUTTON) != 0;
    e.rbutton = (wParam & MK_RBUTTON) != 0;
    e.mbutton = (wParam & MK_MBUTTON) != 0;
    e.xbutton1 = (wParam & MK_XBUTTON1) != 0;
    e.xbutton2 = (wParam & MK_XBUTTON2) != 0;
    bool isPressed = e.lbutton || e.rbutton || e.mbutton;

    switch(msg) {
        case WM_LBUTTONDBLCLK:
	    case WM_MBUTTONDBLCLK:
	    case WM_RBUTTONDBLCLK:
            break;

        case WM_MOUSEMOVE:
            e.activity = MOUSEMOVED;
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            e.activity = MOUSEPRESSED;
            break;
        
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            e.activity = MOUSERELEASED;
            break;
        
        case WM_MOUSEWHEEL:
            e.activity = MOUSEWHEEL;
            e.delta = GET_WHEEL_DELTA_WPARAM(wParam);
            break;
        
        case WM_MOUSEHWHEEL:
            e.activity = MOUSEHWHEEL;
            e.delta = GET_WHEEL_DELTA_WPARAM(wParam);
            break;

        default:
            
        break;
    }

    gMouseEventTopic.notify(e);

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
LRESULT HandleJoystickMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    JoystickEvent e;

    // We can handle up to two joysticks using
    // this mechanism
    switch (msg) {
    case MM_JOY1BUTTONDOWN:
        gJoystick1.getPosition(e);
        e.activity = JOYPRESSED;
        break;

    case MM_JOY2BUTTONDOWN:
        gJoystick2.getPosition(e);
        e.activity = JOYPRESSED;
        break;

    case MM_JOY1BUTTONUP:
        gJoystick1.getPosition(e);
        e.activity = JOYRELEASED;
        break;
    case MM_JOY2BUTTONUP:
        gJoystick2.getPosition(e);
        e.activity = JOYRELEASED;
        break;

    case MM_JOY1MOVE:
        gJoystick1.getPosition(e);
        e.activity = JOYMOVED;
        break;
    case MM_JOY2MOVE:
        gJoystick2.getPosition(e);
        e.activity = JOYMOVED;
        break;

    case MM_JOY1ZMOVE:
        gJoystick1.getPosition(e);
        e.activity = JOYZMOVED;
    break;
    case MM_JOY2ZMOVE:
        gJoystick2.getPosition(e);
        e.activity = JOYZMOVED;
        break;
    }

    gJoystickEventTopic.notify(e);

    return res;
}

LRESULT HandleTouchMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    // cInputs could be set to a maximum value (10) and
    // we could reuse the same allocated array each time
    // rather than allocating a new one each time.
    //std::cout << "wm_touch_event: " << wParam << std::endl;

    int cInputs = LOWORD(wParam);
    int cbSize = sizeof(TOUCHINPUT);

    //printf("HandleTouchMessage 1.0: %d\n", cInputs);
    PTOUCHINPUT pInputs = new TOUCHINPUT[cInputs];

    // 0 == failure?
    BOOL bResult = GetTouchInputInfo((HTOUCHINPUT)lParam, cInputs, pInputs, cbSize);

    if (bResult == 0) {
        delete[] pInputs;
        DWORD err = ::GetLastError();
        //printf("getTouchInputInfo, ERROR: %d %D\n", bResult, err);

        return 0;
    }

    // construct an event for each item
    // BUGBUG - really this should be a deque of events
    // like with file drops
    for (int i = 0; i < cInputs; i++) {
        TOUCHINPUT ti = pInputs[i];
        TouchEvent e = {};

        // convert values to local TouchEvent structure
        e.id = ti.dwID;
        e.rawX = ti.x;
        e.rawY = ti.y;

        POINT PT;
        PT.x = TOUCH_COORD_TO_PIXEL(ti.x);
        PT.y = TOUCH_COORD_TO_PIXEL(ti.y);

        // Convert from screen coordinates to local
        // client coordinates
        auto bResult = ::ScreenToClient(hwnd, &PT);

        // Assign x,y the client area value of the touch point
        e.x = PT.x;
        e.y = PT.y;


        // Now, deal with masks
        //#define TOUCHINPUTMASKF_TIMEFROMSYSTEM  0x0001  // the dwTime field contains a system generated value
        //#define TOUCHINPUTMASKF_EXTRAINFO       0x0002  // the dwExtraInfo field is valid
        //#define TOUCHINPUTMASKF_CONTACTAREA     0x0004  // the cxContact and cyContact fields are valid

        if ((ti.dwMask & TOUCHINPUTMASKF_CONTACTAREA) != 0) {
            e.rawWidth = ti.cxContact;
            e.rawHeight = ti.cyContact;
            e.w = TOUCH_COORD_TO_PIXEL(e.rawWidth);
            e.h = TOUCH_COORD_TO_PIXEL(e.rawHeight);
        }

        // figure out kind of activity and attributes
        /*
        #define TOUCHEVENTF_MOVE            0x0001
#define TOUCHEVENTF_DOWN            0x0002
#define TOUCHEVENTF_UP              0x0004
#define TOUCHEVENTF_INRANGE         0x0008
#define TOUCHEVENTF_PRIMARY         0x0010
#define TOUCHEVENTF_NOCOALESCE      0x0020
#define TOUCHEVENTF_PEN             0x0040
#define TOUCHEVENTF_PALM            0x0080
        */
        if (ti.dwFlags & TOUCHEVENTF_DOWN) {
            e.activity = TOUCH_DOWN;
            e.isDown = true;
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_UP) {
            e.activity = TOUCH_UP;
            e.x = -1;
            e.y = -1;
            e.isUp = true;
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE) {
            e.activity = TOUCH_MOVE;
            e.isMoving = true;
        }

        // Attributes of the event
        if (pInputs[i].dwFlags & TOUCHEVENTF_INRANGE) {
            e.isHovering = true;
        }

        if (ti.dwFlags & TOUCHEVENTF_PRIMARY) {
            e.isPrimary = true;
        }

        if (pInputs[i].dwFlags & TOUCHEVENTF_PALM) {
            e.isPalm = true;
        }
        
        if (pInputs[i].dwFlags & TOUCHEVENTF_PEN) {
            e.isPen = true;
        }

        gTouchEventTopic.notify(e);
    }
    delete[] pInputs;

    if (!CloseTouchInputHandle((HTOUCHINPUT)lParam))
    {
        // error handling
    }

    return res;
}

LRESULT HandlePointerMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    PointerEvent e;

    gPointerEventTopic.notify(e);

    return res;
}

LRESULT HandlePaintMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //printf("HandlePaintMessage\n");

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
        gAppSurface->getPixels(),&info,
        DIB_RGB_COLORS,
        SRCCOPY);
        
	EndPaint(hWnd, &ps);

    return res;
}

LRESULT HandleFileDropMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;
    HDROP dropHandle = (HDROP)wParam;
    

    // if we have a drop handler, then marshall all the file names
    // and call the event handler. 
    // If the handler does not exist, don't bother with all the work

    FileDropEvent e;
    // Find out where the drop occured
    POINT pt;
    ::DragQueryPoint(dropHandle, &pt);
    e.x = pt.x;
    e.y = pt.y;

    // First, find out how many files were dropped
    auto n = ::DragQueryFileA(dropHandle, 0xffffffff, nullptr, 0);

    // Now that we know how many, query individual files
    // FileDropEvent
    char namebuff[512];

    if (n > 0) {
        for (size_t i = 0; i < n; i++) {
            ::DragQueryFileA(dropHandle, (UINT)i, namebuff, 512);
            e.filenames.push_back(std::string(namebuff));
        }

        gFileDropEventTopic.notify(e);
    }

    ::DragFinish(dropHandle);

    return res;
}

// Handling gestures
// Gesture messages WM_GESTURE, will only arrive if we're NOT
// registered for touch messages.  In an app, it might be useful
// to call noTouch() to ensure this is the case.
//
// https://github.com/microsoft/Windows-classic-samples/blob/master/Samples/Win7Samples/Touch/MTGestures/cpp/GestureEngine.h
//
LRESULT HandleGestureMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    //std::cout << "HandleGestureMessage" << std::endl;
    GESTUREINFO gi;
    ZeroMemory(&gi, sizeof(GESTUREINFO));
    gi.cbSize = sizeof(GESTUREINFO);

    // get the gesture information
    BOOL bResult = GetGestureInfo((HGESTUREINFO)lParam, &gi);


    // If getting gesture info fails, return immediately
    // no further processing.
    // This will look the same as us handling the gesture, and
    // it might be better to throw some form of error
    if (!bResult) {
        //DWORD dwErr = ::GetLastError();
        return FALSE;
    }

    // if the gesture ID is BEGIN, or END, we can allow the default
    // behavior handle it
    switch (gi.dwID) {
    case GID_BEGIN:
    case GID_END:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    // If we are here, we can process well known gestures
    LRESULT res = 0;
    BOOL bHandled = FALSE;

    // Turn the physical screen location into local
    // window location
    POINT ptPoint;
    ptPoint.x = gi.ptsLocation.x;
    ptPoint.y = gi.ptsLocation.y;
    ::ScreenToClient(hWnd, &ptPoint);
    
    GestureEvent ge;
    ge.activity = gi.dwID;
    ge.x = ptPoint.x;
    ge.y = ptPoint.y;
    ge.distance = LODWORD(gi.ullArguments);

    // Turn flags into easy bool values
    ge.isBegin = ((gi.dwFlags & GF_BEGIN) == GF_BEGIN);
    ge.isEnd = ((gi.dwFlags & GF_END) == GF_END);
    ge.isInertia = ((gi.dwFlags & GF_INERTIA) == GF_INERTIA);

    //printf("ID: %d\tdwFlags: %d\n", gi.dwID, gi.dwFlags);

    // now interpret the gesture
    switch (gi.dwID) 
    {
        case GID_ZOOM:
            // zoom in and out, from a pinch or spread action
            // ullarguments indicates distance between the two points   
            bHandled = TRUE;
            break;
        case GID_PAN:
            // Code for panning goes here
            // If isInertia, then 
            // HIDWORD(ull) is an inertia vector (two 16-bit values).
            bHandled = TRUE;
            break;

        case GID_ROTATE:
            // Code for rotation goes here
            //printf("ROTATE\n");
            bHandled = TRUE;
            break;
        case GID_TWOFINGERTAP:
            // Code for two-finger tap goes here
            // printf("TWO FINGER TAP\n");
            bHandled = TRUE;
            break;
        case GID_PRESSANDTAP:
            // Code for roll over goes here
            //printf("PRESS AND TAP\n");
            // HIDWORD(ull), is the distance between the two points
            bHandled = TRUE;
            break;
        default:
            // A gesture was not recognized
            printf("GESTURE NOT RECOGNIZED\n");
            break;
        }



    
    ::CloseGestureInfoHandle((HGESTUREINFO)lParam);
    gGestureEventTopic.notify(ge);

    return res;
}

/*
    Subscription routines
*/
// General signal subscription
void subscribe(SignalEventTopic::Subscriber s)
{
    gSignalEventTopic.subscribe(s);
}

// Allow subscription to keyboard events
void subscribe(KeyboardEventTopic::Subscriber s)
{
    gKeyboardEventTopic.subscribe(s);
}

// Allow subscription to mouse events
void subscribe(MouseEventTopic::Subscriber s)
{
    gMouseEventTopic.subscribe(s);
}

// Allow subscription to mouse events
void subscribe(JoystickEventTopic::Subscriber s)
{
    gJoystickEventTopic.subscribe(s);
}

void subscribe(FileDropEventTopic::Subscriber s)
{
    gFileDropEventTopic.subscribe(s);
}

void subscribe(TouchEventTopic::Subscriber s)
{
    gTouchEventTopic.subscribe(s);
}

void subscribe(PointerEventTopic::Subscriber s)
{
    gPointerEventTopic.subscribe(s);
}

void subscribe(GestureEventTopic::Subscriber s)
{
    gGestureEventTopic.subscribe(s);
}



// Controlling the runtime
void halt() {
    ::PostQuitMessage(0);
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
    BOOL bResult = ::RegisterTouchWindow(gAppWindow->getHandle(), 0);
    return (bResult != 0);
}

bool noTouch()
{
    BOOL bResult = ::UnregisterTouchWindow(gAppWindow->getHandle());
    return (bResult != 0);
}

bool isTouch()
{
    ULONG flags = 0;
    BOOL bResult = ::IsTouchWindow(gAppWindow->getHandle(), &flags);
    return (bResult != 0);
}

// Turning drop file support on and off
bool dropFiles()
{
    ::DragAcceptFiles(gAppWindow->getHandle(),TRUE);
    return true;
}

bool noDropFiles()
{
    ::DragAcceptFiles(gAppWindow->getHandle(), FALSE);
    return true;
}

//
// Window management
//
static LONG gLastWindowStyle=0;

void layered()
{
    gAppWindow->addExtendedStyle(WS_EX_LAYERED | WS_EX_NOREDIRECTIONBITMAP);
    gLastWindowStyle = gAppWindow->setWindowStyle(WS_POPUP);

    gIsLayered = true;
}

void noLayered()
{
    gAppWindow->removeExtendedStyle(WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);
    gAppWindow->setWindowStyle(gLastWindowStyle);

    gIsLayered = false;
}

bool isLayered()
{
    return gIsLayered;
}

// Set an opacity value between 0.0 and 1.0
// 1.0 == fully opaque (not transparency)
// less than that makes the whole window more transparent
void windowOpacity(float o)
{
    gAppWindow->setOpacity(o);
}

void setCanvasPosition(int x, int y)
{
    gAppWindow->moveTo(x, y);
}

bool setCanvasSize(long aWidth, long aHeight)
{
    // Create new drawing surface
    if (gAppSurface != nullptr) {
        // Delete old one if it exists
        //delete gAppSurface;
        //gAppSurface.reset();
    }


    gAppSurface = std::make_shared<Surface>(aWidth, aHeight,0);
    canvasWidth = aWidth;
    canvasHeight = aHeight;

    canvasPixels = gAppSurface->getPixels();
    canvasStride = gAppSurface->getStride();

    return true;
}

// Put the application canvas into a window
void createAppWindow(long aWidth, long aHeight, const char* title)
{
    setCanvasSize(aWidth, aHeight);
    gAppWindow->setCanvasSize(aWidth, aHeight);
    gAppWindow->setTitle(title);

    showAppWindow();
}

// A basic Windows event loop
void showAppWindow()
{
    gAppWindow->show();
}

/*
    Generic Windows message handler
    This is used as the function to associate with a window class
    when it is registered.
*/
LRESULT CALLBACK MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT res = 0;

    if ((msg >= WM_MOUSEFIRST) && (msg <= WM_MOUSELAST)) {
        // Handle all mouse messages
        HandleMouseMessage(hWnd, msg, wParam, lParam);
    }
    else if (msg == WM_INPUT) {
        //printf("WM_INPUT\n");
        bool inBackground = GET_RAWINPUT_CODE_WPARAM(wParam) == 1;
        HRAWINPUT inputHandle = (HRAWINPUT)lParam;
        UINT uiCommand = RID_INPUT;
        UINT cbSize=0;

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
                //mouseEvent();
                //printf("RAWMOUSE: %d %d\n", raw->data.mouse.lLastX, raw->data.mouse.lLastY);
            }
                              break;

            case RIM_TYPEKEYBOARD: {
                //keyboardEvent
            }
                                 break;
            }
        }
    }
    else if (msg == WM_DESTROY) {
        // By doing a PostQuitMessage(), a 
        // WM_QUIT message will eventually find its way into the
        // message queue.
        ::PostQuitMessage(0);
        return 0;
    }
    else if ((msg >= WM_KEYFIRST) && (msg <= WM_KEYLAST)) {
        // Handle all keyboard messages
        HandleKeyboardMessage(hWnd, msg, wParam, lParam);
    }
    else if ((msg >= MM_JOY1MOVE) && (msg <= MM_JOY2BUTTONUP)) {
        //printf("MM_JOYxxx: %p\n", gJoystickHandler);
        HandleJoystickMessage(hWnd, msg, wParam, lParam);
    }
    else if (msg == WM_TOUCH) {
        std::cout << "WM_TOUCH" << std::endl;

        // Handle touch specific messages
        HandleTouchMessage(hWnd, msg, wParam, lParam);
    }
    else if (msg == WM_GESTURE) {
    // we will only receive WM_GESTURE if not receiving WM_TOUCH

        HandleGestureMessage(hWnd, msg, wParam, lParam);
    }
    //else if ((msg >= WM_NCPOINTERUPDATE) && (msg <= WM_POINTERROUTEDRELEASED)) {
    //    HandlePointerMessage(hWnd, msg, wParam, lParam);
    //}
    else if (msg == WM_ERASEBKGND) {
        //printf("WM_ERASEBKGND\n");
        if (gPaintHandler != nullptr) {
            gPaintHandler(hWnd, msg, wParam, lParam);
        }

        // return non-zero indicating we dealt with erasing the background
        res = 1;
    }
    else if (msg == WM_PAINT) {
        //printf("WM_PAINT\n");
        if (gPaintHandler != nullptr) {
            // painting is actually handled in ERASEBKGND
            //gPaintHandler(hWnd, msg, wParam, lParam);
        }
        else
        {
            res = ::DefWindowProcA(hWnd, msg, wParam, lParam);
        }
    }
    else if (msg == WM_DROPFILES) {
        HandleFileDropMessage(hWnd, msg, wParam, lParam);
    }
    else {
        res = ::DefWindowProcA(hWnd, msg, wParam, lParam);
    }

    return res;
}

//
// Look for the dynamic routines that will be used
// to setup client applications.
// Most notable is 'onLoad()' and 'onUnload'
//
void registerHandlers()
{
    // we're going to look within our own module
    // to find handler functions.  This is because the user's application should
    // be compiled with the application, so the exported functions should
    // be attainable using 'GetProcAddress()'

    HMODULE hInst = ::GetModuleHandleA(NULL);

    // Start with our default paint message handler
    gPaintHandler = HandlePaintMessage;


    // One of the primary handlers the user can specify is 'onPaint'.  
    // If implemented, this function will be called whenever a WM_PAINT message
    // is seen by the application.
    WinMSGObserver handler = (WinMSGObserver)::GetProcAddress(hInst, "onPaint");
    if (handler != nullptr) {
        gPaintHandler = handler;
    }

    // Get the general app routines
    // onLoad()
    gOnloadHandler = (VOIDROUTINE)::GetProcAddress(hInst, "onLoad");
    gOnUnloadHandler = (VOIDROUTINE)::GetProcAddress(hInst, "onUnload");

    gOnLoopHandler = (VOIDROUTINE)::GetProcAddress(hInst, "onLoop");
}


void run()
{

    // Make sure we have all the event handlers connected
    registerHandlers();

    // call the application's 'onLoad()' if it exists
    if (gOnloadHandler != nullptr) {
        gOnloadHandler();
    }

    // Do a typical Windows message pump
    MSG msg;
    LRESULT res;

    showAppWindow();

    while (true) {
        // we use peekmessage, so we don't stall on a GetMessage
        // should probably throw a wait here
        // WaitForSingleObject
        BOOL bResult = ::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE);
        
        if (bResult > 0) {
            // If we are here, there is some message to be handled
            // If we see a quit message, it's time to stop the program
            if (msg.message == WM_QUIT) {
                break;
            }

            // Do regular Windows message dispatch
            res = ::TranslateMessage(&msg);
            res = ::DispatchMessageA(&msg);
        }
        else {
            // Give the user application some control to do what
            // it wants
            // call onLoop() if it exists
            if (gOnLoopHandler != nullptr) {
                gOnLoopHandler();
            }
        }
    }
    

}


/*
    The 'main()' function is in here to ensure that compiling
    this will result in an executable file.

    The code for the user just needs to implement at least the 'onLoad()' function
*/
// Declare some standard Window Kinds we'll be using
User32WindowClass gAppWindowKind("appwindow", CS_GLOBALCLASS | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, MsgHandler);


// do any initialization that needs to occur 
// in the very beginning
// The most interesting initialization at the moment
// is the networking subsystem
bool prolog()
{
    // Initialize Windows networking
    // BUGBUG - decide whether or not we care about WSAStartup failing
    uint16_t version = MAKEWORD(2,2);
    WSADATA lpWSAData;
    int res = ::WSAStartup(version, &lpWSAData);

    // Typography initialization
    gFontHandler = std::make_shared<FontHandler>();
    
    // Throughout the application, we want to know the true
    // physical dots per inch and screen resolution, so the
    // first thing to do is to let Windows know we are Dpi aware
    // set awareness based on monitor, and get change messages when it changes
    //auto dpiContext = DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
    auto dpiContext = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2;
    
    DPI_AWARENESS_CONTEXT oldContext = ::SetThreadDpiAwarenessContext(dpiContext);
    
    // based on logical inches
    systemDpi = ::GetDpiForSystem();

    // Get the screen size
    auto dpiDpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
    auto dpidisplayWidth = ::GetSystemMetricsForDpi(SM_CXSCREEN, systemDpi);
    auto dpidisplayHeight = ::GetSystemMetricsForDpi(SM_CYSCREEN, systemDpi);

    displayWidth = ::GetSystemMetrics(SM_CXSCREEN);
    displayHeight = ::GetSystemMetrics(SM_CYSCREEN);
    //printf("appmain.prolog, width: %d  height: %d  DPI: %d\n", displayWidth, displayHeight, systemDpi);

    //std::cout << "screen pixels: " << dpidisplayWidth << ", " << dpidisplayHeight << "  dpi: " << dpiDpi << std::endl;

    auto dhdc = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);

    // How big is the screen physically
    // DeviceCaps gives it in millimeters, so we convert to inches
    auto screenWidthInches = ::GetDeviceCaps(dhdc, HORZSIZE)/25.4;
    auto screenHeightInches = ::GetDeviceCaps(dhdc, VERTSIZE)/25.4;

    // pixel count horizontally and vertically
    auto pixelWidth = ::GetDeviceCaps(dhdc, LOGPIXELSX);
    auto pixelHeight = ::GetDeviceCaps(dhdc, LOGPIXELSY);

    // Calculate real pixel density
    double screenHPpi = (double)dpidisplayWidth / screenWidthInches;
    double screenVPpi = (double)dpidisplayHeight / screenHeightInches;
    systemPpi = (unsigned int)screenVPpi;



    // set the canvas a default size to start
    // but don't show it
    setCanvasSize(320, 240);

    gAppWindow = gAppWindowKind.createWindow("Application Window", 320, 240);

    // Get client area
    RECT cRect;
    ::GetClientRect(gAppWindow->getHandle(), &cRect);
    clientLeft = cRect.left;
    clientTop = cRect.top;

    return true;
}

// Do whatever cleanup needs to be done before
// exiting application
void epilog()
{
    if (gOnUnloadHandler != nullptr) {
        gOnUnloadHandler();
    }

    // shut down networking stack
    ::WSACleanup();
}


/*
    Why on Earth are there two 'main' routines in here?
    the first one 'main()' will be called when the code is compiled
    as a CONSOLE subsystem.

    the second one 'WinMain' will be called when the code is compiled
    as a WINDOWS subsystem.  
    
    By having both, we kill two birds with one stone.
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