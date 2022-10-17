#ifndef Surface_h
#define Surface_h

//#pragma once

/*
    The mating between a DIBSection and a BLImage Graphics and BLContext

    This one object creates a UserPixelMap to be the 
    backing storage for graphics.

    It then mates this with a BLContext so that we can
    do drawing with both GDI as well as blend2d.

    The object inherits from GLBraphics, so you can do drawing
    on the object directly.
*/

#include "User32PixelMap.h"
#include "BLGraphics.h"

#include <cstdio>

class Surface : public BLGraphics
{
    // for interacting with win32
    User32PixelMap fPixelMap;

    // For interacting with blend2d
    BLImage fImage;

public:
    Surface(long awidth, long aheight, uint32_t threadCount=0)
    {
        fPixelMap.init(awidth, aheight);

        // Initialize the BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        BLResult bResult = blImageInitAsFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, fPixelMap.data(), (intptr_t)fPixelMap.stride(), nullptr, nullptr);

        // BUGBUG - with multi-thread, flush/sync doesn't quite seem
        // to be in sync, causing tearing.
        // Initialize the BLContext
        BLContextCreateInfo createInfo{};
        createInfo.commandQueueLimit = 255;
        createInfo.threadCount = threadCount;


        bResult = fCtx.begin(fImage, createInfo);

    }

    virtual ~Surface()
    {
        fImage.reset();
    }

    long getWidth() { return fPixelMap.width(); }
    long getHeight() { return fPixelMap.height(); }
    long getStride() { return fPixelMap.stride(); }
    Pixel * getPixels() { return (Pixel *)fPixelMap.data(); }

    // Calculate whether a point is whithin our bounds
    bool contains(double x, double y) 
    { 
        return (
            (x >= 0) && (x < fPixelMap.width()) && 
            (y >= 0) && (y < fPixelMap.height())
                ); 
    }

    BITMAPINFO getBitmapInfo() {return fPixelMap.bitmapInfo(); }

    HDC getDC() {return fPixelMap.bitmapDC(); }

    BLImage& getImage() {return fImage;}

    inline int pixelOffset(int x, int y)
    {
        return (y * fPixelMap.width()) + x;
    }

    Pixel get(int x, int y)
    {
        x = (int)maths::clamp(x, 0, fPixelMap.width() - 1);
        y = (int)maths::clamp(y, 0, fPixelMap.height() - 1);

        // Get data from BLContext
        int offset = pixelOffset(x, y);
        return ((Pixel *)getPixels())[offset];
    }
    
    void set(double x, double y, const Pixel& c) override
    {
        int offset = pixelOffset(x, y);

        ((Pixel*)getPixels())[offset] = c;
    }
 };


#endif  // Surface_h
