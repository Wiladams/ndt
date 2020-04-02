#pragma once

#include "grtypes.hpp"
#include "PixelBuffer.hpp"

/*
    Pixel transfer is the way in which a source pixel (src)
    is combined with a destination pixel (dst).  The simplest
    operation is SRCCOPY, where the source pixel simply replaces
    the destination pixel.
*/
class PixelTransferOp
{
public:
    virtual PixRGBA operator()(GRCOORD x, GRCOORD y, const PixRGBA &src, const PixRGBA &dst) const = 0;
};

class PixelSRCCOPY : public PixelTransferOp
{
public:
    virtual PixRGBA operator()(GRCOORD x, GRCOORD y, const PixRGBA &src, const PixRGBA &dst) const
    {
        return src;
    }

};

// Do a pixel transfer based on the source transparency
class PixelSRCOVER : public PixelTransferOp
{
public:
    virtual PixRGBA operator()(GRCOORD x, GRCOORD y, const PixRGBA &src, const PixRGBA &dst) const
    {

        // if the source pixel is fully transparent
        // then just resturn the destination pixel
        if (src.intValue == 0) {
            return dst;
        }

        if (src.alpha == 255) {
            // in case of full opacity,
            // it's essentially SRCCOPY
            return src;
        } else if (src.alpha == 0) {
            // if the alpha of the src is 0
            // then combine the color values
            // but don't change the alpha of the destination
            return dst;
        } else {
            // for all other cases
            // do the interpolation of colors and alphas
            //printf("SRCOVER: %d\n", src.alpha);

            uint8_t alpha = src.alpha;
            PixRGBA dstPix;
            dstPix.red = lerp255(dst.red, src.red, alpha);
            dstPix.green = lerp255(dst.green, src.green, alpha);
            dstPix.blue = lerp255(dst.blue, src.blue, alpha);
            dstPix.alpha = lerp255(dst.alpha, src.alpha, alpha);
            return dstPix;
        }

        // we should never get here
        return src;
    }

};