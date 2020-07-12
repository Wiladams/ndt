#pragma once

/*
    Data types for ui events
*/
#include <vector>
#include <string>

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

struct FileDropEvent {
    int x;
    int y;
    std::vector<std::string> filenames;
};


#ifdef __cplusplus
extern "C" {
#endif

    // Some generic function signatures
    typedef void (*KeyEventHandler)(const KeyEvent& e);
    typedef void (*MouseEventHandler)(const MouseEvent& e);
    typedef void (*JoystickEventHandler)(const JoystickEvent& e);
    typedef void (*TouchEventHandler)(const TouchEvent& e);
    typedef void (*PointerEventHandler)(const PointerEvent& e);
    typedef void (*FileDropEventHandler)(const FileDropEvent& e);

#ifdef __cplusplus
}
#endif