#include "NativeWindow.hpp"

#include <cstdio>

/*
    References

    https://stackoverflow.com/questions/9238552/how-does-the-message-queue-work-in-win32

*/

class MSGLooper {
    // Window Class for our message looper object
    static User32WindowClass gMSGLoopKind("msglooper", CS_GLOBALCLASS, MSGLooper::MsgHandler);

    // Routine to handle messages
    static LRESULT MsgHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT res = 0;
        
        res = DefWindowProcA(hWnd, msg, wParam, lParam);

        return res;
    }

public:
    MSGLooper()
    {
        MSGLooper::gMSGLoopKind.createWindow("Application Window", 320, 240, WS_OVERLAPPEDWINDOW);
    }
}


void main()
{

}