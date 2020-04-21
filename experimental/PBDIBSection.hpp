#pragma once

/*
    The mating between a DIBSection and a BLImage
*/

#include <windows.h>

#include "bitbang.hpp"
#include "blend2d.h"

class PBDIBSection
{
    // for interacting with win32
    BITMAPINFO fBMInfo;
    HBITMAP fGDIHandle;
    HDC     fBitmapDC;      
    BLRgba32 * fData;       // A pointer to the data
    size_t fDataSize;       // How much data is allocated
    long fWidth;
    long fHeight;

    // For interacting with blend2d
    BLImage fImage;
    BLContext fContext;     // for Blend2D drawing

public:
    PBDIBSection(long awidth, long aheight)
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
    

        fGDIHandle = CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS,(void **)&fData,nullptr,0);
        fDataSize = fBMInfo.bmiHeader.biSizeImage;
        
        // memory Device Context
        fBitmapDC = CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform operations with it
        SelectObject(fBitmapDC, fGDIHandle);

        // Bind BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        BLResult bResult = blImageCreateFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, fData, bytesPerRow, nullptr, nullptr);
        
        //fContext.begin(fImage);
        BLContextCreateInfo createInfo{};
        createInfo.threadCount = 4;
        bResult = fContext.begin(fImage, createInfo);
    }

    virtual ~PBDIBSection()
    {
        fContext.end();
        fImage.reset();
    }
    
    long getWidth() { return fWidth; }
    long getHeight() { return fHeight; }
    BLRgba32* getData() { return fData; }

    BITMAPINFO getBitmapInfo()
    {
        return fBMInfo;
    }

    HDC getDC()
    {
        return fBitmapDC;
    }

    BLContext & getBlend2dContext()
    {
        return fContext;
    }

    BLImage& getBlend2dImage()
    {
        return fImage;
    }

    void flush()
    {
        fContext.flush(BL_CONTEXT_FLUSH_SYNC);
    }
};