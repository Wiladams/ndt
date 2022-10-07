#ifndef User32PixelMap_h
#define User32PixelMap_h

#pragma once

/*
    Simple representation of a bitmap we can use to draw into a window.
    We use a DIBSection because that's the easiest object within Windows 
    to get a pointer to the pixel data.

    we assume a format of 32-bit RGBA to make life very simple.

    A DC Context is also associated with the bitmap, to make it easier 
    to interact with other User32 and GDI interfaces.
*/

#include <windows.h>
#include <cstdio>

#include "bitbang.h"
#include "Graphics.h"
#include "blend2d.h"
#include "pixelarray.h"

// Use this intrinsic for fast memory copies
#pragma intrinsic(__stosd)


class User32PixelMap : public PixelArray
{
    BITMAPINFO fBMInfo{0};              // retain bitmap info for  future usage
    HBITMAP fDIBHandle = nullptr;       // Handle to the dibsection to be created
    HDC     fBitmapDC = nullptr;        // DeviceContext dib is selected into

    //BLImage fImage;

    // A couple of constants
    static const int bitsPerPixel = 32;
    static const int alignment = 4;

    INLINE int GetAlignedByteCount(const int width, const int bitsperpixel, const int alignment)
    {
        return (((width * (bitsperpixel / 8)) + (alignment - 1)) & ~(alignment - 1));
    }

public:
    User32PixelMap()
    {
    }

    User32PixelMap(const long awidth, const long aheight)
    {
        init(awidth, aheight);
    }

    virtual ~User32PixelMap()
    {
        // and destroy it
        ::DeleteObject(fDIBHandle);
    }


    bool init(int awidth, int aheight)
    {
        size_t bytesPerRow = GetAlignedByteCount(awidth, bitsPerPixel, alignment);


        fBMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        fBMInfo.bmiHeader.biWidth = (LONG)awidth;
        fBMInfo.bmiHeader.biHeight = -(LONG)aheight;	// top-down DIB Section
        fBMInfo.bmiHeader.biPlanes = 1;
        fBMInfo.bmiHeader.biClrImportant = 0;
        fBMInfo.bmiHeader.biClrUsed = 0;
        fBMInfo.bmiHeader.biCompression = BI_RGB;
        fBMInfo.bmiHeader.biBitCount = bitsPerPixel;
        fBMInfo.bmiHeader.biSizeImage = DWORD(bytesPerRow * aheight);

        // We'll create a DIBSection so we have an actual backing
        // storage for the context to draw into
        // BUGBUG - check for nullptr and fail if found
        uint8_t* pdata = nullptr;
        fDIBHandle = ::CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS, (void **) & pdata, nullptr, 0);

        attach(pdata, awidth, aheight, bytesPerRow);

        // Create a GDI Device Context
        fBitmapDC = ::CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform GDI operations with it
        ::SelectObject(fBitmapDC, fDIBHandle);

        // Do some setup to the DC to make it suitable
        // for drawing with GDI if we choose to do that
        ::SetGraphicsMode(fBitmapDC, GM_ADVANCED);
        ::SetBkMode(fBitmapDC, TRANSPARENT);        // for GDI text rendering


        // Initialize the BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        //BLResult err = blImageInitAsFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, pdata, (intptr_t)bytesPerRow, nullptr, nullptr);
        //if (err)
        //    return false;

        return true;
    }

    // Make sure all GDI drawing, if any, has completed
    void flush()
    {
        ::GdiFlush();
    }

    BITMAPINFO & bitmapInfo() { return fBMInfo; }
    HDC bitmapDC() { return fBitmapDC; }


    //uint8_t * data() { return fData; }
    size_t dataSize() { return fBMInfo.bmiHeader.biSizeImage; }

    //BLImage& image() { return fImage; }

    Pixel* pixelPointer(const size_t x, const size_t y)
    {
        return (Pixel *)(&fData[(y * fStride) + (x * sizeof(Pixel))]);
    }

    // Retrieve a single pixel
    // This one does no bounds checking, so the behavior is undefined
    // if the coordinates are beyond the boundary
    Pixel pixelGet(const int x, const int y) const
    {
        // Get data from BLContext
        size_t offset = (size_t)(y * width()) + (size_t)x;
        return ((Pixel*)fData)[offset];
    }


    // Set a single pixel value
    // Assume range checking has already occured
    // Perform SRCCOPY operation on a pixel
    void pixelCopy(const int x, const int y, const Pixel &c)
    {
        size_t offset = (size_t)(y * width()) + (size_t)x;
        ((Pixel*)fData)[offset] = c;
    }

    // Set every pixel to a specified value
    // we can use this fast intrinsic to fill
    // the whole area
    void setAllPixels(const Pixel &c)
    {
        size_t nPixels = width() * height();
        __stosd((unsigned long*)fData, c.value, nPixels);
    }
    
 };

#endif  // User32PixelMap