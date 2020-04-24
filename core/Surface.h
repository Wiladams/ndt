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
    BITMAPINFO fBMInfo;
    HBITMAP fGDIHandle;
    HDC     fBitmapDC;
    void * fData;       // A pointer to the data
    size_t fDataSize;       // How much data is allocated
    long fWidth;
    long fHeight;

    // For interacting with blend2d
    BLImage fImage;



public:
    Surface(long awidth, long aheight)
        : fWidth(awidth),
        fHeight(aheight)
    {
        int bitsPerPixel = 32;
        int alignment = 4;
        int bytesPerRow = bitbang::GetAlignedByteCount(awidth, bitsPerPixel, alignment);

        fBMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        fBMInfo.bmiHeader.biWidth = awidth;
        fBMInfo.bmiHeader.biHeight = -(LONG)aheight;	// top-down DIB Section
        fBMInfo.bmiHeader.biPlanes = 1;
        fBMInfo.bmiHeader.biBitCount = bitsPerPixel;
        fBMInfo.bmiHeader.biSizeImage = bytesPerRow * aheight;
        fBMInfo.bmiHeader.biClrImportant = 0;
        fBMInfo.bmiHeader.biClrUsed = 0;
        fBMInfo.bmiHeader.biCompression = BI_RGB;

        // We'll create a DIBSection so we have an actual backing
        // storage for the context to draw into
        fGDIHandle = CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS, &fData, nullptr, 0);
        fDataSize = fBMInfo.bmiHeader.biSizeImage;

        // Create a GDI Device Context
        fBitmapDC = CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform operations with it
        SelectObject(fBitmapDC, fGDIHandle);

        // Initialize the BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        //BLResult bResult = blImageCreateFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, fData, bytesPerRow, nullptr, nullptr);
        BLResult bResult = blImageInitAsFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, fData, (intptr_t)bytesPerRow, nullptr, nullptr);

        // Initialize the BLContext
        BLContextCreateInfo createInfo{};
        createInfo.threadCount = 4;
        bResult = getBlend2dContext().begin(fImage, createInfo);


        getBlend2dContext().setFillStyle(BLRgba32(0xffffffff));
        getBlend2dContext().setStrokeStyle(BLRgba32(0xff000000));
    }

    virtual ~Surface()
    {
        fImage.reset();
    }

    long getWidth() { return fWidth; }
    long getHeight() { return fHeight; }
    Color * getData() { return (Color *)fData; }

    BITMAPINFO getBitmapInfo()
    {
        return fBMInfo;
    }

    HDC getDC()
    {
        return fBitmapDC;
    }

    BLImage& getBlend2dImage()
    {
        return fImage;
    }

    void set(double x, double y, const Color& c)
    {
        int offset = (int)(y * fWidth) + (int)x;
        ((Color *)fData)[offset] = c;
    }

    Color get(double x, double y)
    {
        // Get data from BLContext
        int offset = (int)(y * fWidth) + (int)x;
        return ((Color *)fData)[offset];
    }
 };
