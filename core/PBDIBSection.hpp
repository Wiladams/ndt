#pragma once

/*
    The mating between a DIBSection and a PixelBufferRGBA32
*/

#include <windows.h>

#include "PixelBufferRGBA32.hpp"
#include "bitbang.hpp"
#include "blend2d.h"

class PBDIBSection : public PixelBufferRGBA32
{
    // for interacting with win32
    BITMAPINFO fBMInfo;
    HBITMAP fGDIHandle;
    HDC     fBitmapDC;      
    
    // For interacting with blend2d
    BLImage fImage;
    BLContext fContext;     // for Blend2D drawing

public:
    PBDIBSection(GRSIZE awidth, GRSIZE aheight)
        : PixelBufferRGBA32(awidth, aheight, nullptr)
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
    
        void * pData;
        fGDIHandle = CreateDIBSection(nullptr, &fBMInfo, DIB_RGB_COLORS,(void **)&pData,nullptr,0);
        setData(pData, fBMInfo.bmiHeader.biSizeImage);
        
        // memory Device Context
        fBitmapDC = CreateCompatibleDC(nullptr);

        // select the DIBSection into the memory context so we can 
        // peform operations with it
        SelectObject(fBitmapDC, fGDIHandle);

        // Bind BLImage
        // MUST use the PRGB32 in order for SRC_OVER operations to work correctly
        BLResult bResult = blImageCreateFromData(&fImage, awidth, aheight, BL_FORMAT_PRGB32, pData, bytesPerRow, nullptr, nullptr);
        
        //fContext.begin(fImage);
        BLContextCreateInfo createInfo{};
        createInfo.threadCount = 4;
        bResult = fContext.begin(fImage, createInfo);
        //bResult = fContext.begin(fImage);
        //printf("fContext.begin: %d\n", bResult);
    }

    virtual ~PBDIBSection()
    {
        fContext.end();
        fImage.reset();
    }
    
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