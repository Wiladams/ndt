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
//#include "Graphics.h"



/*
Global State
*/

static StopWatch SWatch;    // Stopwatch used for time 
int width = 0;
int height = 0;


// Specifying a color using a 32-bit integer
// 0xAARRGGBB
class colors {
public:
    const Color transparent{ 0x00000000 };
    const Color black{ 0xff000000 };
    const Color white{ 0xffffffff };

    // RGB primaries
    const Color red{ 0xffff0000 };
    const Color green{ 0xff00ff00 };
    const Color blue{ 0xff0000ff };

    // CYMK primaries
    const Color cyan{ 0xff00ffff };
    const Color magenta{ 0xffff00ff };
    const Color yellow{ 0xffffff00 };

    // grays
    const Color ltGray{ 0xffc0c0c0 };
    const Color midGray{ 0xff7f7f7f };
    const Color darkGray{ 0xff404040 };


    // other colors
    const Color pink{ 0xffffc0cb };
    const Color darkBlue{ 0xff00007f };
    const Color darkGreen{ 0xff007f00 };
    const Color darkRed{ 0xff7f0000 };
    const Color darkCyan{ 0xff008080 };

    const Color midGreen{ 0xff00C000 };
    const Color midBlue{ 0xff0000C0 };
    const Color midRed{ 0xffC00000 };
    const Color midMagenta{ 0xffC00C0 };
};




// Various modes
void angleMode(int mode)
{
    gAppSurface->angleMode(mode);
}


void ellipseMode(const ELLIPSEMODE mode)
{
    gAppSurface->ellipseMode(mode);
}

void rectMode(const RECTMODE mode)
{
    gAppSurface->rectMode(mode);
}

void blendMode(int mode)
{
    gAppSurface->blendMode(mode);
}

// Set stroke caps for beginning, intermediate, end
void strokeCaps(int caps)
{
    gAppSurface->strokeCaps(caps);
}

void strokeJoin(int join)
{
    gAppSurface->strokeJoin(join);
}

void strokeWeight(int weight)
{
    gAppSurface->strokeWeight(weight);
}


// State management
void push()
{
    gAppSurface->push();
}

void pop()
{
    gAppSurface->pop();
}

// Coordinate Transformation
void translate(double dx, double dy)
{
    gAppSurface->translate(dx, dy);
}

void scale(double sx, double sy)
{
    gAppSurface->scale(sx, sy);
}

void scale(double sxy)
{
    gAppSurface->scale(sxy, sxy);
}


void rotate(double angle, double cx, double cy)
{
    gAppSurface->rotate(angle, cx, cy);
}

void rotate(double angle)
{
    gAppSurface->rotate(angle);
}


// Drawing attributes

// Color parts
int blue(const Color& c)
{
    return c.b;
}

int green(const Color& c)
{
    return c.g;
}

int red(const Color& c)
{
    return c.r;
}

int alpha(const Color& c)
{
    return c.a;
}

// general color construction from components
Color color(int a, int b, int c, int d)
{
    Color pix;
    pix.r = a;
    pix.g = b;
    pix.b = c;
    pix.a = d;

    return pix;
}

Color color(int r, int g, int b)
{
    return color(r, g, b, 255);
}

Color color(int gray, int alpha)
{
    return color(gray, gray, gray, alpha);
}

Color color(int gray)
{
    return color(gray,gray,gray,255);
}


Color lerpColor(const Color&from, const Color&to, double f)
{
    uint8_t r = (uint8_t)lerp(from.r, to.r, f);
    uint8_t g = (uint8_t)lerp(from.g, to.g, f);
    uint8_t b = (uint8_t)lerp(from.b, to.b, f);
    uint8_t a = (uint8_t)lerp(from.a, to.a, f);

    return color((int)r, (int)g, (int)b, (int)a);
}

void fill(const BLGradient& g)
{
    gAppSurface->fill(g);
}

void fill(const Color& pix)
{
    gAppSurface->fill(pix);
}

void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = alpha;
    fill(c);
}

void fill(Color pix, uint8_t alpha)
{
    Color c = pix;
    c.a = alpha;
    fill(c);
}

void fill(uint8_t r, uint8_t g, uint8_t b)
{
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = 255;
    fill(c);
}

void fill(uint8_t gray, uint8_t alpha)
{
    Color c;
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
    gAppSurface->noFill();
}

// Setting Stroke
void stroke(const Color &pix)
{
    gAppSurface->stroke(pix);
}

// Change the alpha of an existing color
void stroke(Color pix, int alpha)
{
    Color c = pix;
    c.a = alpha;
    stroke(c);
}

