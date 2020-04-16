#pragma once

/*
    This file represents the interface for the P5 application
    environment.

    All the special functions, structures, and constants
    should all be located in this file.

    P5 uses the apphost routines to connect to the local window
    system to display graphics on the screen, and receive mouse
    and keyboard events.
*/

#include <stdlib.h>

#include "apphost.h"
#include "maths.hpp"
#include "stopwatch.hpp"
#include "random.hpp"

// RectMode, EllipseMode
enum {
    CORNER,
    CORNERS,
    CENTER,
    RADIUS
};

// are angles specified in degrees
// or in radians
enum {
    DEGREES,
    RADIANS
};



// Specifying a color using a 32-bit integer
// 0xAARRGGBB
class colors {
public:
    const BLRgba32 transparent{0x00000000};
    const BLRgba32 black{0xff000000};
    const BLRgba32 white{ 0xffffffff };

    // RGB primaries
    const BLRgba32 red           {0xffff0000};
    const BLRgba32 green         {0xff00ff00};
    const BLRgba32 blue          {0xff0000ff};

    // CYMK primaries
    const BLRgba32 cyan          {0xff00ffff};
    const BLRgba32 magenta       {0xffff00ff};
    const BLRgba32 yellow        {0xffffff00};

    // grays
    const BLRgba32 ltGray        {0xffc0c0c0};
    const BLRgba32 midGray       {0xff7f7f7f};
    const BLRgba32 darkGray      {0xff404040};


    // other colors
    const BLRgba32 pink          {0xffffc0cb};
    const BLRgba32 darkBlue      {0xff00007f};
    const BLRgba32 darkGreen     {0xff007f00};
    const BLRgba32 darkRed       {0xff7f0000};
    const BLRgba32 darkCyan      {0xff008080};

    const BLRgba32 midGreen{0xff00C000};
    const BLRgba32 midBlue{0xff0000C0};
    const BLRgba32 midRed{0xffC00000};
    const BLRgba32 midMagenta{0xffC00C0};
} colors;


/*
Global State
*/
bool gUseStroke = true;
bool gUseFill = true;


static size_t width = 0;
static size_t height = 0;

//BLRgba32 gBackgroundColor;
//BLRgba32 gStrokeColor;
//BLRgba32 gFillColor;

static StopWatch SWatch;


void blendMode(int mode)
{
    // set compositing operator
    gAppDC.setCompOp(mode);
}

// Set stroke caps for beginning, intermediate, end
void strokeCaps(int caps)
{
    gAppDC.setStrokeCaps(caps);
}

void strokeJoin(int join)
{
    gAppDC.setStrokeJoin(join);
}

void strokeWeight(int weight)
{
    gAppDC.setStrokeWidth(weight);
}


// State management
void push()
{
    gAppDC.save();
}

void pop()
{
    gAppDC.restore();
}

// Coordinate Transformation
void translate(double dx, double dy)
{
    gAppDC.translate(dx, dy);
}

void scale(double sx, double sy)
{
    gAppDC.scale(sx, sy);
}

void scale(double sxy)
{
    scale(sxy, sxy);
}


void rotate(double rads, double cx, double cy)
{
    gAppDC.rotate(rads, cx, cy);
}

void rotate(double rads)
{
    rotate(rads, 0, 0);
}


// Drawing attributes

// Color parts
int blue(const BLRgba32& c)
{
    return c.b;
}

int green(const BLRgba32& c)
{
    return c.g;
}

int red(const BLRgba32& c)
{
    return c.r;
}

int alpha(const BLRgba32& c)
{
    return c.a;
}

// general color construction from components
BLRgba32 color(int a, int b, int c, int d)
{
    //printf("color: %d %d %d %d\n", a, b, c, d);
    BLRgba32 pix;
    pix.r = a;
    pix.g = b;
    pix.b = c;
    pix.a = d;

    return pix;
}

BLRgba32 color(int r, int g, int b)
{
    return color(r, g, b, 255);
}

