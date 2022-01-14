#pragma once

// ScreenSnapshot
//
// Take a snapshot of a portion of the screen and hold
// it in a PixelMap (User32PixelMap)
// A sampler2D interface is also provided so you can 
// either use the pixel oriented 'get()' function, or the 
// parametric 'getValue()' function.
//
// When constructed, a single snapshot is taken.
// every time you want a new snapshot, just call 'next()'
// This is great for doing a live screen capture
//
//    ScreenSnapshot ss(x,y, width, height);
//
//    References:
//    https://www.codeproject.com/articles/5051/various-methods-for-capturing-the-screen
//    https://stackoverflow.com/questions/5069104/fastest-method-of-screen-capturing-on-windows
//  https://github.com/bmharper/WindowsDesktopDuplicationSample
//

#include "User32PixelMap.h"
#include "sampler.h"

class ScreenSnapper : public User32PixelMap
{
    HDC fScreenDC;  // Device Context for the screen

    // which location on the screen are we capturing
    int fOriginX;
    int fOriginY;


public:
    ScreenSnapper(int x, int y, int awidth, int aheight)
        : User32PixelMap(awidth, aheight),
        fOriginX(x),
        fOriginY(y)
    {
        // create a device context for the display
        //fScreenDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
        fScreenDC = GetDC(nullptr);

        // take at least one snapshot
        next();
    }

    // take a snapshot of current screen
    bool next()
    {
        ::BitBlt(getDC(), 0, 0, width(), height(), fScreenDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }
};