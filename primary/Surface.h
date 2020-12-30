#pragma once

/*
    The mating between a DIBSection and a BLImage Graphics and BLContext
*/

#include <windows.h>

#include "BLGraphics.h"
#include <cstdio>



class Surface : public BLGraphics
{
    // for interacting with win32
    BITMAPINFO fBMInfo{0};
    HBITMAP fDIBHandle = nullptr;
    HDC     fBitmapDC = nullptr;
    void * fData = nullptr;       // A pointer to the data
    size_t fDataSize=0;       // How much data is allocated
    long fWidth=0;
    long fHeight=0;

    // For interacting with blend2d
    BLImage fImage;


public:
    Surface(long awidth, long aheight, uint32_t threadCount=0)
        : fWidth(awidth),
        fHeight(aheight)
    {
        int bitsPerPixel = 32;
        int alignment = 4;
        int bytesPerRow = ndt::GetAlignedByteCount(awidth, bitsPerPixel, alignment);

        fBMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        fBMInfo.bmiHeader.biWidth = awidth;
        fBMInfo.bmiHeader.biHeight = -(LONG)aheight;	// top-down DIB Section
        fBMInfo.bmiHeader.biPlanes = 1;
        fBMInfo.bmiHeader.biBitCount = bitsPerPixel;
        fBMInfo.bmiHeader.biSizeImage = bytesPerRow * aheight;
        fBMInfo.bmiHeader.biClrImportant = 0;
        fBMInfo.bmiHeader.biClrUsed = 0;
        fBMInfo.bmiHeader.biCompression = BI_RGB;
        fDataSize = fBMInfo.bmiHeader.biSizeImage;

        // We'll create a DIBSection so we have an actual backing
        // storage for the context to draw into
        // BUGBUG - check for nullptr and fail if found
        fDIBHandle = CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS, &fData, nullptr, 0);


        // Create a GDI Device Context
        fBitmapDC = CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform operations with it
        ::SelectObject(fBitmapDC, fDIBHandle);

        // Initialize the BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        //BLResult bResult = blImageCreateFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, fData, bytesPerRow, nullptr, nullptr);
        BLResult bResult = blImageInitAsFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, fData, (intptr_t)bytesPerRow, nullptr, nullptr);

        // BUGBUG - with multi-thread, flush/sync doesn't quite seem
        // to be in sync, causing tearing.
        // Initialize the BLContext
        BLContextCreateInfo createInfo{};
        createInfo.commandQueueLimit = 255;
        createInfo.threadCount = threadCount;
        bResult = getBlend2dContext().begin(fImage, createInfo);

        // Start with a fill of white, and stroke of black
        getBlend2dContext().setFillStyle(BLRgba32(0xffffffff));
        getBlend2dContext().setStrokeStyle(BLRgba32(0xff000000));
    }

    virtual ~Surface()
    {
        fImage.reset();

        // BUGBUG
        // unload the dib section
        // and destroy it
    }

    inline long getWidth() { return fWidth; }
    inline long getHeight() { return fHeight; }
    inline Pixel * getData() { return (Pixel *)fData; }

    // Calculate whether a point is whithin our bounds
    inline bool contains(double x, double y) { return ((x >= 0) && (x < fWidth) && (y >= 0) && (y < fHeight)); }

    inline BITMAPINFO getBitmapInfo() {return fBMInfo;}

    inline HDC getDC() {return fBitmapDC;}

    inline BLImage& getImage()
    {
        return fImage;
    }

    inline void set(int x, int y, uint32_t c)
    {
        // reject pixel if out of boundary
        if ((x < 0) || (x >= fWidth) ||
            (y < 0) || (y >= fHeight)){
            return;
        }


        int offset = (int)(y * fWidth) + (int)x;
        ((uint32_t*)fData)[offset] = c;

    }

    inline void set(int x, int y, const Pixel& c)
    {
        x = (int)maths::Clamp(x, 0, fWidth - 1);
        y = (int)maths::Clamp(y, 0, fHeight - 1);

        int offset = (int)(y * fWidth) + (int)x;
        ((Pixel *)fData)[offset] = c;
    }

    inline Pixel get(int x, int y) const
    {
        x = (int)maths::Clamp(x, 0, fWidth - 1);
        y = (int)maths::Clamp(y, 0, fHeight - 1);

        // Get data from BLContext
        int offset = (y * fWidth) + x;
        return ((Pixel *)fData)[offset];
    }


 };
