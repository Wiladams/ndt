#pragma once

//#include "NativeWindow.hpp"
//#include "Surface.h"
//#include "joystick.h"
//#include "uievent.h"

//APP_EXPORT extern User32Window* gAppWindow;
//APP_EXPORT extern std::shared_ptr<Surface> gAppSurface;


//EXPORT extern bool gRunning;


// Globals we expect the user to consume
//APP_EXPORT extern int displayWidth;
//APP_EXPORT extern int displayHeight;
//APP_EXPORT extern unsigned int systemDpi;
//APP_EXPORT extern unsigned int systemPpi;

//APP_EXPORT extern int canvasWidth;
//APP_EXPORT extern int canvasHeight;

//APP_EXPORT void showAppWindow();

//EXPORT void forceRedraw(void* param, int64_t tickCount);
//APP_EXPORT void screenRefresh();

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