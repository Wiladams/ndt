#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "PixelBuffer.hpp"
#include "RectangleI.hpp"

#pragma intrinsic(__stosd)

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
class PixelBufferRGBA32 : public PixelBuffer {
private:
    // private default constructor, so this can not
    // be an un-initialized element in an array 
    PixelBufferRGBA32();

    PixRGBA * data;         // a pointer to the actual pixel data
    int fDataLength;
    bool fOwnsData;

protected:
    void setData(void *pData, int length)
    {
        data = (PixRGBA *)pData;
        fDataLength = length;
    }

public:
    // Public constructor
    // must assign to const fields using ':'
    // mechanism.
    PixelBufferRGBA32(const size_t width, const size_t height)
        : PixelBuffer(width, height, 32)
    {
        data = {new PixRGBA[width*height]{0}};
        fOwnsData = true;
    }

    PixelBufferRGBA32(const size_t width, const size_t height, void *pixels)
        : PixelBuffer(width, height, 32), 
        data((PixRGBA *)pixels)
    {
        fOwnsData = false;
    }

    // Virtual destructor so this can be sub-classed
    virtual ~PixelBufferRGBA32(){
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

    virtual const int getDataLength() const
    {
        return fDataLength;
    }

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
        data[offset] = pix;

        return true;
    }

    // get the value of a single pixel
    // marked as 'const' because it does not alter the contents
    // of the FrameBuffer
    PixRGBA getPixel(GRCOORD x, GRCOORD y) const
    {
        //printf("getPixel: %d %d, width: %d height: %d\n", x, y, getWidth(), getHeight());

        size_t offset = y * getWidth() + x;
        //PixRGBA c = data[offset];
        //printf("getPixel: (%d,%d,%d)\n", c.red, c.green, c.blue);
        //return c;
        return this->data[offset];
    }

    // setPixels()
    // 
    // set the values of a contiguous set of pixels
    bool setSpan(const GRCOORD x, const GRCOORD y, const GRSIZE width, const PixRGBA * pix)
    {
        // size_t is a good choice, as it's typically the machine's largest
        // unsigned int
        size_t offset = y * getWidth() + x;
        
        // BUGBUG
        // we can do clipping here by reducing width to whatever
        // is remaining on the line, rather than fulfilling the
        // entire 'width' request
        uint32_t * destPtr = (uint32_t *)getPixelPointer(x, y);
        memcpy(destPtr, pix, width*sizeof(PixRGBA));

        //__movsd((unsigned long *)destPtr, (unsigned long *)pix, width);

        return true;
    }

    // set consecutive pixels in a row (srccopy)
    virtual void setPixels(GRCOORD x, GRCOORD y, GRSIZE width, const PixRGBA src)
    {
        uint32_t * pixelPtr = (uint32_t *)getPixelPointer(x, y);
        __stosd((unsigned long *)pixelPtr, src.intValue, width);
        //for (int col=x; col<x+width; col++) {
        //    setPixel(col,y, src);
        //}
    }

    // setAllPixels()
    // Set all the pixels in the framebuffer to the value specified
    // Since we know we're storing aligned 32-bit integers (DWORD)
    // we can use a compiler intrinsic to speed things up over the
    // brute force method provided in PixelBuffer.  
    // Should be faster than a memcpy as well? Something to measure
    bool setAllPixels(const PixRGBA &value)
    {
        //printf("setAllPixels: 0x%x\n", value.intValue);
        size_t nPixels = getWidth() * getHeight();
        __stosd((unsigned long *)data, value.intValue, nPixels);

        return true;
    }

    bool blit(const PixelBuffer &src, 
        int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight)
    {
        //printf("blit, src size : %d X %d\n", srcWidth, srcHeight);
        //printf("blit, dst size : %d X %d\n", destWidth, destHeight);
        
        // If the source and destination start out as the same size
        // AND formats are the same
        // then we can perform some copy optimizations
        bool isOptimal = ((src.getBitsPerPixel() == getBitsPerPixel()) && 
                ((destWidth == srcWidth) && (destHeight == srcHeight)));    

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
                    // Need to change coordinates based on horizontal
                    // and vertical orientation
                    int dx;
                    int dy;

                    if (getVertical() == TopToBottom) {
                        dx = MAP(col, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                        dy = MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);
                    } else {
                        dx = MAP(col, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                        dy = MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);
                    }
                    
                    PixRGBA pix = src.getPixel(dx,dy);
                    setPixel(col, row, pix);
                    //printf("blit, pixel: %d %d (%d, %d, %d, %d)\n", dx, dy, pix.red, pix.green, pix.blue, pix.alpha);
                }
            }
        }

        return true;
    }


};
