#pragma once

//Binding P5 graphics API to a JSON serializer
// this makes it possible to remote the interface
// Basically, use the encoder instead of a regular surface
// Serialize to some sort of stream, and that's it.


enum {
    GCMD_ANGLEMODE = 1,
GCMD_ELLIPSEMODE,
GCMD_RECTMODE,
GCMD_BLENDMODE,
GCMD_STROKECAPS,
GCMD_STROKEJOIN,
GCMD_STROKEWEIGHT,

GCMD_PUSH,
GCMD_POP,

GCMD_TRANSLATE,
GCMD_SCALE,
GCMD_ROTATE,

GCMD_FILL_COLOR,
GCMD_FILL_GRADIENT,
GCMD_FILL_PATTERN,
GCMD_FILL_NONE,

GCMD_STROKE,
GCMD_NOSTROKE,

GCMD_FLUSH,
GCMD_LOADPIXELS,
GCMD_UPDATEPIXELS,
GCMD_CLEAR,
GCMD_BACKGROUND,
GCMD_CLIP,
GCMD_NOCLIP,

GCMD_SET,
GCMD_POINT,
GCMD_LINE,
GCMD_RECT,
GCMD_ROUNDRECT,
GCMD_ELLIPSE,
GCMD_CIRCLE,
GCMD_TRIANGLE,
GCMD_BEZIER,
GCMD_POLYLINE,
GCMD_POLYGON,
GCMD_QUAD,

GCMD_IMAGE,
GCMD_IMAGE_SCALE,

GCMD_TEXTALIGN,
GCMD_TEXTFONT,
GCMD_TEXTSIZE,
GCMD_TEXT,

GCMD_BEGINSHAPE,
GCMD_VERTEX,
GCMD_ENDSHAPE,
};

#include "Graphics.h"
#include <vector>
#include "binstream.hpp"

class GraphicsEncode : public IGraphics
{
    BinStream &fBS;

private:

    // Create initial state
    void initialize()
    {
        // white fill
        fill(Color(255, 255, 255, 255));

        // black stroke
        stroke(Color(0, 0, 0, 255));

        // Start with a default font so we can start drawing text
        textFont("c:\\windows\\fonts\\segoeui.ttf");
    }

    bool writeShort(int16_t value)
    {
        fBS.writeInt16(value);
        return true;
    }

    bool writeInt(int value)
    {
        fBS.writeInt32(value);
        return true;
    }

    bool writeUInt(uint32_t value)
    {
        fBS.writeUInt32(value);
        return true;
    }
    
    bool writeFloat(float value)
    {
        fBS.writeFloat(value);
        return true;
    }

    bool writeString(const char *value)
    {
        fBS.writeStringZ(value);
        return true;
    }

    bool writeCommand(uint32_t cmd)
    {
        fBS.writeUInt32(cmd);
        return true;
    }

    bool writeEnum(uint32_t cmd, uint32_t value)
    {
        writeCommand(cmd);
        writeUInt(value);
    }

    bool writeCoord(double x, double y)
    {
        writeFloat((float)x);
        writeFloat((float)y);

        return true;
    }

    bool writeRect(double x, double y, double w, double h)
    {
        writeFloat((float)x);
        writeFloat((float)y);
        writeFloat((float)w);
        writeFloat((float)h);

        return true;
    }
    
    bool writeColor(const Color &c)
    {
        writeUInt(c.value);
    }


public:

    GraphicsEncode(BinStream& bs)
        : fBS(bs)
    {
        initialize();
    }



    // Various Modes
    virtual void angleMode(int mode) {writeEnum(GCMD_ANGLEMODE, mode);}
    virtual void ellipseMode(const ELLIPSEMODE mode) {writeEnum(GCMD_ELLIPSEMODE, (uint32_t)mode);}
    virtual void rectMode(const RECTMODE mode) {writeEnum(GCMD_RECTMODE, (uint32_t)mode);}
    virtual void blendMode(int op) {writeEnum(GCMD_BLENDMODE, (uint32_t)op);}

    // stroking attributes
    virtual void strokeCaps(int caps) {writeEnum(GCMD_STROKECAPS, caps);}
    virtual void strokeJoin(int style) { writeEnum(GCMD_STROKEJOIN, style); }
    virtual void strokeWeight(int weight) { writeEnum(GCMD_STROKEWEIGHT, weight); }

    // Attribute State Stack
    virtual void push() { writeCommand(GCMD_PUSH); }
    virtual void pop() { writeCommand(GCMD_POP); }

    // Coordinate transformation
    virtual void translate(double dx, double dy) 
    { 
        writeCommand(GCMD_TRANSLATE);
        writeCoord(dx, dy);
    }
    
    virtual void scale(double sx, double sy) 
    { 
        writeCommand(GCMD_SCALE);
        writeCoord(sx, sy);
    }
    
    virtual void rotate(double angle, double cx, double cy) 
    { 
        writeCommand(GCMD_ROTATE);
        writeFloat((float)angle);
        writeCoord(cx, cy); 
    }



    // Color management
    virtual void fill(const BLGradient& g) 
    { 
        writeCommand(GCMD_FILL_GRADIENT);
        // BUGBUG - serialize gradient
    }
    virtual void fill(const Color& c) 
    { 
        writeCommand(GCMD_FILL_COLOR);
        writeColor(c);
    }
    
    virtual void noFill() { writeCommand(GCMD_FILL_NONE); }

