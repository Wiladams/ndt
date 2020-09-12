#pragma once

/*
    Take a snapshot of the video display using GDI.

    Construct an instance of the object by determining
    the location and size of the capture.  It is a fixed
    size and location, so can not be moved.  If you want
    to capture a different location, create another capture
    object.

    ScreenSnapshot ss(x,y, width, height);

    getting an actual snapshot, call moveNext()

    ss.moveNext()

    And finally to get a pixelbuffer that was captured

    ss.getCurrent()
*/

#include "Surface.h"


class ScreenSnapshot
{

    HDC fScreenDC;
    int fWidth;
    int fHeight;

public:
    Surface fImage;
    int fOriginX;
    int fOriginY;


public:
    ScreenSnapshot(int x, int y, int awidth, int aheight, uint32_t threadCount=0)
        : fImage(awidth, aheight, threadCount),
        fOriginX(x),
        fOriginY(y),
        fWidth(awidth),
        fHeight(aheight)
    {
        // create a pixel buffer of the specified size
        fScreenDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
    }

    void reset() {
        // do nothing
    }

    size_t width() { return fWidth; }
    size_t height() { return fHeight; }

    Surface & getCurrent()
    {
        return fImage;
    }

    bool moveNext()
    {
        BitBlt(fImage.getDC(), fOriginX, fOriginY, fImage.getWidth(), fImage.getHeight(),
            fScreenDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }

};