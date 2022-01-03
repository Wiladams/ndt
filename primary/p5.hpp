#pragma once

/*
    This file represents the interface for the P5 application
    environment.

    All the special functions, structures, and constants
    should all be located in this file.

    P5 uses the apphost routines to connect to the local window
    system to display graphics on the screen, and receive mouse
    and keyboard events.

    References
    Colorspace conversions
    http://www.chilliant.com/rgb2hsv.html

*/

#include <stdlib.h>

#include "apphost.h"
#include "gwindow.h"

#include <memory>

#ifdef __cplusplus
extern "C" {
#endif

    // Some generic function signatures
    typedef void (*KeyEventHandler)(const KeyboardEvent& e);
    typedef void (*MouseEventHandler)(const MouseEvent& e);
    typedef void (*JoystickEventHandler)(const JoystickEvent& e);
    typedef void (*TouchEventHandler)(const TouchEvent& e);
    typedef void (*PointerEventHandler)(const PointerEvent& e);
    typedef void (*GestureEventHandler)(const GestureEvent& e);
    typedef void (*FileDropEventHandler)(const FileDropEvent& e);
#ifdef __cplusplus
}
#endif

// Things a p5 based application can implement

#ifdef __cplusplus
extern "C" {
#endif
    APP_EXPORT double getWidth();
    APP_EXPORT double getHeight();

    APP_EXPORT void draw();
    APP_EXPORT void setup();
    APP_EXPORT void update(const double dt);

// IO Event Handlers

// keyboard event processing
    APP_EXPORT void keyPressed(const KeyboardEvent& e);
    APP_EXPORT void keyReleased(const KeyboardEvent& e);
    APP_EXPORT void keyTyped(const KeyboardEvent& e);

// mouse event processing
    APP_EXPORT void mouseClicked(const MouseEvent& e);
    APP_EXPORT void mouseDragged(const MouseEvent& e);
    APP_EXPORT void mouseMoved(const MouseEvent& e);
    APP_EXPORT void mousePressed(const MouseEvent& e);
    APP_EXPORT void mouseReleased(const MouseEvent& e);
    APP_EXPORT void mouseWheel(const MouseEvent& e);

// Handle Joystick events
    APP_EXPORT void joyPressed(const JoystickEvent& e);
    APP_EXPORT void joyReleased(const JoystickEvent& e);
    APP_EXPORT void joyMoved(const JoystickEvent& e);
    APP_EXPORT void joyMovedZ(const JoystickEvent& e);

// Touch Events
    APP_EXPORT void touchStarted(const TouchEvent& e);
    APP_EXPORT void touchEnded(const TouchEvent& e);
    APP_EXPORT void touchMoved(const TouchEvent& e);
    APP_EXPORT void touchHover(const TouchEvent& e);

    // Gesture Events
    APP_EXPORT void panStarted(const GestureEvent& e);
    APP_EXPORT void panMoved(const GestureEvent& e);
    APP_EXPORT void panEnded(const GestureEvent& e);

    APP_EXPORT void zoomStarted(const GestureEvent& e);
    APP_EXPORT void zoomMoved(const GestureEvent& e);
    APP_EXPORT void zoomEnded(const GestureEvent& e);

// Pointer Events

// File Drop events
    APP_EXPORT void fileDrop(const FileDropEvent& e);

#ifdef __cplusplus
}
#endif

