#pragma once

// DCSnapper
//
// Take a snapshot of a device context and hold
// it in a PixelMap (User32PixelMap)
// 
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
//    https://github.com/bmharper/WindowsDesktopDuplicationSample
//


#include "Surface.h"
#include "texture.h"
#include "User32PixelMap.h"

class DCSnapper : public User32PixelMap
{
    HDC fSourceDC;  // Device Context for the screen

    // which location on the screen are we capturing
    int fOriginX;
    int fOriginY;


public:
    DCSnapper(HDC src, int x, int y, int awidth, int aheight)
        : User32PixelMap(awidth, aheight),
        fOriginX(x),
        fOriginY(y)
    {
        // create a device context for the display
        fSourceDC = src;

        // take at least one snapshot
        next();
    }

    // take a snapshot of current screen
    bool next()
    {
        ::BitBlt(getDC(), 0, 0, width(), height(), fSourceDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }
};