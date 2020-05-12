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
        fDataSize = fBMInfo.bmiHeader.biSizeImage;

        // We'll create a DIBSection so we have an actual backing
        // storage for the context to draw into
        // BUGBUG - check for nullptr and fail if found
        fDIBHandle = CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS, &fData, nullptr, 0);


        // Create a GDI Device Context
        fBitmapDC = CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform operations with it
        SelectObject(fBitmapDC, fDIBHandle);

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

    // Calculate whether a point is whithin our bounds
    bool contains(double x, double y) { return ((x >= 0) && (x < fWidth) && (y >= 0) && (y < fHeight)); }

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

    void set(int x, int y, const Color& c)
    {
        x = (int)constrain(x, 0, fWidth - 1);
        y = (int)constrain(y, 0, fHeight - 1);

        int offset = (int)(y * fWidth) + (int)x;
        ((Color *)fData)[offset] = c;
    }

    Color get(int x, int y)
    {
        x = (int)constrain(x, 0, fWidth - 1);
        y = (int)constrain(y, 0, fHeight - 1);

        // Get data from BLContext
        int offset = (y * fWidth) + x;
        return ((Color *)fData)[offset];
    }
 };
