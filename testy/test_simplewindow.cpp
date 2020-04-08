/*
    compiling

    cl /W4 /nologo test_layeredwindow.cpp
*/

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#pragma comment(lib, "user32.lib")


LRESULT GenericWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (WM_DESTROY == message)
    {
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(window, message, wparam, lparam);
}

int __stdcall wWinMain(HINSTANCE module, HINSTANCE, PWSTR, int)
{
    WNDCLASS wc = {};
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hInstance = module;
    wc.lpszClassName = L"samplewindow";
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = GenericWndProc;

    RegisterClass(&wc);

    //User32WindowClass winclass("samplewindow", CS_HREDRAW | CS_VREDRAW, GenericWndProc);


    HWND const window = CreateWindowEx(WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName, L"Sample",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, module, nullptr);

    MSG message;
    while (BOOL result = GetMessage(&message, 0,0,0))
    {
        if (-1 != result) DispatchMessage(&message);
    }
}
