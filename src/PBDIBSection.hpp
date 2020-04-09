#pragma once

/*
    The mating between a DIBSection and a PixelBufferRGBA32
*/

#include "w32.hpp"
#include "PixelBufferRGBA32.hpp"
#include "bitbang.hpp"


class PBDIBSection : public PixelBufferRGBA32
{
    BITMAPINFO fBMInfo;
    HBITMAP fGDIHandle;
    HDC     fBitmapDC;

public:
    PBDIBSection(size_t awidth, size_t aheight)
        : PixelBufferRGBA32(awidth, aheight, nullptr)
    {
        int bitsPerPixel = 32;
        int alignment = 4;
        int bytesPerRow = bitbang::GetAlignedByteCount(awidth, bitsPerPixel, alignment);

        fBMInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        fBMInfo.bmiHeader.biWidth = awidth;
        fBMInfo.bmiHeader.biHeight = -aheight;	// top-down DIB Section
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
    }

    BITMAPINFO getBitmapInfo()
    {
        return fBMInfo;
    }

    HDC getDC()
    {
        return fBitmapDC;
    }
};