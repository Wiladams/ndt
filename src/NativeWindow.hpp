#pragma once
#pragma comment(lib, "user32.lib")

/*
    This file, and the Window class represent the connection to the Win32 
    User32 interface library.  The idea is, if you include this single header
    in your application .cpp file, you have everything you need to create 
    a window of a given size, do drawing, keyboard and mouse handling.

    No need for any Windows specific headers or any other libraries.  This
    single file (and it pulls in w32.hpp) contains everything you need.
*/

#include <stdio.h>

#include "w32.hpp"
#include "PixelBuffer.hpp"

#include <cstring>


class User32Window {
public:

    HWND fHandle;
    //PixelBuffer * fBackground;
    //WINDOWINFO fInfo;

    // Constructor taking an already allocated
    // window handle.  Use a WindowKind to allocate
    // an instance of a particular kind of window
    User32Window(HWND handle)
    {
        fHandle = handle;
    }

    // Virtual destructor so the window can be sub-classed
    virtual ~User32Window()
    {
        DestroyWindow(fHandle);
    }

    HWND getHandle() {return fHandle;}
    
    // All the methods that are useful
    bool isValid() {return fHandle != NULL;}

    // Hide the window
    void hide()
    {
        ::ShowWindow(fHandle, SW_HIDE);
    }

    // Show the window
    void show()
    {
        ::ShowWindow(fHandle, SW_SHOWNORMAL);
    }

    int getWidth() 
    {
        RECT wRect;
        BOOL bResult = GetWindowRect(fHandle, &wRect);
        int cx = wRect.right-wRect.left;
        return cx;
    }

    int getHeight() 
    {
        RECT wRect;
        BOOL bResult = GetWindowRect(fHandle, &wRect);
        int cy = wRect.bottom-wRect.top;
        
        return cy;
    }

    void moveTo(int x, int y)
    {
        RECT wRect;
        BOOL bResult = GetWindowRect(fHandle, &wRect);
        int cx = wRect.right - wRect.left;
        int cy = wRect.bottom - wRect.top;
        int flags = SWP_NOOWNERZORDER | SWP_NOSIZE;

        bResult = ::SetWindowPos(fHandle, (HWND)0, x, y, 0, 0, flags);
    }

    void setCanvasSize(size_t awidth, size_t aheight)
    {
        // Get current size of window
        RECT wRect;
        BOOL bResult = GetWindowRect(fHandle, &wRect);

        // Set the new size of the window based on the client area
        RECT cRect = {0,0,(LONG)awidth,(LONG)aheight};
        bResult = AdjustWindowRect(&cRect, WS_OVERLAPPEDWINDOW, 1);

        //HWND hWndInsertAfter = (HWND)-1;
        HWND hWndInsertAfter = NULL;
        int X = wRect.left;
        int Y = wRect.top;
        int cx = cRect.right-cRect.left;
        int cy = cRect.bottom-cRect.top;
        UINT uFlags = 0;

        bResult = SetWindowPos(getHandle(), hWndInsertAfter,X,Y,cx,cy, uFlags);

    }

    // Set a specific extended window style
    LONG setExtendedStyle(int xstyle)
    {
        return SetWindowLongA(fHandle, GWL_EXSTYLE, GetWindowLongA(fHandle, GWL_EXSTYLE) | xstyle);
    }

    // clear a specific extended window style
    LONG clearExtendedStyle(int xstyle)
    {
        return SetWindowLongA(fHandle, GWL_EXSTYLE, (~(LONG)xstyle)&GetWindowLongA(fHandle, GWL_EXSTYLE));
    }

};


// In Win32, a window 'class' must be registered
// before you can use the CreateWindow call.  This
// WindowClass object makes it easier to handle these
// Window Classes, and do that registration.  As well,
// it makes it relatively easy to create various instances
// of classes.
class User32WindowClass {
    WNDCLASSEXA fWndClass;      // data structure holding class information
    bool fIsRegistered;
    int fLastError;

    char * fClassName;       // this is only here to guarantee string sticks around
    uint16_t    fClassAtom;

public:
    User32WindowClass(const char *classOrAtom)
        : fLastError(0)
        , fClassAtom(0)
        , fIsRegistered(false)
    {
        // In this case, we're essentially doing a lookup
        // The classOrAtom is either a pointer to a string
        // or it's an unsigned uint16
        memset(&fWndClass, 0, sizeof(WNDCLASSEXA));

        BOOL bResult = GetClassInfoExA(GetModuleHandleA(NULL), classOrAtom, &fWndClass);
        if (bResult == 0) {
            fLastError = GetLastError();
            return;
        }

        // if we got to here, the lookup worked
        // We know the window class is registered
        // We won't bother trying to get the atom as
        // it is of no practical use
        fIsRegistered = true;
    }

    User32WindowClass(char *className, unsigned int classStyle, WNDPROC wndProc = nullptr)
        :fLastError(0),
        fClassAtom(0)
    {
        if (className == nullptr) {
            return;
        }
        fClassName = strdup(className);

        memset(&fWndClass, 0, sizeof(fWndClass));
        fWndClass.cbSize = sizeof(WNDCLASSEXA);
        fWndClass.hInstance = GetModuleHandleA(NULL);
        fWndClass.lpszClassName = fClassName; 
        fWndClass.lpfnWndProc = wndProc;
        fWndClass.style = classStyle;

        // The user might want to change any of these as they are not passed
        // into the constructor.
        fWndClass.hbrBackground = nullptr;
        fWndClass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
        fWndClass.lpszMenuName = nullptr;
        fWndClass.hIcon = nullptr;          // LoadIcon(nullptr, IDI_APPLICATION);
        fWndClass.hIconSm = nullptr;

        // Try to register the window class
        fClassAtom = ::RegisterClassExA(&fWndClass);

        if (fClassAtom == 0) {
            fLastError = GetLastError();
            return ;
        }

        fIsRegistered = true;
    }

    bool isValid() const {return fIsRegistered;}

    int getLastError() const {return fLastError;}
    const char * getName() const {return fWndClass.lpszClassName;}

    User32Window * createWindow(const char *title, int width, int height, int style=WS_OVERLAPPEDWINDOW, int xstyle=0, WNDPROC handler = nullptr)
    {
        if (!isValid()) {
            return nullptr;
        }

        HMODULE hInst = fWndClass.hInstance;
        
        // Create the window handle
        int winxstyle = xstyle;
        int winstyle = style;

        //HMODULE hInst = GetModuleHandleA(NULL);
	    HWND winHandle = CreateWindowExA(
		    winxstyle,
		    fWndClass.lpszClassName,
		    title,
		    winstyle,
            CW_USEDEFAULT, CW_USEDEFAULT,
            width, height,
		    NULL,
		    NULL,
		    fWndClass.hInstance,
		    NULL);

        if (winHandle == nullptr) {
            return nullptr;
        }

        User32Window * win = new User32Window(winHandle);

        return win;
    }
};
