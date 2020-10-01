#pragma once

#include "blend2d.h"
#include "grmath.h"

class GCanvas : public BLContext
{
	BLImage fImage;
    BLImageData fImageData;

public:
	GCanvas(const int w, const int h, uint32_t pixelFormat = BL_FORMAT_PRGB32)
		: fImage(w, h, pixelFormat)
	{
        fImage.getData(&fImageData);
		begin(fImage);	// Single threaded
	}

    inline size_t getWidth() { return fImageData.size.w; }
    inline size_t getHeight() { return fImageData.size.h; }

    BLImage& getImage() { return fImage; }

	// cast to a BLImage
	operator const BLImage& () { return fImage; }


    void sync()
    {
        BLContext::flush(BL_CONTEXT_FLUSH_SYNC);
    }

    inline int getBytesPerPixel()
    {
        switch (fImageData.format) {
        case BL_FORMAT_A8:
            return 1;
            break;
        case BL_FORMAT_PRGB32:
        case BL_FORMAT_XRGB32:
            return 4;
            break;
        }

        return 0;
    }

    
    void set(const int x, const int y, const BLRgba32& c)
    {
        if ((x < 0 || x >= fImageData.size.w) || (y < 0 || y >= fImageData.size.h)) 
            return;

        int bytesPer = getBytesPerPixel();
        int offset = (int)(y * fImageData.stride) + (int)x * bytesPer;

        switch (bytesPer) {
            case 4: {
                int offset = y * fImageData.size.w + x;
                ((BLRgba32*)fImageData.pixelData)[offset] = c;
            }
            break;
        }

    }
    

    BLRgba32 get(int x, int y)
    {
        if ((x < 0 || x >= fImageData.size.w) || (y < 0 || y >= fImageData.size.h))
            return BLRgba32();


        // Get data from BLContext
        int bytesPer = getBytesPerPixel();

        
        switch (bytesPer) {
            case 1: {
                int offset = (int)(y * fImageData.stride) + (int)x * bytesPer;
                uint8_t c = ((uint8_t*)fImageData.pixelData)[offset];
                return BLRgba32(c, c, c, c);
            }
            break;

            case 4: {
                int offset = y * fImageData.size.w + x;
                return ((BLRgba32*)fImageData.pixelData)[offset];
            }
            break;
        }

        return BLRgba32();
    }
};