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

#include "apphost.hpp"
#include "grtypes.hpp"
#include "maths.hpp"
#include "Image.hpp"
#include "PixelOps.hpp"
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
    const PixRGBA transparent   = {0x00000000};
    const PixRGBA black         = {0xff000000};
    const PixRGBA white         = {0xffffffff};

    // RGB primaries
    const PixRGBA red           = {0xffff0000};
    const PixRGBA green         = {0xff00ff00};
    const PixRGBA blue          = {0xff0000ff};

    // CYMK primaries
    const PixRGBA cyan          = {0xff00ffff};
    const PixRGBA magenta       = {0xffff00ff};
    const PixRGBA yellow        = {0xffffff00};

    // grays
    const PixRGBA ltGray        = {0xffc0c0c0};
    const PixRGBA midGray       = {0xff7f7f7f};
    const PixRGBA darkGray      = {0xff404040};

    // other colors
    const PixRGBA pink          = {0xffffc0cb};
    const PixRGBA darkBlue      = {0xff00007f};
    const PixRGBA darkGreen     = {0xff007f00};
    const PixRGBA darkRed       = {0xff7f0000};
    const PixRGBA darkCyan      = {0xff008080};
} colors;

/*
local Colorrefs = {
    MidGreen = color(0,192,0);
    MidBlue = color(0,0,192);
    MidRed = color(0xC0, 0,0);
    MidMagenta = color(0xC0, 0, 0xC0);
}
*/

/*
Global State
*/
bool gUseStroke = true;
bool gUseFill = true;
static StopWatch SWatch;

static size_t width = 0;
static size_t height = 0;

BLRgba32 gBackgroundColor;
BLRgba32 gStrokeColor;
BLRgba32 gFillColor;


// Drawing attributes
// general color construction from components
BLRgba32 color(int a, int b, int c, int d)
{
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

// Clearing the canvas
void clear()
{
    gAppDC.save();
    gAppDC.clearAll();
    gAppDC.restore();
}

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

// Drawing Primitives
void set(int x1, int y1, PixRGBA c)
{
    // fill a rectangle of size 1
    gAppDC.fillRect(x1, y1, 1, 1);
}

void line(int x1, int y1, int x2, int y2)
{
    if (!gUseStroke) 
    { return ;}

    gAppDC.strokeLine(x1, y1, x2, y2);
}

void rect(int x, int y, int width, int height)
{
    if (gUseFill) {
        gAppDC.fillRect(x, y, width, height);
    }

    if (gUseStroke) {
        gAppDC.strokeRect(x, y, width, height);
    }
}

void triangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
    if (gUseFill) {
        // fill a triangle
    }

    if (gUseStroke) {
        //gAppDC->strokeTriangle(x1, y1, x2, y2, x3, y3);
    }
}

void ellipse(int cx, int cy, int xRadius, int yRadius)
{
    if (gUseFill) {
        gAppDC.fillEllipse(cx, cy, xRadius, yRadius);
    }

    if (gUseStroke) {
        gAppDC.strokeEllipse(cx, cy, xRadius, yRadius);
    }
}

void circle(int cx, int cy, int diameter)
{
    ellipse(cx, cy, diameter/2, diameter/2);
}

void image(BLImage &img, int x, int y)
{
    gAppDC.blitImage(BLPointI(x, y), img);
}








static int gAngleMode = DEGREES;
void angleMode(int newMode)
{
    gAngleMode = newMode;
}


// Image management
BLImage * createImage(size_t width, size_t height)
{
    BLImage *img = new BLImage(width, height, BL_FORMAT_PRGB32);
    return img;
}

// Canvas management

void createCanvas(size_t aWidth, size_t aHeight)
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

void redraw(int nTimes=1)
{
    for (int i=1;i<=nTimes;i++) {
        forceRedraw();
    }
}

void frameRate(int newRate)
{
    setFrameRate(newRate);
}

double seconds()
{
    return SWatch.seconds();
}

double millis()
{
    // get millis from p5 stopwatch
    return SWatch.millis();
}