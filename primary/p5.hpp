

#pragma once

/*
    This file represents the interface for a P5 inspired
    application interface.



    All the special functions, data structures, and constants
    should be located in this, and the matching p5.cpp file.

    P5 uses the apphost routines to connect to the local window
    system to display graphics on the screen, and receive mouse
    keyboard, joystick and touch events.

    References
    Colorspace conversions
    https://p5js.org/
    http://www.chilliant.com/rgb2hsv.html

*/

#include <stdlib.h>

#include "apphost.h"
#include "gwindow.h"
#include "Surface.h"

#include <memory>

using REAL = float;

APP_EXPORT extern std::shared_ptr<Surface> gAppSurface;

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

    APP_EXPORT void preload();
    APP_EXPORT void setup();
    APP_EXPORT void draw();
    APP_EXPORT void onComposed();
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
    APP_EXPORT void mouseHWheel(const MouseEvent& e);

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
    // Constants available in the p5 namespace
    APP_EXPORT extern int frameCount;
    APP_EXPORT extern int droppedFrames;

    APP_EXPORT extern uint8_t* pixels;    // a pointer to the canvas pixels

    // Text console
    APP_EXPORT extern int textLineSize;
    APP_EXPORT extern int textCursorX;
    APP_EXPORT extern int textCursorY;

    // Keyboard Globals
    APP_EXPORT extern int keyCode;      // virtual keycode
    APP_EXPORT extern int keyChar;      // actual character typed

    // Mouse Globals
 //   APP_EXPORT extern bool mouseIsPressed;
 //   APP_EXPORT extern int mouseX;
 //   APP_EXPORT extern int mouseY;
 //   APP_EXPORT extern int mouseDelta;
 //   APP_EXPORT extern int pmouseX;
 //   APP_EXPORT extern int pmouseY;

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
    REAL x;
    REAL y;

    PVector() :x(0), y(0) {}
    PVector(REAL x, REAL y) : x(x), y(y) {}


    // Some simple arithmetic
    PVector& add(const PVector& other) {
        return add(other.x, other.y);
    }

    PVector& add(const REAL x, const REAL y)
    {
        this->x = this->x + x;
        this->y = this->y + y;

        return *this;
    }


    PVector operator *(const REAL sxy)
    {
        return { x * sxy, y * sxy };
    }

    PVector& mult(const REAL x, const REAL y)
    {
        this->x = this->x * x;
        this->y = this->y * y;

        return *this;
    }
    
    PVector& mult(const PVector& other) {
        return mult(other.x, other.y);
    }

    PVector& mult(const REAL sxy) {
        return mult(sxy, sxy);
    }

    static PVector mult(const PVector& v, REAL m)
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

    // Environment attributes
    void setFontDpiUnits(const int dpi, const float units);
    void setDpiUnits(const int dpi, const float units);
    void setUnitsPerInch(REAL units);

    // Windowing functions
    void addWindow(std::shared_ptr<GWindow> win);
    std::shared_ptr<GWindow> window(int x, int y, int w, int h);
    void windowToFront(std::shared_ptr<GWindow> win);
    void windowLayout(std::shared_ptr<ILayoutGraphics>& layout);

    void angleMode(const ANGLEMODE mode) noexcept;
    void ellipseMode(const ELLIPSEMODE mode) noexcept;
    void rectMode(const RECTMODE mode) noexcept;
    void blendMode(int mode) noexcept;
    void strokeCaps(int caps) noexcept;
    void strokeJoin(int join) noexcept;
    void strokeWeight(REAL weight) noexcept;

    // clearing command cache
    void flush() noexcept;
    void commandThreshold(int threshold) noexcept;

    void redraw() noexcept;

    // Runtime management
    void loop() noexcept;
    void noLoop() noexcept;
    void frameRate(float newRate) noexcept;
    float getFrameRate() noexcept;
    size_t getFrameCount() noexcept;

    // coordinate transform
    void push() noexcept;
    void pop() noexcept;
    void translate(REAL dx, REAL dy) noexcept;
    void scale(REAL sx, REAL sy) noexcept;
    void scale(REAL sxy) noexcept;
    void rotate(REAL angle, REAL cx, REAL cy) noexcept;
    void rotate(REAL angle) noexcept;

    // Getting color components
    int red(const Pixel& c) noexcept;
    int green(const Pixel& c) noexcept;
    int blue(const Pixel& c) noexcept;
    int alpha(const Pixel& c) noexcept;


    // Creating a color object
    Pixel color(int a, int b, int c, int d) noexcept;
    Pixel color(int r, int g, int b) noexcept;
    Pixel color(int gray, int alpha) noexcept;
    Pixel color(int gray) noexcept;

    // Create a color through liner interpolation
    Pixel lerpColor(const Pixel& from, const Pixel& to, REAL f) noexcept;

    // Set the fill color for drawing
    void fill(const BLVar& s) noexcept;
    void fill(const BLGradient& g) noexcept;
    void fill(const Pixel& pix) noexcept;
    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept;
    void fill(Pixel pix, uint8_t alpha) noexcept;
    void fill(uint8_t r, uint8_t g, uint8_t b) noexcept;
    void fill(uint8_t gray, uint8_t alpha) noexcept;
    void fill(uint8_t gray) noexcept;

    // Set the stroke color for drawing
    void stroke(const BLVar& s) noexcept;
    void stroke(const BLGradient& g) noexcept;
    void stroke(const Pixel& pix) noexcept;
    void stroke(Pixel pix, int alpha) noexcept;
    void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept;
    void stroke(uint8_t r, uint8_t g, uint8_t b) noexcept;
    void stroke(uint8_t gray, uint8_t alpha = 255) noexcept;

    // Turn off fill and stroke
    void noFill() noexcept;
    void noStroke() noexcept;


    // Clear the entire canvas to transparent
    void clear() noexcept;
    void clearRect(REAL x, REAL y, REAL w, REAL h) noexcept;

    // Set a background for the canvas
    void background(const Pixel& pix) noexcept;
    void background(int a, int b, int c, int d) noexcept;
    void background(int a, int b, int c) noexcept;
    void background(int gray, int alpha) noexcept;
    void background(int gray) noexcept;
    
    // Set a clipping rectangle
    void clip(REAL x, REAL y, REAL w, REAL h) noexcept;
    
    // Turn off the clipping rectangle
    void noClip() noexcept;

    // Set a single pixel
    // does not observe point size
    // does boundary checking
    void set(int x1, int y1, const Pixel& c) noexcept;
    // get the value of a single pixel
    Pixel get(int x, int y) noexcept;

    // Draw a point.  This will value the point size
    void point(REAL x, REAL y) noexcept;

    // Draw an arbitrary line
    // constrain to boundary and clip area
    void line(REAL x1, REAL y1, REAL x2, REAL y2) noexcept;


    // draw rectangle with rounded corners
    // honors rectangle mode
    void rect(REAL x, REAL y, REAL width, REAL height, REAL xradius, REAL yradius) noexcept;

    // Draw a rectangle with sharp corners
    // honors rectangle mode
    void rect(REAL x, REAL y, REAL width, REAL height) noexcept;


    void square(REAL x, REAL y, REAL side) noexcept;

    // Draw an ellipse
    // honors the ellipse mode
    void ellipse(REAL a, REAL b, REAL c, REAL d) noexcept;

    // Draw a circle with a diameter
    void circle(REAL cx, REAL cy, REAL diameter) noexcept;

    // Draw a triangle
    // do clipping
    void triangle(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3) noexcept;
    void quad(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4) noexcept;

    // Draw a bezier line
    void bezier(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4) noexcept;
    
    // Draw multiple lines
    void polyline(const BLPoint* pts, size_t n) noexcept;

    // Fill and stroke a polygon
    void polygon(const BLPoint* pts, size_t n) noexcept;

    // Fill and stroke an arbitrary path
    void path(const BLPath& p) noexcept;

    // Blit an image
    void image(const BLImage& img, int x, int y) noexcept;


    // Draw a sub-image while scaling
    void scaleImage(const BLImage& src,
        REAL srcX, REAL srcY, REAL srcWidth, REAL srcHeight,
        REAL dstX, REAL dstY, REAL dstWidth, REAL dstHeight) noexcept;

    // Careate an image from a filename
    BLImage loadImage(const char* filename) noexcept;
    BLImage* createImage(int width, int height) noexcept;

    // drawing text
    void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical) noexcept;
    void textFace(const BLFontFace& face) noexcept;
    void textFont(const char* fontname) noexcept;
    void textSize(REAL size) noexcept;
    void text(const char* txt, REAL x, REAL y) noexcept;
    void text(REAL x, REAL y, const char* format, ...) noexcept;
    REAL textWidth(const char* txt) noexcept;
    maths::vec2f textMeasure(const char* txt) noexcept;
    REAL textAscent() noexcept;
    REAL textDescent() noexcept;

    void beginShape(SHAPEMODE shapeKind = SHAPEMODE::OPEN) noexcept;
    void vertex(REAL x, REAL y) noexcept;
    void endShape(SHAPEEND endKind = SHAPEEND::STROKE) noexcept;



    void createCanvas(long aWidth, long aHeight, const char *title="p5 Application") noexcept;
    void fullscreen() noexcept;
    bool isFullscreen() noexcept;
    //Surface * createSurface(long aWidth, long aHeight) noexcept;

    void loadPixels() noexcept;
    void updatePixels() noexcept;

    // create random numbers
    void randomSeed(uint64_t s)noexcept;
    float random() noexcept;
    float random(float low, float high) noexcept;
    float random(float high) noexcept;

    // get fractions of seconds
    double millis() noexcept;
    double seconds() noexcept;

    // Math routines
    //
    inline REAL constrain(REAL x, REAL low, REAL high) noexcept { return maths::clamp(x, low, high); }

    // convert degrees to radians
    inline REAL radians(REAL deg) noexcept {return maths::radians(deg);}

    // convert radians to degrees
    inline REAL degrees(REAL rad) noexcept {return maths::degrees(rad);}
    inline REAL lerp(REAL start, REAL stop, REAL amt) noexcept { return maths::lerp(start, stop, amt); }

    // Return the square of a number
    inline REAL sq(REAL x) { return x * x; }

    // Return the square root of a number
    inline REAL sqrt(REAL x) { return std::sqrt(x); }

    // Calculate distance between two points
    inline REAL dist(REAL x1, REAL y1, REAL x2, REAL y2){return sqrt(sq(x2 - x1) + sq(y2 - y1));}

    inline REAL mag(REAL a, REAL b) { return sqrt(a*a+ b*b); }
    inline REAL mag(REAL a, REAL b, REAL c) { return sqrt(a * a + b * b + c * c); }

    inline REAL map(REAL value, REAL start1, REAL stop1, REAL start2, REAL stop2, bool withBounds = false)
    {
        REAL value_ = value;

        if (withBounds)
            value_ = maths::clamp(value, start1, stop1);

        return maths::map(value_ , start1, stop1, start2, stop2);
    }

    inline REAL max(REAL a, REAL b) { return maths::max(a, b); }
    inline REAL min(REAL a, REAL b) { return maths::min(a, b); }

    inline REAL ceil(REAL x) { return std::ceil(x); }
    inline REAL floor(REAL x) { return std::floor(x); }
    inline long round(REAL x) { return std::lround(x); }
}


