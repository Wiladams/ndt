#include "p5.hpp"
#include "sampler.hpp"
#include "stopwatch.hpp"
#include "windowmgr.h"
#include "ticktopic.h"

#include <iostream>

static VOIDROUTINE gPreloadHandler = nullptr;
static VOIDROUTINE gSetupHandler = nullptr;
static PFNDOUBLE1 gUpdateHandler = nullptr;

static MouseEventHandler gMouseEventHandler = nullptr;
static MouseEventHandler gMouseMovedHandler = nullptr;
static MouseEventHandler gMouseClickedHandler = nullptr;
static MouseEventHandler gMousePressedHandler = nullptr;
static MouseEventHandler gMouseReleasedHandler = nullptr;
static MouseEventHandler gMouseWheelHandler = nullptr;
static MouseEventHandler gMouseHWheelHandler = nullptr;
static MouseEventHandler gMouseDraggedHandler = nullptr;

// Keyboard event handling
static KeyEventHandler gKeyPressedHandler = nullptr;
static KeyEventHandler gKeyReleasedHandler = nullptr;
static KeyEventHandler gKeyTypedHandler = nullptr;

// Joystick event handling
static JoystickEventHandler gJoystickPressedHandler = nullptr;
static JoystickEventHandler gJoystickReleasedHandler = nullptr;
static JoystickEventHandler gJoystickMovedHandler = nullptr;
static JoystickEventHandler gJoystickMovedZHandler = nullptr;

// File dropping
static FileDropEventHandler gFileDroppedHandler = nullptr;

// Touch event handling
static TouchEventHandler gTouchStartedHandler = nullptr;
static TouchEventHandler gTouchEndedHandler = nullptr;
static TouchEventHandler gTouchMovedHandler = nullptr;
static TouchEventHandler gTouchHoverHandler = nullptr;

// Gesture Event handling
static GestureEventHandler gPanStartedHandler = nullptr;
static GestureEventHandler gPanMovedHandler = nullptr;
static GestureEventHandler gPanEndedHandler = nullptr;

static GestureEventHandler gZoomStartedHandler = nullptr;
static GestureEventHandler gZoomMovedHandler = nullptr;
static GestureEventHandler gZoomEndedHandler = nullptr;


// Pointer Event Handling - Pen
static PointerEventHandler gPointerHandler = nullptr;


static std::shared_ptr<WindowManager> gWindowManager;
static VOIDROUTINE gDrawHandler = nullptr;
static VOIDROUTINE gComposedHandler = nullptr;

float gFPS = 15;   // Frames per second
TickTopic gTickTopic;
bool gLooping = true;
bool gIsFullscreen = false;

namespace p5 {

    
    int canvasWidth = 0;              // width of the canvas
    int canvasHeight = 0;             // height of the canvas

    int frameCount = 0;         // how many frames drawn so far
    int droppedFrames = 0;

    int textLineSize = 16;
    int textCursorX = 0;
    int textCursorY = 0;

    // Keyboard globals
    int keyCode = 0;
    int keyChar = 0;

    // Mouse Globals
    bool mouseIsPressed = false;
    int mouseX = 0;
    int mouseY = 0;
    int mouseDelta = 0;
    int pmouseX = 0;
    int pmouseY = 0;

    // Gesture Globals
    long panX = 0;
    long panY = 0;
    long ppanX = 0;
    long ppanY = 0;
    long panVecX = 0;
    long panVecY = 0;

    long zoomDistance = 0;
    long previousZoomDistance = 0;
    double zoomFactor = 1.0;

    uint8_t* pixels = nullptr;    // a pointer to the raw pixels of app canvas


    static StopWatch SWatch;    // Stopwatch used for time 
    double deltaTime = 0;
    double gAppLastTime = 0;


    void setUnitsPerInch(double units)
    {
        gAppSurface->setPpiUnits(systemPpi, units);
    }

    // Window management
    void addWindow(std::shared_ptr<GWindow> win)
    {
        if (nullptr != gWindowManager)
            gWindowManager->addChild(win);
    }

    std::shared_ptr<GWindow> window(const int x, const int y, const int w, const int h)
    {
        std::shared_ptr<GWindow> win = std::make_shared<GWindow>(x, y, w, h);
        addWindow(win);

        return win;
    }

