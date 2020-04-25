#pragma once

/*
    A representation of an image object
*/

#include "grtypes.hpp"
#include "PixelBufferRGBA32.hpp"


class P5Image : public PixelBufferRGBA32 
{


public:
    const GRCOORD width;
    const GRCOORD height;
    uint8_t *pixels;

    P5Image(GRCOORD awidth, GRCOORD aheight)
        : PixelBufferRGBA32(awidth, aheight),
        width(awidth), height(aheight)
    {
        // Pixels are actually in BGRA format as far as accessing
        // from an array is concerned
        pixels = (uint8_t *)this->getData();
    }

    // A couple of do nothing routines
    void loadPixels() { pixels = (uint8_t *)this->getData(); }
    void updatePixels() {
        // this->setData(uint8_t *data);
    }

    // Doing drawing
    void set(GRCOORD x1, GRCOORD y1, const PixRGBA &c)
    {
        setPixel(x1, y1, c);
    }

    // retrieve a pixel
    PixRGBA get(GRCOORD x, GRCOORD y)
    {
        return getPixel(x, y);
    }

    // resize()
    // copy()
    void copy(const PixelBuffer &src, 
        GRCOORD srcX, GRCOORD srcY, GRSIZE srcWidth, GRSIZE srcHeight,
        GRCOORD destX, GRCOORD destY, GRSIZE destWidth, GRSIZE destHeight)
    {
        for (GRCOORD row=destY;row<(destY+destHeight-1);row++){
            for (GRCOORD col=destX; col<(destX+destWidth-1); col++)
            {
                GRCOORD dx = (GRCOORD)MAP(col, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                GRCOORD dy = (GRCOORD)MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);

                PixRGBA pix = src.getPixel(dx,dy);
                //printf("pixel: %d %d\n", dx, dy);

                set(col, row, pix);
            }
        }
    }

    void copy(GRCOORD srcX, GRCOORD srcY, GRSIZE srcWidth, GRSIZE srcHeight,
        GRCOORD destX, GRCOORD destY, GRSIZE destWidth, GRSIZE destHeight)
    {
        copy(*this, srcX, srcY, srcWidth, srcHeight, destX, destY, destWidth, destHeight);
    }


    // mask()
    // filter()
    // blend()
    // save()

};