/*
    compiling

    cl /W4 /nologo test_layeredwindow.cpp
*/

#include "NativeWindow.hpp"
#include "LayeredWindow.hpp"
#include "DrawingContext.hpp"
#include "PBDIBSection.hpp"

#define VK_ESCAPE         0x1B

LRESULT GenericWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (WM_DESTROY == message)
    {
        PostQuitMessage(0);
        return 0;
    }

    if (WM_KEYUP == message)
    {
        if ((int)wparam == VK_ESCAPE)
        {
            PostQuitMessage(0);
        }
    }

    return DefWindowProcA(window, message, wparam, lparam);
}

int main(int argc, char **argv)
{
    // Window class with a generic WndProc
    User32WindowClass winclass("samplewindow", CS_HREDRAW | CS_VREDRAW, GenericWndProc);
    
    // Instance of a window with no client area
    // backing buffer
    User32Window * win = winclass.createWindow("Layer", 800, 600, WS_OVERLAPPEDWINDOW | WS_VISIBLE, WS_EX_LAYERED|WS_EX_NOREDIRECTIONBITMAP);

    // Construct a thing to do drawing
    PBDIBSection pb(800,600);
    DrawingContext dc(pb);
    LayeredWindowInfo lw(800, 600);

    // red border around whole window
    dc.setStroke(PixRGBA(0xffff0000));
    dc.strokeRectangle(0,0,800,600);

    // fill with default color
    dc.fillRectangle(10,10, 200, 200);

    // Standard message pump
    MSG message;
    while (BOOL result = GetMessageA(&message, 0,0,0))
    {
        if (-1 != result) {
            DispatchMessageA(&message);
        }

        //printf("DISPLAY\n");
        if (!lw.display(win->fHandle, pb.getDC()))
        {
            printf("lw.display, ERROR: %d\n", lw.getLastError());
        }
    }
}