namespace p5 {


#ifdef __cplusplus
extern "C" {
#endif
    // Size of the application area, set through
    // createCanvas()
    APP_EXPORT extern int width;
    APP_EXPORT extern int height;

    APP_EXPORT extern int frameCount;
    APP_EXPORT extern int droppedFrames;

    APP_EXPORT extern Pixel* pixels;

    // Keyboard Globals
    APP_EXPORT extern int keyCode;
    APP_EXPORT extern int keyChar;

    // Mouse Globals
    APP_EXPORT extern bool mouseIsPressed;
    APP_EXPORT extern int mouseX;
    APP_EXPORT extern int mouseY;
    APP_EXPORT extern int mouseDelta;
    APP_EXPORT extern int pmouseX;
    APP_EXPORT extern int pmouseY;

    // Gesture Globals
        // Gesture Globals
    APP_EXPORT extern long panX;
    APP_EXPORT extern long panY;
    APP_EXPORT extern long ppanX;
    APP_EXPORT extern long ppanY;
    APP_EXPORT extern long panVecX;
    APP_EXPORT extern long panVecY;

    APP_EXPORT extern long previousZoomDistance;
    APP_EXPORT extern long zoomDistance;

#ifdef __cplusplus
}
#endif





struct PVector {
    double x;
    double y;

    PVector() :x(0), y(0) {}
    PVector(double x, double y) : x(x), y(y) {}


    // Some simple arithmetic
    PVector& add(const PVector& other) {
        return add(other.x, other.y);
    }

    PVector& add(const double x, const double y)
    {
        this->x = this->x + x;
        this->y = this->y + y;

        return *this;
    }


    PVector operator *(const double sxy)
    {
        return { x * sxy, y * sxy };
    }

    PVector& mult(const double x, const double y)
    {
        this->x = this->x * x;
        this->y = this->y * y;

        return *this;
    }
    
    PVector& mult(const PVector& other) {
        return mult(other.x, other.y);
    }

    PVector& mult(const double sxy) {
        return mult(sxy, sxy);
    }

    static PVector mult(const PVector& v, double m)
    {
        PVector res = v;
        return res.mult(m);
    }
};


    // Specifying a color using a 32-bit integer
    // 0xAARRGGBB
    class colors {
    public:
        const Pixel transparent{ 0x00000000 };
        const Pixel black{ 0xff000000 };
        const Pixel white{ 0xffffffff };

        // RGB primaries
        const Pixel red{ 0xffff0000 };
        const Pixel green{ 0xff00ff00 };
        const Pixel blue{ 0xff0000ff };

        // CYMK primaries
        const Pixel cyan{ 0xff00ffff };
        const Pixel magenta{ 0xffff00ff };
        const Pixel yellow{ 0xffffff00 };

        // grays
        const Pixel ltGray{ 0xffc0c0c0 };
        const Pixel midGray{ 0xff7f7f7f };
        const Pixel darkGray{ 0xff404040 };


        // other colors
        const Pixel pink{ 0xffffc0cb };
        const Pixel darkBlue{ 0xff00007f };
        const Pixel darkGreen{ 0xff007f00 };
        const Pixel darkRed{ 0xff7f0000 };
        const Pixel darkCyan{ 0xff008080 };

        const Pixel midGreen{ 0xff00C000 };
        const Pixel midBlue{ 0xff0000C0 };
        const Pixel midRed{ 0xffC00000 };
        const Pixel midMagenta{ 0xffC00C0 };
    };


    // Windowing functions
    void addWindow(std::shared_ptr<GWindow> win);
    std::shared_ptr<GWindow> window(int x, int y, int w, int h);
    void windowToFront(std::shared_ptr<GWindow> win);

    void angleMode(int mode) noexcept;
    void ellipseMode(const ELLIPSEMODE mode) noexcept;
    void rectMode(const RECTMODE mode) noexcept;
    void blendMode(int mode) noexcept;
    void strokeCaps(int caps) noexcept;
    void strokeJoin(int join) noexcept;
    void strokeWeight(double weight) noexcept;

    // clearing command cache
    void flush() noexcept;
    void commandThreshold(int threshold) noexcept;

    void redraw() noexcept;

    // coordinate transform
    void push() noexcept;
    void pop() noexcept;
    void translate(double dx, double dy) noexcept;
    void scale(double sx, double sy) noexcept;
    void scale(double sxy) noexcept;
    void rotate(double angle, double cx, double cy) noexcept;
    void rotate(double angle) noexcept;

    int red(const Pixel& c) noexcept;
    int green(const Pixel& c) noexcept;
    int blue(const Pixel& c) noexcept;
    int alpha(const Pixel& c) noexcept;