    void windowLayout(std::shared_ptr<ILayoutGraphics> & layout)
    {
        gWindowManager->setLayout(layout);
    }

    void windowToFront(std::shared_ptr<GWindow> win)
    {
        if (nullptr != gWindowManager)
            gWindowManager->moveToFront(win);
    }

    double seconds() noexcept
    {
        return SWatch.seconds();
    }

    double millis() noexcept
    {
        // get millis from p5 stopwatch
        return SWatch.millis();
    }

    // Various modes
    void angleMode(const ANGLEMODE mode) noexcept
    {
        gAppSurface->angleMode(mode);
    }


    void ellipseMode(const ELLIPSEMODE mode) noexcept
    {
        gAppSurface->ellipseMode(mode);
    }

    void rectMode(const RECTMODE mode) noexcept
    {
        gAppSurface->rectMode(mode);
    }

    void blendMode(int mode) noexcept
    {
        gAppSurface->blendMode(mode);
    }

    // Set stroke caps for beginning, intermediate, end
    void strokeCaps(int caps) noexcept
    {
        gAppSurface->strokeCaps(caps);
    }

    void strokeJoin(int join) noexcept
    {
        gAppSurface->strokeJoin(join);
    }

    void strokeWeight(REAL weight) noexcept
    {
        gAppSurface->strokeWeight(weight);
    }

    void redraw() noexcept
    {
        screenRefresh();
    }



    // State management
    void push() noexcept
    {
        gAppSurface->push();
    }

    void pop() noexcept
    {
        gAppSurface->pop();
    }

    // Coordinate Transformation
    void translate(REAL dx, REAL dy) noexcept
    {
        gAppSurface->translate(dx, dy);
    }

    void scale(REAL sx, REAL sy) noexcept
    {
        gAppSurface->scale(sx, sy);
    }

    void scale(REAL sxy) noexcept
    {
        gAppSurface->scale(sxy, sxy);
    }


    void rotate(REAL angle, REAL cx, REAL cy) noexcept
    {
        gAppSurface->rotate(angle, cx, cy);
    }

    void rotate(REAL angle) noexcept
    {
        gAppSurface->rotate(angle,0,0);
    }


    // Drawing attributes

    // Pixel parts
    int blue(const Pixel& c) noexcept
    {
        return c.b();
    }

    int green(const Pixel& c) noexcept
    {
        return c.g();
    }

    int red(const Pixel& c) noexcept
    {
        return c.r();
    }

    int alpha(const Pixel& c) noexcept
    {
        return c.a();
    }

    // general color construction from components
    Pixel color(int a, int b, int c, int d) noexcept
    {
        Pixel pix(a,b,c,d);
        //pix.r = a;
        //pix.g = b;
        //pix.b = c;
        //pix.a = d;

        return pix;
    }

    Pixel color(int r, int g, int b) noexcept
    {
        return color(r, g, b, 255);
    }

    Pixel color(int gray, int alpha) noexcept
    {
        return color(gray, gray, gray, alpha);
    }

    Pixel color(int gray) noexcept
    {
        return color(gray, gray, gray, 255);
    }


    Pixel lerpColor(const Pixel& from, const Pixel& to, REAL f) noexcept
    {
        uint8_t r = (uint8_t)maths::lerp(from.r(), to.r(),f);
        uint8_t g = (uint8_t)maths::lerp(from.g(), to.g(),f);
        uint8_t b = (uint8_t)maths::lerp(from.b(), to.b(),f);
        uint8_t a = (uint8_t)maths::lerp(from.a(), to.a(),f);

        return color((int)r, (int)g, (int)b, (int)a);
    }


    void fill(const BLVar& s) noexcept
    {
        gAppSurface->fill(s);
    }

    void fill(const BLGradientCore& g) noexcept
    {
        gAppSurface->fill(g);
    }

    void fill(const Pixel& pix) noexcept
    {
        gAppSurface->fill(pix);
    }

    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept
    {
        Pixel c(r,g,b,alpha);
        //c.r = r;
        //c.g = g;
        //c.b = b;
        //c.a = alpha;
        fill(c);
    }

    void fill(Pixel pix, uint8_t alpha) noexcept
    {
        Pixel c = pix;
        c.setA(alpha);
        fill(c);
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) noexcept
    {
        Pixel c(r,g,b,255);
        fill(c);
    }