BLRgba32 color(int gray, int alpha)
{
    return color(gray, gray, gray, alpha);
}

BLRgba32 color(int gray)
{
    return color(gray,gray,gray,255);
}


BLRgba32 lerpColor(const BLRgba32 &from, const BLRgba32 &to, double f)
{
    uint8_t r = (uint8_t)lerp(from.r, to.r, f);
    uint8_t g = (uint8_t)lerp(from.g, to.g, f);
    uint8_t b = (uint8_t)lerp(from.b, to.b, f);
    uint8_t a = (uint8_t)lerp(from.a, to.a, f);

    return color((int)r, (int)g, (int)b, (int)a);
}


void fill(BLRgba32 pix)
{
    gUseFill = true;
    gAppDC.setFillStyle(pix);
}

void fill(BLRgba32 pix, uint8_t alpha)
{
    BLRgba32 c = pix;
    c.a = alpha;
    fill(c);
}

void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    BLRgba32 c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = alpha;
    fill(c);
}

void fill(uint8_t r, uint8_t g, uint8_t b)
{
    BLRgba32 c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = 255;
    fill(c);
}

void fill(uint8_t gray, uint8_t alpha)
{
    BLRgba32 c;
    c.r = gray;
    c.g = gray;
    c.b = gray;
    c.a = alpha;
    fill(c);
}

void fill(uint8_t gray)
{
    fill(gray, 255);
}

void noFill()
{
    gUseFill = false;
}

// Setting Stroke
void stroke(BLRgba32 pix)
{
    gUseStroke = true;
    gAppDC.setStrokeStyle(pix);
}

// Change the alpha of an existing color
void stroke(BLRgba32 pix, int alpha)
{
    BLRgba32 c = pix;
    c.a = alpha;
    stroke(c);
}

void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    BLRgba32 c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = alpha;
    stroke(c);
}

void stroke(uint8_t r, uint8_t g, uint8_t b)
{
    BLRgba32 c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = 255;
    stroke(c);
}

// create color with gray and alpha
void stroke(uint8_t gray, uint8_t alpha=255)
{
    BLRgba32 c;
    c.r = gray;
    c.g = gray;
    c.b = gray;
    c.a = alpha;
    stroke(c);
}

void noStroke()
{
    gUseStroke = false;
}


inline void frameRate(int newRate)
{
    setFrameRate(newRate);
}

inline double seconds()
{
    return SWatch.seconds();
}

inline double millis()
{
    // get millis from p5 stopwatch
    return SWatch.millis();
}

// Clearing is a complete wipe, which will leave 
// the canvas with fully transparent black
// The other state information is saved
void clear()
{
    gAppDC.save();
    gAppDC.clearAll();
    gAppDC.restore();
}

// Background will do a fillAll() to set the background
// to a particular color
void background(BLRgba32 pix)
{
    gAppDC.save();
    gAppDC.setFillStyle(pix);
    gAppDC.fillAll();
    gAppDC.restore();
}

void background(int a, int b, int c, int d)
{
    background(color(a, b, c, d));
}

void background(int gray, int alpha)
{
    background(color(gray, alpha));
}

void background(int gray)
{
    background(color(gray));
}

void clip(double x, double y, double w, double h)
{
    gAppDC.clipToRect(x, y, w, h);
}

void noClip()
{
    gAppDC.restoreClipping();
}

// Drawing Primitives
// You can set a single pixel, but this is an extremely expensive
// operation.  It would be better to get a pointer to the data
// and set the pixel directly.
void set(double x1, double y1, BLRgba32 c)
{
    // fill a rectangle of size 1
    gAppDC.save();
    gAppDC.setFillStyle(c);
    gAppDC.fillRect(x1, y1, 1, 1);
    gAppDC.restore();
}

// draw a line using the stroke color
void line(double x1, double y1, double x2, double y2)
{
    if (!gUseStroke) 
    { return ;}

    gAppDC.strokeLine(x1, y1, x2, y2);
}

