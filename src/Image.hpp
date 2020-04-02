#pragma once

/*
    A representation of an image object
*/

#include "grtypes.hpp"
#include "PixelBufferRGBA32.hpp"
#include "DrawingContext.hpp"

class Image : public PixelBufferRGBA32 
{


public:
    int originX;
    int originY;

    const size_t width;
    const size_t height;
    uint8_t *pixels;

    Image(size_t awidth, size_t aheight)
        : width(awidth), height(aheight), PixelBufferRGBA32(awidth, aheight)
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
    void set(size_t x1, size_t y1, PixRGBA c)
    {
        setPixel(x1, y1, c);
    }

    // retrieve a pixel
    PixRGBA get(size_t x, size_t y)
    {
        return getPixel(x, y);
    }

    // resize()
    // copy()
    void copy(const PixelBuffer &src, 
        int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight)
    {
        for (int row=destY;row<(destY+destHeight-1);row++){
            for (int col=destX; col<(destX+destWidth-1); col++)
            {
                int dx = MAP(col, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                int dy = MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);

                PixRGBA pix = src.getPixel(dx,dy);
                //printf("pixel: %d %d\n", dx, dy);

                set(col, row, pix);
            }
        }
    }

    void copy( int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight)
    {
        copy(*this, srcX, srcY, srcWidth, srcHeight, destX, destY, destWidth, destHeight);
    }
    // mask()
    // filter()
    // blend()
    // save()

    /* Animated GIF related */
    // reset()
    // getCurrentFrame()
    // setFrame()
    // numFrames()
    // play()
    // pause()
    // delay()
};