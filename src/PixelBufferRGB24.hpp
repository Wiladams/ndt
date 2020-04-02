#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "PixelBuffer.hpp"
#include "RectangleI.hpp"

/*
    This is a class that represents a framebuffer.
    A framebuffer is the most rudimentary graphics
    object.  It is meant to be displayed on the screen,
    or passed around for remote viewing, and whatnot.

    There are very few drawing routines associated with 
    the framebuffer itself.  Most routines are related
    to the PixelBuffer Object instead.

    What should be in here is the ability to blit
    a PixelBuffer, in various ways.

    The format of a FrameBuffer is 32-bit pixels.
*/
class PixelBufferRGB24 : public PixelBuffer {
private:
    // private default constructor, so this can not
    // be an un-initialized element in an array 
    PixelBufferRGB24();

    PixRGB * data;      // a pointer to the actual pixel data
    int fDataLength;
    bool fOwnsData;

protected:
    void setData(void *pData, const int length)
    {
        data = (PixRGB *)pData;
        fDataLength = length;
    }

public:
    // Public constructor
    // must assign to const fields using ':'
    // mechanism.
    PixelBufferRGB24(const size_t width, const size_t height)
        : PixelBuffer(width, height, 24)
    {
        data = {new PixRGB[width*height]{}};
        fOwnsData = true;
    }

    PixelBufferRGB24(const size_t width, const size_t height, void *pixels)
        : PixelBuffer(width, height, 24), 
        data((PixRGB *)pixels)
    {
        fOwnsData = false;
    }

    // Virtual destructor so this can be sub-classed
    virtual ~PixelBufferRGB24(){
        // must delete the data element, as we 
        // constructred it.
        if (fOwnsData) {
            delete [] data;
        }
    }

    // We should not do the following as it allows
    // the data pointer to escape our control
    // it also allows unrestricted access to the data itself
    // which breaks encapsulation.
    // PixRGBA * getData() const {return this->data;}
    // BUT, very convenient, and the const helps somewhat
    virtual void * getData()
    {
        return data;
    }
    virtual const int getDataLength() const {return fDataLength;}
    
    virtual const void * getPixelPointer(int x, int y) const
    {
        size_t offset = y * getWidth() + x;
        return &data[offset];
    }

    // Set the value of a single pixel
    bool setPixel(GRCOORD x, GRCOORD y, const PixRGBA &pix)
    {
        //printf("setPixel: %d %d (%d,%d,%d)\n", x, y, pix.red, pix.green, pix.blue);

        // BUGBUG - we should quick reject if alpha == 0
        if (x>= getWidth() || y >= getHeight()  || (pix.alpha == 0)) 
        {
            return false;   // outside bounds, or invisible
        }
        size_t offset = y * getWidth() + x;
        data[offset] = PixRGB(pix.red, pix.green, pix.blue);

        return true;
    }

    // get the value of a single pixel
    // marked as 'const' because it does not alter the contents
    // of the FrameBuffer
    PixRGBA getPixel(GRCOORD x, GRCOORD y) const
    {
        //printf("RGB24.getPixel: %d %d, width: %d height: %d\n", x, y, getWidth(), getHeight());

        size_t offset = y * getWidth() + x;
        //PixRGBA c = data[offset];
        //printf("getPixel: (%d,%d,%d)\n", c.red, c.green, c.blue);
        //return c;
        return PixRGBA(data[offset].red, data[offset].green, data[offset].blue, 255);
    }

/*
    bool blit(const PixelBuffer &src, 
        int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight)
    {
        //printf("blit, src size : %d X %d\n", srcWidth, srcHeight);
        //printf("blit, dst size : %d X %d\n", destWidth, destHeight);
        
        // If the source and destination start out as the same size
        // then we can perform some copy optimizations
        bool isOptimal = ((destWidth == srcWidth) && (destHeight == srcHeight));    // AND formats are the same
        
        RectangleI myRect(0,0,getWidth(), getHeight());
        RectangleI destRect(destX, destY, destWidth, destHeight);
        RectangleI clipRect = RectangleI::intersection(myRect, destRect);

        if (isOptimal) {
            //printf("OPTIMAL\n");
            for (int row=clipRect.y1; row<clipRect.y2; row++) {
                int sx = MAP(clipRect.x1, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                int sy = MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);
                // get pointer from source
                //    virtual const void * getPixelPointer(int x, int y) const = 0;
                const PixRGBA * pixPtr = (const PixRGBA *)src.getPixelPointer(sx, sy);
                // setSpan on ourself
                setSpan(clipRect.x1, row, clipRect.getWidth(), pixPtr);
            }
        } else {
            for (int row=clipRect.y1; row<clipRect.y2; row++) {
                for (int col=clipRect.x1; col<clipRect.x2; col++)
                {
                    int dx = MAP(col, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                    int dy = MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);
                
                    //printf("blit: %d %d\n", dx, dy);

                    PixRGBA pix = src.getPixel(dx,dy);
                    //printf("blit, pixel: %d %d (%d, %d, %d)\n", dx, dy, pix.red, pix.green, pix.blue);

                    setPixel(col, row, pix);
                }
            }
        }

        return true;
    }
*/

};