    void fill(uint8_t gray, uint8_t alpha) noexcept
    {
        Pixel c(gray, gray, gray, alpha);
        fill(c);
    }

    void fill(uint8_t gray) noexcept
    {
        fill(gray, 255);
    }

    void noFill() noexcept
    {
        gAppSurface->noFill();
    }


    // Setting Stroke
    void stroke(const Pixel& pix) noexcept
    {
        gAppSurface->stroke(pix);
    }

    void stroke(const BLGradient& g) noexcept
    {
        gAppSurface->stroke(g);
    }

    // Change the alpha of an existing color
    void stroke(const BLVar& s) noexcept
    {
        gAppSurface->stroke(s);
    }

    void stroke(Pixel pix, int alpha) noexcept
    {
        Pixel c = pix;
        c.setA(alpha);
        stroke(c);
    }

    void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept
    {
        stroke(Pixel(r, g, b, alpha));
    }

    void stroke(uint8_t r, uint8_t g, uint8_t b) noexcept
    {
        stroke(Pixel(r, g, b, 255));
    }

    // create color with gray and alpha
    void stroke(uint8_t gray, uint8_t alpha) noexcept
    {
        stroke(Pixel(gray, gray, gray, alpha));
    }

    void stroke(uint8_t gray) noexcept
    {
        stroke(Pixel(gray, gray, gray, 255));
    }

    void noStroke() noexcept
    {
        gAppSurface->noStroke();
    }



    void frameRate(float newRate) noexcept
    {
        gFPS = newRate;
        gTickTopic.setFrequency(newRate);
    }

    float getFrameRate() noexcept
    {
        return gFPS;
    }

    // turn looping on
    void loop() noexcept
    {
        gLooping = true;
        gTickTopic.start();
    }

    // turn looping off
    // we still need to process the windows events
    // but we stop calling draw() on a timer
    void noLoop() noexcept
    {
        gLooping = false;
        gTickTopic.pause();
    }

    // Clearing is a complete wipe, which will leave 
    // the canvas with fully transparent black
    // The other state information is saved
    void clear() noexcept
    {
        gAppSurface->clear();
    }

    void clearRect(double x, double y, double w, double h) noexcept
    {
        gAppSurface->clearRect(x, y, w, h);
    }

    // Background will do a fillAll() to set the background
    // to a particular color
    void background(const Pixel& pix) noexcept
    {
        gAppSurface->background(pix);
    }

    void background(int a, int b, int c, int d) noexcept
    {
        background(color(a, b, c, d));
    }

    void background(int a, int b, int c) noexcept
    {
        background(color(a, b, c, 255));
    }

    void background(int gray, int alpha) noexcept
    {
        background(color(gray, alpha));
    }

    void background(int gray) noexcept
    {
        background(color(gray));
    }


    // Clipping
    void clip(double x, double y, double w, double h) noexcept
    {
        gAppSurface->clip(x, y, w, h);
    }

    void noClip() noexcept
    {
        gAppSurface->noClip();
    }


    // Drawing Primitives
    // You can set a single pixel, but this is an extremely expensive
    // operation.  It would be better to get a pointer to the data
    // and set the pixel directly.
    void set(int x1, int y1, const Pixel& c) noexcept
    {
        gAppSurface->set(x1, y1, c);
    }

    Pixel get(int x, int y) noexcept
    {
        return gAppSurface->get(x, y);
    }


    // Point uses the stroke color to do the painting
    // and honors the strokeWeight
    // so, if the weight is <= 1, we can simply set a single pixel
    // for anything else, we can draw an ellipse
    // using a radius of 1/2 the stroke weight
    void point(double x, double y) noexcept
    {
        gAppSurface->point(x, y);
    }

    // draw a line using the stroke color
    void line(REAL x1, REAL y1, REAL x2, REAL y2) noexcept
    {
        gAppSurface->line(x1, y1, x2, y2);
    }

    // Draw rectangle
    // if using fill, then fill first
    // if using stroke, then draw outline second
    void rect(REAL x, REAL y, REAL width, REAL height, REAL xradius, REAL yradius) noexcept
    {
        gAppSurface->rect(x, y, width, height, xradius, yradius);
    }

    void rect(REAL x, REAL y, REAL width, REAL height) noexcept
    {
        gAppSurface->rect(x, y, width, height);
    }

