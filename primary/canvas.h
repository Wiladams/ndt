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

	// cast to a BLImage
	operator const BLImage& () { return fImage; }

    void flush()
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

    /*
    void set(const int x, const int y, const BLRgba32& c)
    {
        int px = (int)Clamp(x, 0, fImageData.size.w - 1);
        int py = (int)Clamp(y, 0, fImageData.size.h - 1);

        int bytesPer = bpp(fImageData);
        int offset = (int)(y * fImageData.stride) + (int)x*bytesPer;
        ((Pixel*)fData)[offset] = c;
    }
    */

    BLRgba32 get(int x, int y)
    {
        int px = (int)Clamp(x, 0, fImageData.size.w - 1);
        int py = (int)Clamp(y, 0, fImageData.size.h - 1);

        // Get data from BLContext
        int bytesPer = getBytesPerPixel();
        int offset = (int)(y * fImageData.stride) + (int)x * bytesPer;
        
        switch (bytesPer) {
            case 1: {
                uint8_t c = ((uint8_t*)fImageData.pixelData)[offset];
                return BLRgba32(c, c, c, c);
            }
            break;

            case 4:
                return ((BLRgba32*)fImageData.pixelData)[offset];
            break;
        }

        return BLRgba32();
    }
};