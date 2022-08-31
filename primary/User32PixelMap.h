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

// Use this intrinsic for fast memory copies
#pragma intrinsic(__stosd)


class User32PixelMap //: public PixelMap
{
    BITMAPINFO fBMInfo{0};              // retain bitmap info for  future usage
    HBITMAP fDIBHandle = nullptr;       // Handle to the dibsection to be created
    HDC     fBitmapDC = nullptr;        // DeviceContext dib is selected into

    uint8_t * fData = nullptr;
    size_t fWidth = 0;
    size_t fHeight = 0;
    ptrdiff_t fStride = 0;

    BLImage fImage;

    // A couple of constants
    static const int bitsPerPixel = 32;
    static const int alignment = 4;

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


    virtual bool init(int awidth, int aheight)
    {
        size_t bytesPerRow = ndt::GetAlignedByteCount(awidth, bitsPerPixel, alignment);

        fWidth = awidth;
        fHeight = aheight;
        fStride = bytesPerRow;      // stride could be negative if bottom up orientation used

        fBMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        fBMInfo.bmiHeader.biWidth = awidth;
        fBMInfo.bmiHeader.biHeight = -(LONG)aheight;	// top-down DIB Section
        fBMInfo.bmiHeader.biPlanes = 1;
        fBMInfo.bmiHeader.biClrImportant = 0;
        fBMInfo.bmiHeader.biClrUsed = 0;
        fBMInfo.bmiHeader.biCompression = BI_RGB;
        fBMInfo.bmiHeader.biBitCount = bitsPerPixel;
        fBMInfo.bmiHeader.biSizeImage = bytesPerRow * aheight;

        // We'll create a DIBSection so we have an actual backing
        // storage for the context to draw into
        // BUGBUG - check for nullptr and fail if found
        fDIBHandle = ::CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS, (void **) & fData, nullptr, 0);

        //attach(data, awidth, aheight, bytesPerRow);

        // Create a GDI Device Context
        fBitmapDC = ::CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform GDI operations with it
        ::SelectObject(fBitmapDC, fDIBHandle);

        // Do some setup to the DC to make it suitable
        // for drawing with GDI if we choose to do that
        ::SetGraphicsMode(fBitmapDC, GM_ADVANCED);
        ::SetBkMode(fBitmapDC, TRANSPARENT);
        //::SetBkColor(fBitmapDC, 0x0);

        // Initialize the BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        BLResult bResult = blImageInitAsFromData(&fImage, fWidth, fHeight, BL_FORMAT_PRGB32, fData, (intptr_t)bytesPerRow, nullptr, nullptr);


        return true;
    }

    // Make sure all GDI drawing, if any, has completed
    void flush()
    {
        ::GdiFlush();
    }

    //INLINE BITMAPINFO getBitmapInfo() { return fBMInfo; }
    INLINE HDC getDC() { return fBitmapDC; }


    constexpr size_t width() const noexcept { return fWidth; }
    constexpr size_t height() const noexcept { return fHeight; }
    constexpr ptrdiff_t stride() const noexcept { return fStride; }

    uint8_t * data() { return fData; }
    size_t dataSize() { return fBMInfo.bmiHeader.biSizeImage; }

    BLImage& image() { return fImage; }

    Pixel* rowPointer(const size_t y)
    {
        return (Pixel*)(&fData[y * fStride]);
    }

    Pixel* pixelPointer(const size_t x, const size_t y)
    {
        return (Pixel *)(&fData[(y * fStride) + (x * sizeof(Pixel))]);
    }

    // Retrieve a single pixel
    // This one does no bounds checking, so the behavior is undefined
    // if the coordinates are beyond the boundary
    INLINE Pixel pixelGet(const int x, const int y) const
    {
        // Get data from BLContext
        size_t offset = (size_t)(y * width()) + (size_t)x;
        return ((Pixel*)fData)[offset];
    }


    // Set a single pixel value
    // Assume range checking has already occured
    // Perform SRCCOPY operation on a pixel
    INLINE void pixelCopy(const int x, const int y, const Pixel &c)
    {
        size_t offset = (size_t)(y * width()) + (size_t)x;
        ((Pixel*)fData)[offset] = c;
    }
    /*
    // Perform SRCOVER operation on a pixel
    INLINE void blendPixel(const int x, const int y, const PixelRGBA &c) override
    {
        size_t offset = (size_t)(y * width()) + (size_t)x;
        ((PixelRGBA*)fData)[offset] = blend_pixel(((PixelRGBA*)fData)[offset],c);
    }

    // set consecutive pixels in a row 
    // Assume the range has already been clipped
    INLINE void span_copy(const int x, const int y, const int w, const Pixel &src)
    {
        // do line clipping
        // copy actual pixel data
        uint32_t* pixelPtr = (uint32_t*)pixelPointer(x, y);
        __stosd((unsigned long*)pixelPtr, src.value, w);
    }
    */
    // Set every pixel to a specified value
    // we can use this fast intrinsic to fill
    // the whole area
    INLINE void setAllPixels(const Pixel &c)
    {
        size_t nPixels = width() * height();
        __stosd((unsigned long*)fData, c.value, nPixels);
    }
    
 };