    void ellipse(REAL a, REAL b, REAL c, REAL d) noexcept
    {
        gAppSurface->ellipse(a, b, c, d);
    }

    void circle(REAL cx, REAL cy, REAL diameter) noexcept
    {
        ellipse(cx, cy, diameter / 2.0f, diameter / 2.0f);
    }

    void triangle(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3) noexcept
    {
        gAppSurface->triangle(x1, y1, x2, y2, x3, y3);
    }

    void quad(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4) noexcept
    {
        gAppSurface->quad(x1, y1, x2, y2, x3, y3, x4, y4);
    }


    //
    // Curves
    //
    void bezier(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL x4, REAL y4) noexcept
    {
        gAppSurface->bezier(x1, y1, x2, y2, x3, y3, x4, y4);
    }

    // 
    // Polygons
    //
    void polyline(const BLPoint* pts, size_t n) noexcept
    {
        gAppSurface->polyline(pts, n);
    }

    void polygon(const BLPoint* pts, size_t n) noexcept
    {
        gAppSurface->polygon(pts, n);
    }

    // Paths
    void path(const BLPath& p) noexcept
    {
        gAppSurface->path(p);
    }

    void image(const BLImage& img, int x, int y) noexcept
    {
        gAppSurface->image(img, x, y);
    }

    void scaleImage(const BLImage& src,
        REAL srcX, REAL srcY, REAL srcWidth, REAL srcHeight,
        REAL dstX, REAL dstY, REAL dstWidth, REAL dstHeight) noexcept
    {
        gAppSurface->scaleImage(src, srcX, srcY, srcWidth, srcHeight,
            dstX, dstY, dstWidth, dstHeight);
    }
    /*
    void scaleImage(const BLImage& src, double x, double y, double scaleX, double scaleY)
    {
        gAppSurface->scaleImage(src, x, y, scaleX, scaleY);
        double dstWidth = src.width() * scaleX;
        double dstHeight = src.height() * scaleY;

        BLRect dst{ x,y,dstWidth,dstHeight };
        BLRectI srcArea{ 0,0,src.width(),src.height() };

        gAppDC.blitImage(dst, src, srcArea);
    }
    */

    BLImage loadImage(const char* filename) noexcept
    {
        BLImage img;
        auto err = img.readFromFile(filename);

        if (err)
            printf("could not load pot of gold (%d)\n", err);
        
        return img;
    }

    // Text Handling
    void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical) noexcept
    {
        gAppSurface->textAlign(horizontal, vertical);
    }

    void textFace(const BLFontFace& face) noexcept
    {
        gAppSurface->textFace(face);
    }

    void textFont(const char* fontname) noexcept
    {
        gAppSurface->textFont(fontname);
    }

    void textSize(REAL size) noexcept
    {
        gAppSurface->textSize(size);
    }

    void text(const char* txt, REAL x, REAL y) noexcept
    {
        gAppSurface->text(txt, x, y);
    }

    void text(REAL x, REAL y, const char* format, ...) noexcept
    {
        va_list args = nullptr;
        va_start(args, format);

        gAppSurface->vartext(x, y, format, args);

        va_end(args);
    }

    maths::vec2f textMeasure(const char* txt) noexcept
    {
        return gAppSurface->textMeasure(txt);
    }

    REAL textWidth(const char* txt) noexcept
    {
        auto pt = textMeasure(txt);
        return pt.x;
    }

    void beginShape(SHAPEMODE shapeKind) noexcept
    {
        gAppSurface->beginShape(shapeKind);
    }

    void vertex(REAL x, REAL y) noexcept
    {
        gAppSurface->vertex(x, y);
    }

    void endShape(SHAPEEND endKind) noexcept
    {
        gAppSurface->endShape(endKind);
    }


    // Image management
    BLImage* createImage(int width, int height) noexcept
    {
        BLImage* img = new BLImage(width, height, BL_FORMAT_PRGB32);
        return img;
    }

    // Canvas management

    void createCanvas(long aWidth, long aHeight, const char *title) noexcept
    {
        createAppWindow(aWidth, aHeight, title);

        gWindowManager = std::make_shared<WindowManager>(aWidth, aHeight);
    }

    void fullscreen() noexcept
    {

        createCanvas(displayWidth, displayHeight);
        setCanvasPosition(0, 0);
        layered();
        gIsFullscreen = true;
    }

    bool isFullscreen() noexcept
    {
        return gIsFullscreen;
    }

    void flush() noexcept
    {
        gAppSurface->flush();
    }

    void commandThreshold(int threshold) noexcept
    {
        gAppSurface->setCommandThreshold(threshold);
    }

    void loadPixels() noexcept
    {
        gAppSurface->loadPixels();
        pixels = (uint8_t *)gAppSurface->getPixels();
    }

    void updatePixels() noexcept
    {
        gAppSurface->updatePixels();
        pixels = nullptr;
    }



    // Random number generator
    maths::rng_state gRNGState{};

    void randomSeed(uint64_t s)noexcept
    {
        gRNGState = maths::make_rng(s);
    }

    float random(float low, float high) noexcept
    {
        auto d = maths::rand1f(gRNGState);

        d = (d * (high - low + 1.0f)) + low;

        return d;
    }

    float random() noexcept
    {
        return maths::rand1f(gRNGState);
    }

    float random(float high) noexcept
    {
        auto d = maths::rand1f(gRNGState);

        d = (d * (high + 1.0f));

        return d;
    }
};  // namespace p5

