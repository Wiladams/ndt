#include "PixelBuffer.hpp"
#include "functor.hpp"

/*
    The PixelBufferCodec has functor representations
    for all drawing operations against a PixelBuffer.

    These functors are meant to be fed to a computicle
    to be executed in a separate thread.  They are also
    meant to provide a serialized representation of the
    commands so they can be executed remotely.

    In this sense, executing in a computicle environment
    is just a specialization of remote execution.
*/

/*
    virtual bool setPixel(GRCOORD x, GRCOORD y, const PixRGBA &pix) = 0;
    virtual bool transferPixel(GRCOORD x, GRCOORD y, const PixRGBA &src, const PixelTransferOp &tOp)
    virtual void setPixels(GRCOORD x, GRCOORD y, GRSIZE width, const PixRGBA src)
    virtual void horizontalLine(GRCOORD x, GRCOORD y, GRSIZE width, const PixRGBA src, const PixelTransferOp &tOp) 
    virtual bool setSpan(const GRCOORD x, const GRCOORD y, const GRSIZE width, const PixRGBA * pix)
    virtual bool setAllPixels(const PixRGBA pix)
    virtual bool blit(const PixelBuffer &src, 
        int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight)
    virtual bool blend(const PixelBuffer &src, 
        int srcX, int srcY, int srcWidth, int srcHeight, 
        int destX, int destY, int destWidth, int destHeight, const PixelTransferOp &tOp)

*/



class PixelBufferOp : public Functor
{
public:
    enum Commands {
        NONE,
        SETPIXEL,
        TRANSFERPIXEL,
        SETPIXELS,
        HORIZONTALLINE,
        SETSPAN,
        SETALLPIXELS,
        BLIT,
        BLEND
    };

protected:
    PixelBuffer &fPixelBuffer;
    int fCommand;

public:
    PixelBufferOp(PixelBuffer &pb, int command)
        :fPixelBuffer(pb),
        fCommand(command)
    {}
    
    int getCommand() const { return fCommand;}

    // Must be implemented by base class
    //virtual void operator()() = 0;
};

// Operation to set a single pixel
//    virtual bool setPixel(GRCOORD x, GRCOORD y, const PixRGBA &pix) = 0;
class PBOpSetPixel : public PixelBufferOp
{
    int fX;
    int fY;
    PixRGBA fPix;

public:
    PBOpSetPixel(PixelBuffer &pb, int x, int y, const PixRGBA &pix)
        : PixelBufferOp(pb, PixelBufferOp::SETPIXEL)
        , fX(x)
        , fY(y)
        , fPix(pix)
    {
    }

    void operator()(){
        fPixelBuffer.setPixel(fX, fY, fPix);
    }
};

//    virtual bool setPixel(GRCOORD x, GRCOORD y, const PixRGBA &pix) = 0;
class PBOpTransferPixel : public PixelBufferOp
{
    int fX;
    int fY;
    PixRGBA fPix;

public:
    PBOpTransferPixel(PixelBuffer &pb, int x, int y, const PixRGBA &pix)
        : PixelBufferOp(pb, PixelBufferOp::TRANSFERPIXEL)
        , fX(x)
        , fY(y)
        , fPix(pix)
    {
    }

    void operator()(){
        fPixelBuffer.setPixel(fX, fY, fPix);
    }
};

//    virtual void setPixels(GRCOORD x, GRCOORD y, GRSIZE width, const PixRGBA src)
class PBOpSetPixels : public PixelBufferOp
{
    int fX;
    int fY;
    int fWidth;

    PixRGBA fPix;

public:
    PBOpSetPixels(PixelBuffer &pb, GRCOORD x, GRCOORD y, GRSIZE width, const PixRGBA &pix)
        : PixelBufferOp(pb, PixelBufferOp::SETPIXELS)
        , fX(x)
        , fY(y)
        , fWidth(width)
        , fPix(pix)
    {
    }

    void operator()() {
        fPixelBuffer.setPixels(fX, fY, fWidth, fPix);
    }
};


