#pragma once

/*
    Data types for ui events
*/
#include <vector>
#include <string>

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

// Keyboard interactions
enum {
    KEYPRESSED,
    KEYRELEASED,
    KEYTYPED
};

struct KeyboardEvent {
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

// Pointer interactions
struct PointerEvent {
    int id;
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


#ifdef __cplusplus
extern "C" {
#endif

    // Some generic function signatures
    typedef void (*KeyEventHandler)(const KeyboardEvent& e);
    typedef void (*MouseEventHandler)(const MouseEvent& e);
    typedef void (*JoystickEventHandler)(const JoystickEvent& e);
    typedef void (*TouchEventHandler)(const TouchEvent& e);
    typedef void (*PointerEventHandler)(const PointerEvent& e);
    typedef void (*FileDropEventHandler)(const FileDropEvent& e);

#ifdef __cplusplus
}
#endif