//
// The following routines have to be located outside
// the namespace, or the application framework won't be
// able to find them with getProcAddress()
//

// This is called per frame
// giving us an opportunity to do composition
// before final display
void handleComposition()
{
    // Allow the user's drawing to happen first
    // This could be captured in drawBackground/drawForeground/draw
    if (gDrawHandler != nullptr) {
        p5::push();
        gDrawHandler();
        p5::pop();
    }

    // If we have a window manager, tell it to draw
    if (nullptr != gWindowManager) {
        gWindowManager->draw(*gAppSurface);
    }

    gAppSurface->flush();

    if (gComposedHandler != nullptr)
    {
        gComposedHandler();
    }
}


void handleFrameTick(const double secs)
{
    p5::frameCount = p5::frameCount + 1;
    
    // Do whatever drawing the application wants to do
    handleComposition();
    
    // Push our drawing to the screen
    screenRefresh();
}


void handleKeyboardEvent(const KeyboardEvent& e)
{
    //std::cout << "keyboardEvent: " << e.activity << "\n" ;

    switch (e.activity) {


    case KEYPRESSED:
        p5::keyCode = e.keyCode;
        if (gKeyPressedHandler) {
            gKeyPressedHandler(e);
        }
        break;
    case KEYRELEASED:
        p5::keyCode = e.keyCode;
        if (gKeyReleasedHandler) {
            gKeyReleasedHandler(e);
        }
        break;
    case KEYTYPED:
        p5::keyChar = e.keyCode;
        if (gKeyTypedHandler) {
            gKeyTypedHandler(e);
        }
        break;
    }
}

void handleMouseEvent(const MouseEvent& e)
{
    //printf("p5::handleMouseEvent: %d (%d, %d)\n", e.activity, e.x, e.y);
    // assign new mouse position
    p5::pmouseX = p5::mouseX;
    p5::pmouseY = p5::mouseY;
    p5::mouseX = e.x;
    p5::mouseY = e.y;
    p5::mouseIsPressed = e.lbutton || e.rbutton || e.mbutton;
    //printf("  mouseX,Y: %d, %d\n", p5::mouseX, p5::mouseY);
    
    // If there is a window manager, let it have first crack
    // at the mouse event.
    if (nullptr != gWindowManager) {
        gWindowManager->mouseEvent(e);
    }
    
    // If the user has implemented explicit mouse handling routines
    // send the event there.
    switch (e.activity) {


    case MOUSEMOVED:

        if (gMouseMovedHandler != nullptr) {
            gMouseMovedHandler(e);
        }

        if (p5::mouseIsPressed && gMouseDraggedHandler) {
            gMouseDraggedHandler(e);
        }
        break;

    case MOUSEPRESSED:
        if (gMousePressedHandler != nullptr) {
            gMousePressedHandler(e);
        }
        break;
    case MOUSERELEASED:
        if (gMouseReleasedHandler != nullptr) {
            gMouseReleasedHandler(e);
        }
        if (gMouseClickedHandler != nullptr) {
            gMouseClickedHandler(e);
        }
        break;

    case MOUSEWHEEL:
        p5::mouseDelta = e.delta;
        if (gMouseWheelHandler != nullptr) {
            gMouseWheelHandler(e);
        }
        break;

    case MOUSEHWHEEL:
        p5::mouseDelta = e.delta;
        if (gMouseHWheelHandler != nullptr) {
            gMouseHWheelHandler(e);
        }
        break;
    }
    

}

