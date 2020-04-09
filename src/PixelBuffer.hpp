#pragma once

#include <stdint.h>
#include <stdio.h>

#include "grtypes.hpp"
#include "PixelOps.hpp"

/*
    This base class essentially defines an interface
    for the lowest level drawing API on a buffer of pixels.

    The virtual destructor, plus the pure virtual methods
    ensure you can not create an instance of one of these,
    but you can use it as a parameter in function calls.
*/


class PixelBuffer
{
public:
    enum HorizontalOrientation : int {
        LeftToRight = 0,
        RightToLeft = 1
    };

    enum VerticalOrientation : int {
        BottomToTop = 0,
        TopToBottom = 1,
    };

private:
    // private default constructor, so this can not
    // be an un-initialized element in an array 
    // or otherwise default constructed
    PixelBuffer();

    // The width and height are const because
    // you don't want them to ever change
    const size_t width;   //how many pixels wide
    const size_t height;  // how many pixels tall
    const int fBitsPerPixel;
    HorizontalOrientation fHorizontalOrientation;
    VerticalOrientation fVerticalOrientation;

protected:
    virtual void setData(void *, int length) = 0;

public:

    PixelBuffer(const size_t awidth, const size_t aheight, const int bpp)
        :width(awidth), height(aheight), fBitsPerPixel(bpp)
    {
        fHorizontalOrientation = LeftToRight;
        fVerticalOrientation = TopToBottom;
    }

    // virtual destructor here to ensure the derived class
    // has a chance to create their own destructor
    virtual ~PixelBuffer() = 0 {};

    // Properties
    void setHorizontal(HorizontalOrientation value) {fHorizontalOrientation = value;}
    HorizontalOrientation getHorizontal() {return fHorizontalOrientation;}

    void setVertical(VerticalOrientation value) {fVerticalOrientation = value;}
    VerticalOrientation getVertical() {return fVerticalOrientation;}
    
    GRSIZE getWidth() const { return this->width;}
    GRSIZE getHeight() const { return this->height;}
    size_t getBitsPerPixel() const {return fBitsPerPixel;}
    
    virtual void * getData() = 0;
    virtual const void * getConstData() {return (const void *) getData();}
    virtual const int getDataLength() const = 0;
    virtual const void * getPixelPointer(int x, int y) const = 0;



    // Sub-classes MUST implement the following
    // Set a single pixel value at the specified location
    virtual bool setPixel(GRCOORD x, GRCOORD y, const PixRGBA &pix) = 0;
    
    // Transfer a pixel using an operation
    virtual bool transferPixel(GRCOORD x, GRCOORD y, const PixRGBA &src, const PixelTransferOp &tOp)
    {
        // don't render pixels out of bounds
        if (((x < 0) || (x >= width)) ||
            ((y< 0) || (y >= height))) {
                return false;
        }
        
        //PixRGBA dst = getPixel(x, y);
        //PixRGBA pix = tOp(x, y, src, dst);
        //return setPixel(x, y, pix);

        return setPixel(x, y, tOp(x, y, src, getPixel(x, y)));
    }

    // Retrieve a single pixel value from the specified location 
    virtual PixRGBA getPixel(GRCOORD x, GRCOORD y) const = 0;
    
    // Draw a horizontal line using srccopy as fast as possible
    virtual void setPixels(GRCOORD x, GRCOORD y, GRSIZE width, const PixRGBA src)
    {
        for (int col=x; col<x+width; col++) {
            setPixel(col,y, src);
        }
    }

    // Draw a horizontal line with transfer op
    virtual void horizontalLine(GRCOORD x, GRCOORD y, GRSIZE width, const PixRGBA src, const PixelTransferOp &tOp) 
    {
        for (int col=x; col<x+width; col++) {
            transferPixel(col,y, src, tOp);
        }
    }

    // BUGBUG - There should be a TransferSpan() that honors a transfer function
    // Copy the span of pixels into the pixel buffer
    // Like a horizontal line, but individual color values
    virtual bool setSpan(const GRCOORD x, const GRCOORD y, const GRSIZE width, const PixRGBA * pix)
    {
        for (int offset =0; offset < width; offset++)
        {
            setPixel(x+offset, y, pix[offset]);
        }

        return true;
    }

    
    // Set all pixels within the pixel buffer to the specified value
    // we do a brute force implementation here so there's one less
    // thing for a derived class to implement. 
    virtual bool setAllPixels(const PixRGBA &pix)
    {
        for (int row=0;row<height-1;row++)
        {
            for (int col=0; col<width-1; col++)
            {
                setPixel(col, row, pix);
            }
        }
        
        return true;
    }

    virtual bool blit(const PixelBuffer &src, 
        int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight)
    {
        //printf("blit, src size : %d X %d\n", srcWidth, srcHeight);
        //printf("blit, dst size : %d X %d\n", destWidth, destHeight);

        for (int row=destY;row<(destY+destHeight-1);row++){
            for (int col=destX; col<(destX+destWidth-1); col++)
            {
                int dx = MAP(col, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                int dy = MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);
                
                //printf("blit: %d %d\n", dx, dy);

                PixRGBA pix = src.getPixel(dx,dy);
                printf("blit, pixel: %d %d (%d, %d, %d)\n", dx, dy, pix.red, pix.green, pix.blue);

                setPixel(col, row, pix);
            }
        }

        return true;
    }

    virtual bool blend(const PixelBuffer &src, 
        int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight, const PixelTransferOp &tOp)
    {
        //printf("blit, src size : %d X %d\n", srcWidth, srcHeight);
        //printf("blit, dst size : %d X %d\n", destWidth, destHeight);

        for (int row=destY;row<(destY+destHeight-1);row++){
            for (int col=destX; col<(destX+destWidth-1); col++)
            {
                int dx = MAP(col, destX, destX+destWidth-1, srcX, srcX+srcWidth-1);
                int dy = MAP(row, destY, destY+destHeight-1, srcY, srcY+srcHeight-1);
                
                //printf("blit: %d %d\n", dx, dy);

                PixRGBA pix = src.getPixel(dx,dy);
                //printf("blit, pixel: %d %d (%d, %d, %d)\n", dx, dy, pix.red, pix.green, pix.blue);

                transferPixel(col, row, pix, tOp);
            }
        }

        return true;
    }


};
