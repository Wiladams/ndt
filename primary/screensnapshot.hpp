#pragma once

/*
    Take a snapshot of the video display using GDI.

    Construct an instance of the object by determining
    the location and size of the capture.  It is a fixed
    size and location, so can not be moved.  If you want
    to capture a different location, create another capture
    object.

    ScreenSnapshot ss(x,y, width, height);

    getting an actual snapshot, call next()

    ss.next()

    And finally to get a pixelbuffer that was captured

    ss.getCurrent()
    ss.getImage()

    References:
    https://www.codeproject.com/articles/5051/various-methods-for-capturing-the-screen
*/

#include "Surface.h"
#include "texture.h"

class ScreenSnapshot : public Texture
{
    HDC fScreenDC;
    Surface fSurface;

    int fOriginX;
    int fOriginY;
    int fWidth;
    int fHeight;

public:
    ScreenSnapshot(int x, int y, int awidth, int aheight, uint32_t threadCount=0)
        : fSurface(awidth, aheight, threadCount),
        fOriginX(x),
        fOriginY(y),
        fWidth(awidth),
        fHeight(aheight)
    {
        // create a device context for the display
        fScreenDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
    }

    void reset() {
        // do nothing
    }

    size_t width() { return fWidth; }
    size_t height() { return fHeight; }

    virtual rtcolor value(const double u, const double v, const vec3& p) const
    {
        auto pval = pixelValue(u,v,p);
        return { (double)pval.r / 255, (double)pval.g / 255, (double)pval.b / 255 };
    }

    virtual BLRgba32 pixelValue(double u, double v, const vec3& p) const
    {
        int x = int(u * (fWidth - 1));
        int y = int(v * (fHeight - 1));

        return fSurface.get(x, y);
    }

    Surface & getCurrent()
    {
        return fSurface;
    }

    // return image directly
    BLImage& getImage()
    {
        return fSurface.getImage();
    }

    // take a snapshot
    bool next()
    {
        BitBlt(fSurface.getDC(), 0, 0, fSurface.getWidth(), fSurface.getHeight(),
            fScreenDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }

};