void handleJoystickEvent(const JoystickEvent& e)
{

    // We can handle up to two joysticks using
    // this mechanism
    switch (e.activity) {
    case JOYPRESSED:
        if (gJoystickPressedHandler)
            gJoystickPressedHandler(e);
        break;

    case JOYRELEASED:
        if (gJoystickReleasedHandler != nullptr)
            gJoystickReleasedHandler(e);
        break;

    case JOYMOVED:
        if (gJoystickMovedHandler)
            gJoystickMovedHandler(e);
        break;

    case JOYZMOVED:
        if (gJoystickMovedZHandler)
            gJoystickMovedZHandler(e);
        break;
    }
}

void handleFileDroppedEvent(const FileDropEvent& e)
{
    if (nullptr != gWindowManager) {
        gWindowManager->fileDropped(e);
    }

    // do the file drop handling
    if (nullptr != gFileDroppedHandler)
        gFileDroppedHandler(e);
}

// Handle a singular touch event
// dispatch to user provided functions
void handleTouchEvent(const TouchEvent& e)
{
    switch (e.activity)
    {
        // switch based on activity
        case TOUCH_DOWN: {
        if (gTouchStartedHandler)
            gTouchStartedHandler(e);
        }
        break;

        case TOUCH_UP: {
            if (gTouchEndedHandler)
                gTouchEndedHandler(e);
        }
        break;

        case TOUCH_MOVE: {
            if (gTouchMovedHandler)
                gTouchMovedHandler(e);
            }
        break;

    //case TOUCH_HOVER: {
    //    if (gTouchHoverHandler)
    //        gTouchHoverHandler(e);
    //}
    //                break;
    }

}

void handleGestureEvent(const GestureEvent& e)
{
    //printf("handleGestureEvent: %d (%d,%d) isBegin: %d, isEnd: %d, isInertia: %d \n", e.activity, e.x, e.y, e.isBegin, e.isEnd, e.isInertia);
    p5::panVecX = e.x - p5::ppanX;
    p5::panVecY = e.y - p5::ppanY;
    
    switch (e.activity) {
        case GESTURE_PAN: {
            if (e.isBegin) {
                if (gPanStartedHandler)
                    gPanStartedHandler(e);
            }
            else if (e.isEnd) {
                // compare distances, save delta
                if (gPanEndedHandler)
                    gPanEndedHandler(e);

                p5::panVecX = 0;
                p5::panVecY = 0;
            }
            else {
                // Intermediate movement
                if (gPanMovedHandler)
                    gPanMovedHandler(e);
            }
        }
        break;

        case GESTURE_ZOOM: {
            if (e.isBegin) {
                if (gZoomStartedHandler)
                    gZoomStartedHandler(e);
            }
            else if (e.isEnd) {
                // compare distances, save delta
                if (gZoomEndedHandler)
                    gZoomEndedHandler(e);

                p5::panVecX = 0;
                p5::panVecY = 0;
            }
            else {
                // Intermediate movement
                if (gZoomMovedHandler)
                    gZoomMovedHandler(e);
            }
        }
        break;
    }

    // Reset previous and current pan values
    p5::ppanX = p5::panX;
    p5::ppanY = p5::panY;

    p5::panX = e.x;
    p5::panY = e.y;
}

void handlePointerEvent(const PointerEvent& e)
{
    std::cout << "p5::handlePointerEvent" << std::endl;
}


static void p5frameTickSubscriber(const double t)
{
    //droppedFrames = p.droppedFrames();

    handleFrameTick(t);
}

static void p5keyboardSubscriber(const KeyboardEvent& e)
{
    handleKeyboardEvent(e);
}

static void p5mouseSubscriber(const MouseEvent& e)
{
    handleMouseEvent(e);
}

static void p5joystickSubscriber(const JoystickEvent& e)
{
    handleJoystickEvent(e);
}

static void p5touchSubscriber(const TouchEvent& e)
{
    handleTouchEvent(e);
}

