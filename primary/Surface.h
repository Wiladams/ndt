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

#include "BLGraphics.h"
//#include "pixelarray.h"
#include "pixelaccessor.h"

#include <cstdio>



class Surface : public BLGraphics
{
    // For interacting with blend2d
    BLImage fImage{};
    BLImageData fImageInfo{};

public:
    Surface() = default;

    void attachPixelArray(PixelArray& pixmap, uint32_t threadCount = 0)
    {
        // Reset the BLImage so we can initialize it anew
        fImage.reset();

        // Initialize the BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        blImageInitAsFromData(&fImage, pixmap.width(), pixmap.height(), BL_FORMAT_PRGB32, pixmap.data(), (intptr_t)pixmap.stride(), nullptr, nullptr);
    
        fImage.getData(&fImageInfo);

        // BUGBUG - with multi-thread, flush/sync doesn't quite seem
        // to be in sync, causing tearing.
        // Initialize the BLContext
        BLContextCreateInfo createInfo{};
        createInfo.commandQueueLimit = 255;
        createInfo.threadCount = threadCount;

        fCtx.begin(fImage, createInfo);
    }


    virtual ~Surface()
    {
        fImage.reset();
    }

    long getWidth() { return fImageInfo.size.w; }
    long getHeight() { return fImageInfo.size.h; }
    ptrdiff_t getStride() { return ptrdiff_t(fImageInfo.stride);}
    Pixel * getPixels() { return (Pixel *)fImageInfo.pixelData; }

    // Calculate whether a point is whithin our bounds
    bool contains(double x, double y) 
    { 
        return (
            (x >= 0) && (x < fImage.width()) &&
            (y >= 0) && (y < fImage.height())
                ); 
    }

    //BITMAPINFO getBitmapInfo() {return fPixelMap.bitmapInfo(); }

    //HDC getDC() {return fPixelMap.bitmapDC(); }

    BLImage& getImage() {return fImage;}

    inline int pixelOffset(int x, int y)
    {
        return (y * fImage.width()) + x;
    }

    Pixel get(int x, int y)
    {
        x = (int)maths::clamp(x, 0, fImage.width() - 1);
        y = (int)maths::clamp(y, 0, fImage.height() - 1);

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
