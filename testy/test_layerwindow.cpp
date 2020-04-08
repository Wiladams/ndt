/*
    compiling

    cl /W4 /nologo test_layeredwindow.cpp
*/

#include "NativeWindow.hpp"

LRESULT GenericWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (WM_DESTROY == message)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcA(window, message, wparam, lparam);
}

int __stdcall wWinMain(HINSTANCE module, HINSTANCE, wchar_t *, int)
{
    //wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    // Window class with a generic WndProc
    User32WindowClass winclass("samplewindow", CS_HREDRAW | CS_VREDRAW, GenericWndProc);
    
    // Instance of a window with no client area
    // backing buffer
    User32Window * win = winclass.createWindow("Layer", 800, 600, WS_OVERLAPPEDWINDOW | WS_VISIBLE, WS_EX_NOREDIRECTIONBITMAP);

    // Standard message pump
    MSG message;
    while (BOOL result = GetMessageA(&message, 0,0,0))
    {
        if (-1 != result) DispatchMessageA(&message);
    }
}