static void p5gestureSubscriber(const GestureEvent& e)
{
    handleGestureEvent(e);
}

static void p5pointerSubscriber(const PointerEvent& e)
{
    handlePointerEvent(e);
}

static void p5filedroppedSubscriber(const FileDropEvent& e)
{
    handleFileDroppedEvent(e);
}

void onUnload()
{
    gTickTopic.stop();
}




void onLoad()
{

    // setup the drawing context
    // ppi and user units
    gAppSurface->setPpiUnits(systemPpi, systemPpi);     // default to raw pixels

    HMODULE hInst = ::GetModuleHandleA(NULL);

    // Setup initial frequency
    gTickTopic.setFrequency(gFPS);

    // setup subscriptions
    gTickTopic.subscribe(p5frameTickSubscriber);
    subscribe(p5mouseSubscriber);
    subscribe(p5keyboardSubscriber);
    subscribe(p5joystickSubscriber);
    subscribe(p5filedroppedSubscriber);
    subscribe(p5pointerSubscriber);
    subscribe(p5touchSubscriber);
    subscribe(p5gestureSubscriber);

    // load the setup() function if user specified
    gPreloadHandler = (VOIDROUTINE)GetProcAddress(hInst, "preload");
    gSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "setup");
    gUpdateHandler = (PFNDOUBLE1)GetProcAddress(hInst, "update");

    // Look for implementation of drawing handler
    gDrawHandler = (VOIDROUTINE)GetProcAddress(hInst, "draw");
    gComposedHandler = (VOIDROUTINE)GetProcAddress(hInst, "onComposed");

    // Look for implementation of mouse events
    //gMouseEventHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseEvent");
    gMouseMovedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseMoved");
    gMouseClickedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseClicked");
    gMousePressedHandler = (MouseEventHandler)GetProcAddress(hInst, "mousePressed");
    gMouseReleasedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseReleased");
    gMouseWheelHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseWheel");
    gMouseHWheelHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseHWheel");
    gMouseDraggedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseDragged");

    // Look for implementation of keyboard events
    gKeyPressedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyPressed");
    gKeyReleasedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyReleased");
    gKeyTypedHandler = (KeyEventHandler)GetProcAddress(hInst, "keyTyped");

    // Look for implementation of joystick events
    gJoystickPressedHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyPressed");
    gJoystickReleasedHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyReleased");
    gJoystickMovedHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyMoved");
    gJoystickMovedZHandler = (JoystickEventHandler)GetProcAddress(hInst, "joyMovedZ");

    // File dropping
    gFileDroppedHandler = (FileDropEventHandler)GetProcAddress(hInst, "fileDrop");

    // Touch event routines
    gTouchStartedHandler = (TouchEventHandler)GetProcAddress(hInst, "touchStarted");
    gTouchEndedHandler = (TouchEventHandler)GetProcAddress(hInst, "touchEnded");
    gTouchMovedHandler = (TouchEventHandler)GetProcAddress(hInst, "touchMoved");
    gTouchHoverHandler = (TouchEventHandler)GetProcAddress(hInst, "touchHovered");

    // Gesture Events
    gPanStartedHandler = (GestureEventHandler)GetProcAddress(hInst, "panStarted");
    gPanMovedHandler = (GestureEventHandler)GetProcAddress(hInst, "panMoved");
    gPanEndedHandler = (GestureEventHandler)GetProcAddress(hInst, "panEnded");

    gZoomStartedHandler = (GestureEventHandler)GetProcAddress(hInst, "zoomStarted");
    gZoomMovedHandler = (GestureEventHandler)GetProcAddress(hInst, "zoomMoved");
    gZoomEndedHandler = (GestureEventHandler)GetProcAddress(hInst, "zoomEnded");

    gPointerHandler = (PointerEventHandler)GetProcAddress(hInst, "pointerStarted");

    // call a preload routine, if there's anything before setup
    if (gPreloadHandler != nullptr)
        gPreloadHandler();

    // Call a setup routine if the user specified one
    if (gSetupHandler != nullptr) {
        gSetupHandler();
    }

    // do drawing at least once in case
    // the user calls noLoop() within 
    // the setup() routine
    handleFrameTick(0);

    // Start ticking if the user hasn't called noLoop()
    if (gLooping)
        gTickTopic.start();
}