    Pixel color(int a, int b, int c, int d) noexcept;
    Pixel color(int r, int g, int b) noexcept;
    Pixel color(int gray, int alpha) noexcept;
    Pixel color(int gray) noexcept;

    Pixel lerpColor(const Pixel& from, const Pixel& to, double f) noexcept;


    void fill(const BLStyle& s) noexcept;
    void fill(const BLGradient& g) noexcept;
    void fill(const Pixel& pix) noexcept;
    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept;
    void fill(Pixel pix, uint8_t alpha) noexcept;
    void fill(uint8_t r, uint8_t g, uint8_t b) noexcept;
    void fill(uint8_t gray, uint8_t alpha) noexcept;
    void fill(uint8_t gray) noexcept;
    void noFill() noexcept;

    void stroke(const BLStyle& s) noexcept;
    void stroke(const Pixel& pix) noexcept;
    void stroke(Pixel pix, int alpha) noexcept;
    void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept;
    void stroke(uint8_t r, uint8_t g, uint8_t b) noexcept;
    void stroke(uint8_t gray, uint8_t alpha = 255) noexcept;
    void noStroke() noexcept;

    void loop() noexcept;
    void noLoop() noexcept;
    void frameRate(int newRate) noexcept;
    int getFrameRate() noexcept;

    void clear() noexcept;
    void clearRect(double x, double y, double w, double h) noexcept;

    void background(const Pixel& pix) noexcept;
    void background(int a, int b, int c, int d) noexcept;
    void background(int a, int b, int c) noexcept;
    void background(int gray, int alpha) noexcept;
    void background(int gray) noexcept;

    void clip(double x, double y, double w, double h) noexcept;
    void noClip() noexcept;


    void set(int x1, int y1, const Pixel& c) noexcept;
    Pixel get(int x, int y) noexcept;

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
    void path(const BLPath& p) noexcept;

    void image(const BLImage& img, int x, int y) noexcept;
    BLImage loadImage(const char* filename) noexcept;

    void scaleImage(const BLImage& src,
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
    void createCanvas(long aWidth, long aHeight, const char *title="Application") noexcept;
    void fullscreen() noexcept;
    bool isFullscreen() noexcept;
    //Surface * createSurface(long aWidth, long aHeight) noexcept;

    void loadPixels() noexcept;
    void updatePixels() noexcept;


    double random() noexcept;
    double random(double low, double high) noexcept;
    double random(double high) noexcept;

    double millis() noexcept;
    double seconds() noexcept;

    // Math routines
    inline double constrain(double x, double low, double high) noexcept { return maths::Clamp(x, low, high); }

    inline double radians(double deg) noexcept {return maths::Radians(deg);}
    inline double degrees(double rad) noexcept {return maths::Degrees(rad);}
    inline double lerp(double start, double stop, double amt) noexcept { return maths::Lerp(amt, start, stop); }
    inline double sq(double x) { return x * x; }
    inline double sqrt(double x) { return std::sqrt(x); }

    // Calculate distance between two points
    inline double dist(double x1, double y1, double x2, double y2)
    {
        return std::sqrt(sq(x2 - x1) + sq(y2 - y1));
    }

    inline double mag(double a, double b) { return sqrt(a*a+ b*b); }
    inline double mag(double a, double b, double c) { return std::sqrt(a * a + b * b + c * c); }

    inline double map(double value, double start1, double stop1, double start2, double stop2, bool withBounds = false)
    {
        if (withBounds)
            value = maths::Clamp(value, start1, stop1);

        return maths::Map(value, start1, stop1, start2, stop2);
    }

    inline double max(double a, double b) { return maths::Max(a, b); }
    inline double min(double a, double b) { return maths::Min(a, b); }

    inline double ceil(double x) { return std::ceil(x); }
    inline double floor(double x) { return std::floor(x); }
    inline long round(double x) { return std::lround(x); }
}