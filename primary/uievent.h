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
//    TOUCH_HOVER,
};

struct TouchEvent {
    int id;
    int activity;

    bool isMoving;
    bool isUp;
    bool isDown;
    bool isPrimary;
    bool isHovering;
    bool isPalm;
    bool isPen;

    long rawX;
    long rawY;
    unsigned long rawWidth;
    unsigned long rawHeight;

    long x;
    long y;
    size_t w;
    size_t h;
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
