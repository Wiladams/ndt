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

#include "PBDIBSection.hpp"


class ScreenSnapshot //: public IEnumerator<PixelBufferRGBA32>
{
    PBDIBSection fImage;
    HDC fScreenDC;
    int fOriginX;
    int fOriginY;

public:
    ScreenSnapshot(int x, int y, GRSIZE awidth, GRSIZE aheight)
        : fImage(awidth, aheight),
        fOriginX(x),
        fOriginY(y)
    {
        // create a pixel buffer of the specified size
        fScreenDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
    }

    void reset() {
        // do nothing
    }

    PBDIBSection & getCurrent()
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