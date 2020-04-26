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


namespace p5 {


#ifdef __cplusplus
extern "C" {
#endif
    EXPORT extern int width;
    EXPORT extern int height;
#ifdef __cplusplus
}
#endif

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



    void angleMode(int mode) noexcept;
    void ellipseMode(const ELLIPSEMODE mode) noexcept;
    void rectMode(const RECTMODE mode) noexcept;
    void blendMode(int mode) noexcept;
    void strokeCaps(int caps) noexcept;
    void strokeJoin(int join) noexcept;
    void strokeWeight(int weight) noexcept;


    void push() noexcept;
    void pop() noexcept;
    void translate(double dx, double dy) noexcept;
    void scale(double sx, double sy) noexcept;
    void scale(double sxy) noexcept;
    void rotate(double angle, double cx, double cy) noexcept;
    void rotate(double angle) noexcept;

    int red(const Color& c) noexcept;
    int green(const Color& c) noexcept;
    int blue(const Color& c) noexcept;
    int alpha(const Color& c) noexcept;


    Color color(int a, int b, int c, int d) noexcept;
    Color color(int r, int g, int b) noexcept;
    Color color(int gray, int alpha) noexcept;
    Color color(int gray) noexcept;

    Color lerpColor(const Color& from, const Color& to, double f) noexcept;


    void fill(const BLGradient& g) noexcept;
    void fill(const Color& pix) noexcept;
    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept;
    void fill(Color pix, uint8_t alpha) noexcept;
    void fill(uint8_t r, uint8_t g, uint8_t b) noexcept;
    void fill(uint8_t gray, uint8_t alpha) noexcept;
    void fill(uint8_t gray) noexcept;
    void noFill() noexcept;


    void stroke(const Color& pix) noexcept;
    void stroke(Color pix, int alpha) noexcept;
    void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept;
    void stroke(uint8_t r, uint8_t g, uint8_t b) noexcept;
    void stroke(uint8_t gray, uint8_t alpha = 255) noexcept;
    void noStroke() noexcept;

    void frameRate(int newRate) noexcept;


    void clear() noexcept;
    void background(const Color& pix) noexcept;
    void background(int a, int b, int c, int d) noexcept;
    void background(int gray, int alpha) noexcept;
    void background(int gray) noexcept;

    void clip(double x, double y, double w, double h) noexcept;
    void noClip() noexcept;


    void set(double x1, double y1, const Color& c) noexcept;
    Color get(double x, double y) noexcept;

    void point(double x, double y) noexcept;
    void line(double x1, double y1, double x2, double y2) noexcept;
    void rect(double x, double y, double width, double height, double xradius, double yradius) noexcept;
    void rect(double x, double y, double width, double height) noexcept;
    void ellipse(double a, double b, double c, double d) noexcept;
    void circle(double cx, double cy, double diameter) noexcept;
    void triangle(double x1, double y1, double x2, double y2, double x3, double y3) noexcept;
    void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) noexcept;


    void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) noexcept;
    void polyline(const BLPoint* pts, size_t n) noexcept;
    void polygon(const BLPoint* pts, size_t n) noexcept;


    void image(BLImage& img, int x, int y) noexcept;


    void scaleImage(BLImage& src,
        double srcX, double srcY, double srcWidth, double srcHeight,
        double dstX, double dstY, double dstWidth, double dstHeight) noexcept;



    void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical) noexcept;
    void textFont(const char* fontname) noexcept;
    void textSize(double size) noexcept;
    void text(const char* txt, double x, double y) noexcept;


    void beginShape(SHAPEMODE shapeKind = SHAPEMODE::OPEN) noexcept;
    void vertex(double x, double y) noexcept;
    void endShape(SHAPEEND endKind = SHAPEEND::STROKE) noexcept;


    BLImage* createImage(int width, int height) noexcept;
    void createCanvas(long aWidth, long aHeight) noexcept;
    Surface * createSurface(long aWidth, long aHeight) noexcept;

    void loadPixels() noexcept;
    void updatePixels() noexcept;


    double random() noexcept;
    double random(double low, double high) noexcept;
    double random(double high) noexcept;

    double millis() noexcept;
    double seconds() noexcept;
}