    virtual void stroke(const Color& c) 
    {
        writeCommand(GCMD_STROKE); 
        writeColor(c);
    }

    virtual void noStroke() { writeCommand(GCMD_NOSTROKE); }


    // Synchronization
    virtual void flush()
    {
        writeCommand(GCMD_FLUSH);
    }

    virtual void loadPixels()
    {
        writeCommand(GCMD_LOADPIXELS);
    }

    virtual void updatePixels()
    {
        writeCommand(GCMD_UPDATEPIXELS);
    }

    // Background management
    virtual void clear() 
    {
        writeCommand(GCMD_CLEAR);
    }

    virtual void background(const Color& c)
    {
        writeCommand(GCMD_BACKGROUND);
        writeColor(c);
    }

    // Clipping
    virtual void clip(double x, double y, double w, double h) 
    {
        writeCommand(GCMD_CLIP);
        writeRect(x, y, w, y);
    }
    virtual void noClip() { writeCommand(GCMD_NOCLIP); }

    // Geometry
    // hard set a specfic pixel value
    virtual void set(double x, double y, const Color& c)
    {
        writeCommand(GCMD_SET);
        writeCoord(x, y);
        writeColor(c);
    }
    


    virtual void point(double x, double y) 
    { 
        writeCommand(GCMD_POINT);
        writeCoord(x, y);
    }

    virtual void line(double x1, double y1, double x2, double y2) 
    {
        writeCommand(GCMD_LINE);
        writeCoord(x1, y1);
        writeCoord(x2,y2);
    }

    virtual void rect(double x, double y, double width, double height, double xradius, double yradius)
    {
        writeCommand(GCMD_ROUNDRECT);
        writeRect(x, y, width, height);
        writeCoord(xradius, yradius);
    }

    virtual void rect(double x, double y, double width, double height)
    {
        writeCommand(GCMD_RECT);
        writeRect(x, y, width, height);
    }
    
    virtual void rect(const BLRect& arect)
    {
        writeCommand(GCMD_RECT);
        writeRect(arect.x, arect.y, arect.w, arect.h);
    }

    virtual void ellipse(double a, double b, double c, double d) 
    {
        writeCommand(GCMD_ELLIPSE);
        writeCoord(a,b);
        writeCoord(c,d);
    }

    virtual void circle(double cx, double cy, double diameter) 
    { 
        writeCommand(GCMD_CIRCLE);
        writeCoord(cx, cy);
        writeFloat(diameter);
    }

    virtual void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        writeCommand(GCMD_TRIANGLE);
        writeCoord(x1, y1);
        writeCoord(x2, y2);
        writeCoord(x3, y3);
    }

    virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        writeCommand(GCMD_BEZIER);
        writeCoord(x1, y1);
        writeCoord(x2, y2);
        writeCoord(x3, y3);
        writeCoord(x4, y4);
    }
    
    virtual void polyline(const BLPoint* pts, size_t n)
    {
        writeCommand(GCMD_POLYLINE);
        writeUInt(n);
        for (size_t i=0; i< n; i++) {
            writeCoord(pts[i].x, pts[i].y);
        }
    }
    
    virtual void polygon(const BLPoint* pts, size_t n)
    {
        writeCommand(GCMD_POLYGON);
        writeUInt(n);
        for (size_t i=0; i< n; i++) {
            writeCoord(pts[i].x, pts[i].y);
        }
    }
    
    virtual void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
    {
        writeCommand(GCMD_QUAD);
        writeCoord(x1, y1);
        writeCoord(x2, y2);
        writeCoord(x3, y3);
        writeCoord(x4, y4);
    }

    // Bitmaps
    virtual void image(BLImage& img, int x, int y)
    {
        writeCommand(GCMD_IMAGE);
        writeCoord(x, y);
        // BUGBUG - serialize image efficiently
        // maybe runlength encoding
    }
    
    virtual void scaleImage(BLImage& src,
        double srcX, double srcY, double srcWidth, double srcHeight,
        double dstX, double dstY, double dstWidth, double dstHeight)
    {
        writeCommand(GCMD_IMAGE_SCALE);
        writeCoord(dstX, dstY);
        writeCoord(dstWidth, dstHeight);
        writeCoord(srcX, srcY);
        writeCoord(srcWidth, srcHeight);
        // BUGBUG - serialize image efficiently
        // maybe runlength encoding
    }

    virtual void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical)
    {
        writeCommand(GCMD_TEXTALIGN);
        writeUInt((uint32_t)horizontal);
        writeUInt((uint32_t)vertical);
    }

    virtual void textFont(const char* fontfile)
    {
        writeCommand(GCMD_TEXTFONT);
        writeString(fontfile);
    }
    
    virtual void textSize(double size)
    {
        writeCommand(GCMD_TEXTSIZE);
        writeFloat((float)size);
    }

    void text(const char* txt, double x, double y)
    {
        writeCommand(GCMD_TEXT);
        writeCoord(x, y);
        writeString(txt);
    }

    // Vertex shaping
    void beginShape(SHAPEMODE shapeKind = SHAPEMODE::OPEN)
    {
        writeCommand(GCMD_BEGINSHAPE);
        writeUInt((uint32_t)shapeKind);
    }

    void vertex(double x, double y)
    {
        writeCommand(GCMD_VERTEX);
        writeCoord(x, y);
    }

    // endKind - SHAPEEND
    void endShape(SHAPEEND endKind)
    {
        writeCommand(GCMD_ENDSHAPE);
        writeUInt((uint32_t)endKind);
    }
};
