#pragma once

/*
    Data types for ui events
*/

#include "appconfig.h"
#include "pubsub.h"

#include <vector>
//#include <string>
//#include <functional>

// Basic type to encapsulate a mouse event
enum {
    // These are based on regular events
    MOUSEMOVED,
    MOUSEPRESSED,
    MOUSERELEASED,
    MOUSEWHEEL,

    // These are based on application semantics
    MOUSECLICKED,
    MOUSEDRAGGED,

    MOUSEENTERED,
    MOUSEHOVER,         // like move, when we don't have focus
    MOUSELEFT           // exited boundary
};

struct MouseEvent {
    int id;
    int activity;
    int x;
    int y;
    int delta;

    // derived attributed
    bool control;
    bool shift;
    bool lbutton;
    bool rbutton;
    bool mbutton;
    bool xbutton1;
    bool xbutton2;
};

// Keyboard interactions
enum {
    KEYPRESSED,
    KEYRELEASED,
    KEYTYPED
};

struct KeyboardEvent {
    int id;
    int activity;
    int keyCode;        // wparam
    int repeatCount;    // 0 - 15
    int scanCode;       // 16 - 23
    bool isExtended;    // 24
    bool wasDown;       // 30
};

// Touch interactions
enum {
    TOUCH_DOWN,
    TOUCH_UP,
    TOUCH_MOVE,
    TOUCH_HOVER,
};

struct TouchEvent {
    int id;
    int activity;

    bool isPrimary;
    bool isHovering;
    bool isPalm;
    bool isPen;

    int rawX;
    int rawY;
    int rawWidth;
    int rawHeight;

    int x;
    int y;
    int w;
    int h;
};

// Pointer interactions
struct PointerEvent {
    int id;
    int activity;
    int x;
    int y;
};

// File drop interactions
struct FileDropEvent {
    int x;
    int y;
    std::vector<std::string> filenames;
};

// For joystick activity
enum {
    JOYPRESSED,
    JOYRELEASED,
    JOYMOVED,
    JOYZMOVED
};

struct JoystickEvent {
    unsigned int ID;
    int activity;			// What kind of joystick action is it
    int buttons;			// Bitfield of buttons being pressed
    int numButtonsPressed;	// total number of buttons currently pressed
    int flags;
    int POV;				// Value of hat switch

    // Axes values, return -1.0..1.0
    double x;
    double y;
    double z;
    double r;
    double u;
    double v;
};

// Doing C++ pub/sub
using MouseEventTopic = Topic<MouseEvent &>;
using KeyboardEventTopic = Topic<KeyboardEvent &>;
using JoystickEventTopic = Topic<JoystickEvent &>;
using FileDropEventTopic = Topic<FileDropEvent&>;
using TouchEventTopic = Topic<TouchEvent&>;
using PointerEventTopic = Topic<PointerEvent&>;

#ifdef __cplusplus
extern "C" {
#endif

    //using KeyboardEventHandler = std::function<void(const KeyboardEvent& e)>;

    // Some generic function signatures
    typedef void (*KeyEventHandler)(const KeyboardEvent& e);
    typedef void (*MouseEventHandler)(const MouseEvent& e);
    typedef void (*JoystickEventHandler)(const JoystickEvent& e);
    typedef void (*TouchEventHandler)(const TouchEvent& e);
    typedef void (*PointerEventHandler)(const PointerEvent& e);
    typedef void (*FileDropEventHandler)(const FileDropEvent& e);

    APP_EXPORT void handleKeyboardEvent(const KeyboardEvent& e);
    APP_EXPORT void handleMouseEvent(const MouseEvent& e);
    APP_EXPORT void handleJoystickEvent(const JoystickEvent& e);
    APP_EXPORT void handleTouchEvent(const TouchEvent& e);
    APP_EXPORT void handlePointerEvent(const PointerEvent& e);

#ifdef __cplusplus
}
#endif