void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    stroke(Color(r,g,b,alpha));
}

void stroke(uint8_t r, uint8_t g, uint8_t b)
{
    stroke(Color(r, g, b, 255));
}

// create color with gray and alpha
void stroke(uint8_t gray, uint8_t alpha=255)
{
    stroke(Color(gray, gray, gray, alpha));
}

void noStroke()
{
    gAppSurface->noStroke();
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
    gAppSurface->clear();
}

// Background will do a fillAll() to set the background
// to a particular color
void background(const Color &pix)
{
    gAppSurface->background(pix);
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
    gAppSurface->clip(x, y, w, h);
}

void noClip()
{
    gAppSurface->noClip();
}

// Drawing Primitives
// You can set a single pixel, but this is an extremely expensive
// operation.  It would be better to get a pointer to the data
// and set the pixel directly.
void set(double x1, double y1, const Color &c)
{
    gAppSurface->set(x1, y1, c);
}

Color get(double x, double y)
{
    return gAppSurface->get(x, y);
}

// Point uses the stroke color to do the painting
// and honors the strokeWeight
// so, if the weight is <= 1, we can simply set a single pixel
// for anything else, we can draw an ellipse
// using a radius of 1/2 the stroke weight
void point(double x, double y)
{
    gAppSurface->point(x, y);
}

// draw a line using the stroke color
void line(double x1, double y1, double x2, double y2)
{
    gAppSurface->line(x1, y1, x2, y2);
}

// Draw rectangle
// if using fill, then fill first
// if using stroke, then draw outline second
void rect(double x, double y, double width, double height, double xradius, double yradius)
{
    gAppSurface->rect(x, y, width, height, xradius, yradius);
}

void rect(double x, double y, double width, double height)
{
    gAppSurface->rect(x, y, width, height);
}

void ellipse(double a, double b, double c, double d)
{
    gAppSurface->ellipse(a, b, c, d);
}

void circle(double cx, double cy, double diameter)
{
    ellipse(cx, cy, diameter / 2.0, diameter / 2.0);
}

void triangle(double x1, double y1, double x2, double y2, double x3, double y3)
{
    gAppSurface->triangle(x1, y1, x2, y2, x3, y3);
}



//
// Curves
//
void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    gAppSurface->bezier(x1, y1, x2, y2, x2, y3, x4, y4);
}

// 
// Polygons
//
void polyline(const BLPoint* pts, size_t n)
{
    gAppSurface->polyline(pts, n);
}

void polygon(const BLPoint* pts, size_t n)
{
    gAppSurface->polygon(pts, n);
}


void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4)
{
    gAppSurface->quad(x1, y1, x2, y2, x3, y3, x4, y4);
}

void image(BLImage &img, int x, int y)
{
    gAppSurface->image(img, x, y);
}

void scaleImage(BLImage& src, 
    double srcX, double srcY, double srcWidth, double srcHeight, 
    double dstX, double dstY, double dstWidth, double dstHeight)
{
    gAppSurface->scaleImage(src, srcX, srcY, srcWidth, srcHeight,
        dstX, dstY, dstWidth, dstHeight);
}
/*
void scaleImage(BLImage& src, double x, double y, double scaleX, double scaleY)
{
    gAppSurface->scaleImage(src, x, y, scaleX, scaleY);
    double dstWidth = src.width() * scaleX;
    double dstHeight = src.height() * scaleY;

    BLRect dst{ x,y,dstWidth,dstHeight };
    BLRectI srcArea{ 0,0,src.width(),src.height() };

    gAppDC.blitImage(dst, src, srcArea);
}
*/


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


// Text Handling
void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical)
{
    gAppSurface->textAlign(horizontal, vertical);
}

void textFont(const char* fontname)
{
    gAppSurface->textFont(fontname);
}

void textSize(double size)
{
    gAppSurface->textSize(size);
}

void text(const char* txt, double x, double y)
{
    gAppSurface->text(txt, x, y);
}

// Image management
BLImage* createImage(int width, int height)
{
    BLImage* img = new BLImage(width, height, BL_FORMAT_PRGB32);
    return img;
}

// Canvas management

void createCanvas(long aWidth, long aHeight)
{
    width = aWidth;
    height = aHeight;

    setCanvasSize(aWidth, aHeight);

    gAppWindow->setCanvasSize(aWidth, aHeight);
    gAppWindow->show();

    // get current mouse position


    gAppSurface->clear();

    // reset overall timer
    SWatch.reset();
}

void loadPixels()
{
    gAppSurface->loadPixels();
}

void updatePixels()
{
    gAppSurface->updatePixels();
}
