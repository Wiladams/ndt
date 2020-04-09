#pragma once

/*
    AlphaWindow

    Create windows that honor the alpha channel of an 
    image as it displays.

https://docs.microsoft.com/en-us/archive/msdn-magazine/2009/december/windows-with-c-layered-windows-with-direct2d
https://docs.microsoft.com/en-us/archive/msdn-magazine/2014/june/windows-with-c-high-performance-window-layering-using-the-windows-composition-engine
*/

#include "w32.hpp"
#include "NativeWindow.hpp"

/*
    DWORD cbSize;
    HDC hdcDst;
    const POINT* pptDst;
    const SIZE* psize;
    HDC hdcSrc;
    const POINT* pptSrc;
    COLORREF crKey;
    const BLENDFUNCTION* pblend;
    DWORD dwFlags;
    const RECT* prcDirty;
*/
class LayeredWindowInfo
{
public:
    POINT fSourcePosition;
    POINT fWindowPosition;
    SIZE fSize;
    BLENDFUNCTION   fBlendFunction;
    UPDATELAYEREDWINDOWINFO fInfo;
    int fLastError;


    LayeredWindowInfo(int width, int height)
        :fSourcePosition(),
        fWindowPosition(),
        fSize({width, height}),
        fBlendFunction(),
        fInfo(),
        fLastError(0)
    {
        // When SourceConstantAlpha == 255
        // the layered window will use per pixel
        // alpha when compositing
        // AC_SRC_ALPHA indicates the source bitmap has
        // an alpha channel
        //fBlendFunction.BlendOp = AC_SRC_OVER;
        //fBlendFunction.BlendFlags = 0;
        fBlendFunction.SourceConstantAlpha = 255;
        fBlendFunction.AlphaFormat = AC_SRC_ALPHA;
    
        fInfo.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
        fInfo.pptSrc = &fSourcePosition;
        fInfo.pptDst = &fWindowPosition;
        fInfo.psize = &fSize;
        fInfo.pblend = &fBlendFunction;
        fInfo.dwFlags = ULW_ALPHA;

        //fInfo.hdcSrc = fSourceDC;  // DC of dibsection

    }

    int getLastError() {return fLastError;}
    int getWidth() {return fSize.cx;}
    int getHeight() {return fSize.cy;}


    bool display(HWND win, HDC source)
    {
        fInfo.hdcSrc = source;
        RECT wRect;
        BOOL bResult = GetWindowRect(win, &wRect);
        
        fWindowPosition.x = wRect.left;
        fWindowPosition.y = wRect.top;
        
        bResult = UpdateLayeredWindowIndirect(win, &fInfo);

        if (!bResult) {
            fLastError = GetLastError();
            return false;
        }

        return true;
    }



};

/*
class AlphaWindow {
    static User32WindowClass AlphaWindowKind("alphawindow", CS_GLOBALCLASS | CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW, MsgHandler);


public:
    ATOM fwinclass;
    HWND fHandle;
    //PixelBuffer * fBackground;
    WINDOWINFO fInfo;
    int fScreenX;
    int fScreenY;
    int fSystemDpi;
    
    // default to default desktop

    AlphaWindow()
    {
        // First thing to do is let the system know we are
        // going to be DPI aware
        DPI_AWARENESS_CONTEXT oldContext = ::SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // Get the DPI for the monitor
        fSystemDpi = ::GetDpiForSystem();

        // Get the screen size
        int fScreenX = ::GetSystemMetrics(SM_CXSCREEN);
        int fScreenY = ::GetSystemMetrics(SM_CYSCREEN);

        // create a dib section to match the size


    }

    void display()
    {
        UpdateLayeredWindowIndirect(getHandle(), &fLayeredWindowInfo);
    }
};
*/