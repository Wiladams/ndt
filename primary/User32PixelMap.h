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


#include "maths.hpp"
#include "pixelaccessor.h"
#include "memutils.h"


class User32PixelMap : public PixelAccessor<maths::vec4b>
{
    BITMAPINFO fBMInfo{ {0} };              // retain bitmap info for  future usage
    HBITMAP fDIBHandle = nullptr;       // Handle to the dibsection to be created
    HDC     fBitmapDC = nullptr;        // DeviceContext dib is selected into

    // A couple of constants
    static const int bitsPerPixel = 32;
    static const int alignment = 4;

    User32PixelMap(const User32PixelMap& other) = delete;

public:
    User32PixelMap()
    {
        // Create a GDI Device Context
        fBitmapDC = ::CreateCompatibleDC(nullptr);

        // Do some setup to the DC to make it suitable
        // for drawing with GDI if we choose to do that
        ::SetGraphicsMode(fBitmapDC, GM_ADVANCED);
        ::SetBkMode(fBitmapDC, TRANSPARENT);        // for GDI text rendering
    }

    virtual ~User32PixelMap()
    {
        ;
        // and destroy it
        //::DeleteObject(fDIBHandle);
    }


    bool init(int awidth, int aheight)
    {
        // Delete the old DIBSection if it exists
        ::DeleteObject(fDIBHandle);

        size_t bytesPerRow = binops::GetAlignedByteCount(awidth, bitsPerPixel, alignment);

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
        fDIBHandle = ::CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS, (void**)& pdata, nullptr, 0);

        reset(pdata, awidth, aheight, bytesPerRow);

        // select the DIBSection into the memory context so we can 
        // peform GDI operations with it
        ::SelectObject(fBitmapDC, fDIBHandle);



        return true;
    }

    // Make sure all GDI drawing, if any, has completed
    void flush()
    {
        ::GdiFlush();
    }

    BITMAPINFO & bitmapInfo() { return fBMInfo; }
    HDC bitmapDC() { return fBitmapDC; }

    size_t dataSize() { return fBMInfo.bmiHeader.biSizeImage; }

    void setAllPixels(const maths::vec4b& c) override
    {
        for (size_t row = 0; row < fHeight; row++)
            ndt::memset_l((uint8_t*)(fData)+(row*stride()), c.value, fWidth);
    }
    
 };

#endif  // User32PixelMap