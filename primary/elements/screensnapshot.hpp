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

/*
*   To use it as Texture
    virtual rtcolor value(const double u, const double v, const vec3& p) const
    {
        auto pval = pixelValue(u,v,p);
        return { (double)pval.r() / 255, (double)pval.g() / 255, (double)pval.b() / 255};
    }

    virtual BLRgba32 pixelValue(double u, double v, const vec3& p) const
    {
        int x = int(u * (fWidth - 1));
        int y = int(v * (fHeight - 1));

        return fSurface.get(x, y);
    }
*/

#include "Surface.h"
#include "User32PixelMap.h"

bool BLImageFromPixelArray(PixelAccessor<Pixel> &arr, BLImage &img)
{
    BLResult bResult = blImageInitAsFromData(&img, arr.width(), arr.height(), BL_FORMAT_PRGB32, arr.data(), (intptr_t)arr.stride(), nullptr, nullptr);

    return bResult == 0;
}

class ScreenSnapper : public User32PixelMap
{
    HDC fSourceDC=nullptr;
    int fOriginX = 0;
    int fOriginY = 0;
    BLImage fImage{};

public:
    ScreenSnapper()
    {
    }

    void reset(int x, int y, int w, int h, HDC srcDC=nullptr)
    {
        if (srcDC == nullptr)
            fSourceDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
        else
            fSourceDC = srcDC;
        
        fOriginX = x;
        fOriginY = y;
        init(w, h);
        BLImageFromPixelArray(*this, fImage);
    }

    BLImage& getImage()
    {
        return fImage;
    }

    // take a snapshot
    bool next()
    {
        auto bResult = BitBlt(bitmapDC(), 0, 0, width(), height(),
            fSourceDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }
};

/*
class ScreenSnapshot
{
    HDC fSourceDC;
    User32PixelMap fPixelMap{};
    BLImage fImage{};

    int fOriginX=0;
    int fOriginY=0;
    size_t fWidth=0;
    size_t fHeight=0;

public:
    ScreenSnapshot(int x, int y, size_t awidth, size_t aheight)
        : fOriginX(x)
        ,fOriginY(y)
        ,fWidth(awidth)
        ,fHeight(aheight)
    {
        fPixelMap.init(awidth, aheight);
        BLImageFromPixelArray(fPixelMap, fImage);

        // create a device context for the display
        fSourceDC = CreateDCA("DISPLAY", nullptr, nullptr, nullptr);
    }

    void reset() {
        // do nothing
    }

    size_t width() { return fWidth; }
    size_t height() { return fHeight; }

    // return image directly
    BLImage& getImage()
    {
        return fImage;
    }

    // take a snapshot
    bool next()
    {
        auto bResult = BitBlt(fPixelMap.bitmapDC(), 0, 0, fPixelMap.width(), fPixelMap.height(),
            fSourceDC, fOriginX, fOriginY, SRCCOPY | CAPTUREBLT);

        return true;
    }

};
*/