// Draw rectangle
// if using fill, then fill first
// if using stroke, then draw outline second
void rect(double x, double y, double width, double height)
{
    if (gUseFill) {
        gAppDC.fillRect(x, y, width, height);
    }

    if (gUseStroke) {
        gAppDC.strokeRect(x, y, width, height);
    }
}

void ellipse(double cx, double cy, double xRadius, double yRadius)
{
    if (gUseFill) {
        gAppDC.fillEllipse(cx, cy, xRadius, yRadius);
    }

    if (gUseStroke) {
        gAppDC.strokeEllipse(cx, cy, xRadius, yRadius);
    }
}

void circle(double cx, double cy, double diameter)
{
    ellipse(cx, cy, diameter / 2.0, diameter / 2.0);
}

void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
    BLTriangle tri = {x1,y1, x1,y2,x3,y3};

    if (gUseFill) {
        gAppDC.fillGeometry(BL_GEOMETRY_TYPE_TRIANGLE, &tri);
    }

    if (gUseStroke) {
        gAppDC.strokeGeometry(BL_GEOMETRY_TYPE_TRIANGLE, &tri);
    }
}



//
// Curves
//
void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    BLPath path;
    path.moveTo(x1, y1);
    path.cubicTo(x2, y2, x2, y3, x4, y4);
    gAppDC.strokePath(path);
}

// 
// Polygons
//
void polyline(const BLPoint* pts, size_t n)
{
    gAppDC.strokePolyline(pts, n);
}

void polygon(const BLPoint* pts, size_t n)
{
    if (gUseFill) {
        gAppDC.fillPolygon(pts, n);
    }

    if (gUseStroke) {
        gAppDC.strokePolygon(pts, n);
    }
}


void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    BLPoint pts[] = { {x1,y1},{x2,y2},{x3,y3},{x4,y4} };
    polygon(pts, 4);
}

void image(BLImage &img, int x, int y)
{
    gAppDC.blitImage(BLPointI(x, y), img);
}

void scaleImage(BLImage& src, 
    double srcX, double srcY, double srcWidth, double srcHeight, 
    double dstX, double dstY, double dstWidth, double dstHeight)
{
    BLRect dst{dstX,dstY,dstWidth,dstHeight };
    BLRectI srcArea{ (int)srcX,(int)srcY,(int)srcWidth,(int)srcHeight };

    gAppDC.blitImage(dst, src, srcArea);
}

void scaleImage(BLImage& src, double x, double y, double scaleX, double scaleY)
{
    double dstWidth = src.width() * scaleX;
    double dstHeight = src.height() * scaleY;

    BLRect dst{ x,y,dstWidth,dstHeight };
    BLRectI srcArea{ 0,0,src.width(),src.height() };

    gAppDC.blitImage(dst, src, srcArea);
}

static int gAngleMode = DEGREES;
void angleMode(int newMode)
{
    gAngleMode = newMode;
}

// Random number generator
TausPRNG mRandomNumberGenerator(5);

inline double random()
{
    return mRandomNumberGenerator.next();
}

inline double random(double low, double high)
{
    return mRandomNumberGenerator.next(low, high);
}

inline double random(double high)
{
    return mRandomNumberGenerator.next(0, high);
}

#define randomLowHigh random
#define randomHigh random



// Image management
BLImage* createImage(int width, int height)
{
    BLImage* img = new BLImage(width, height, BL_FORMAT_PRGB32);
    return img;
}

// Canvas management

void createCanvas(GRCOORD aWidth, GRCOORD aHeight)
{
    width = aWidth;
    height = aHeight;

    setCanvasSize(aWidth, aHeight);

    gAppWindow->setCanvasSize(aWidth, aHeight);
    gAppWindow->show();

    // get current mouse position


    gAppDC.clearAll();

    // reset overall timer
    SWatch.reset();
}