//    virtual bool setSpan(const GRCOORD x, const GRCOORD y, const GRSIZE width, const PixRGBA * pix)
class PBOpSetSpan : public PixelBufferOp
{
    int fX;
    int fY;
    int fWidth;

    PixRGBA * fPix;

public:
    PBOpSetSpan(PixelBuffer &pb, GRCOORD x, GRCOORD y, GRSIZE width, PixRGBA *pix)
        : PixelBufferOp(pb, PixelBufferOp::SETSPAN)
        , fX(x)
        , fY(y)
        , fWidth(width)
        , fPix(pix)
    {
    }

    void operator()() {
        fPixelBuffer.setSpan(fX, fY, fWidth, fPix);
    }
};

//    virtual bool setAllPixels(const PixRGBA pix)
class PBOpSetAllPixels : public PixelBufferOp
{
    PixRGBA fPix;

public:
    PBOpSetAllPixels(PixelBuffer &pb, const PixRGBA &pix)
        : PixelBufferOp(pb, PixelBufferOp::SETALLPIXELS)
        , fPix(pix)
    {
    }

    void operator()(){
        fPixelBuffer.setAllPixels(fPix);
    }
};

// BLIT
//    virtual bool blit(const PixelBuffer &src, 
//        int srcX, int srcY, int srcWidth, int srcHeight, 
//        int destX, int destY, int destWidth, int destHeight)
class PBOpBlit : public PixelBufferOp
{
    const PixelBuffer &fSource;
    int fsrcX;
    int fsrcY;
    int fsrcWidth;
    int fsrcHeight;
    
    int fdestX;
    int fdestY;
    int fdestWidth;
    int fdestHeight;
    
public:
    PBOpBlit(PixelBuffer &pb, const PixelBuffer &src,
        int srcX, int srcY, int srcWidth, int srcHeight,
        int destX, int destY, int destWidth, int destHeight)
        : PixelBufferOp(pb, PixelBufferOp::BLIT)
        , fSource(src)
        , fsrcX(srcX)
        , fsrcY(srcY)
        , fsrcWidth(srcWidth)
        , fsrcHeight(srcHeight)
        , fdestX(destX)
        , fdestY(destY)
        , fdestWidth(destWidth)
        , fdestHeight(destHeight)
    {
    }

    void operator()(){
        fPixelBuffer.blit(fSource, 
            fsrcX, fsrcY, fsrcWidth, fsrcHeight,
            fdestX, fdestY, fdestWidth, fdestHeight);
    }
};

// BLEND
//    virtual bool blend(const PixelBuffer &src, 
//        int srcX, int srcY, int srcWidth, int srcHeight, 
//        int destX, int destY, int destWidth, int destHeight, const PixelTransferOp &tOp)
class PBOpBlend : public PixelBufferOp
{
    const PixelBuffer &fSource;
    int fsrcX;
    int fsrcY;
    int fsrcWidth;
    int fsrcHeight;
    
    int fdestX;
    int fdestY;
    int fdestWidth;
    int fdestHeight;
    const PixelTransferOp &fTransferOp;

public:
    PBOpBlend(PixelBuffer &pb, const PixelBuffer &src,
        int srcX, int srcY, int srcWidth, int srcHeight,
        int destX, int destY, int destWidth, int destHeight,
        const PixelTransferOp &tOp)
        : PixelBufferOp(pb, PixelBufferOp::BLIT)
        , fSource(src)
        , fsrcX(srcX)
        , fsrcY(srcY)
        , fsrcWidth(srcWidth)
        , fsrcHeight(srcHeight)
        , fdestX(destX)
        , fdestY(destY)
        , fdestWidth(destWidth)
        , fdestHeight(destHeight)
        , fTransferOp(tOp)
    {
    }

    void operator()(){
        fPixelBuffer.blend(fSource, 
            fsrcX, fsrcY, fsrcWidth, fsrcHeight,
            fdestX, fdestY, fdestWidth, fdestHeight,
            fTransferOp);
    }
};
