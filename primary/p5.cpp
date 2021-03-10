#include "p5.hpp"
#include "random.hpp"
#include "stopwatch.hpp"
#include "windowmgr.h"
#include "ticktopic.h"

#include <iostream>

static VOIDROUTINE gSetupHandler = nullptr;
static PFNDOUBLE1 gUpdateHandler = nullptr;

static MouseEventHandler gMouseEventHandler = nullptr;
static MouseEventHandler gMouseMovedHandler = nullptr;
static MouseEventHandler gMouseClickedHandler = nullptr;
static MouseEventHandler gMousePressedHandler = nullptr;
static MouseEventHandler gMouseReleasedHandler = nullptr;
static MouseEventHandler gMouseWheelHandler = nullptr;
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

int gFPS = 15;   // Frames per second
TickTopic gTickTopic;
bool gLooping = true;
bool gIsFullscreen = false;

namespace p5 {

    int width = 0;              // width of the canvas
    int height = 0;             // height of the canvas

    int frameCount = 0;         // how many frames drawn so far
    int droppedFrames = 0;

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

    Pixel* pixels = nullptr;    // a pointer to the raw pixels of app canvas


    static StopWatch SWatch;    // Stopwatch used for time 
    double deltaTime = 0;
    double gAppLastTime = 0;

    // Window management
    void addWindow(std::shared_ptr<GWindow> win)
    {
        if (nullptr != gWindowManager)
            gWindowManager->addChild(win);
    }

    std::shared_ptr<GWindow> window(int x, int y, int w, int h)
    {
        auto win = std::make_shared<GWindow>(x, y, w, h);
        addWindow(win);

        return win;
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
    void angleMode(int mode) noexcept
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

    void strokeWeight(double weight) noexcept
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
    void translate(double dx, double dy) noexcept
    {
        gAppSurface->translate(dx, dy);
    }

    void scale(double sx, double sy) noexcept
    {
        gAppSurface->scale(sx, sy);
    }

    void scale(double sxy) noexcept
    {
        gAppSurface->scale(sxy, sxy);
    }


    void rotate(double angle, double cx, double cy) noexcept
    {
        gAppSurface->rotate(angle, cx, cy);
    }

    void rotate(double angle) noexcept
    {
        gAppSurface->rotate(angle);
    }


    // Drawing attributes

    // Pixel parts
    int blue(const Pixel& c) noexcept
    {
        return c.b;
    }

    int green(const Pixel& c) noexcept
    {
        return c.g;
    }

    int red(const Pixel& c) noexcept
    {
        return c.r;
    }

    int alpha(const Pixel& c) noexcept
    {
        return c.a;
    }

    // general color construction from components
    Pixel color(int a, int b, int c, int d) noexcept
    {
        Pixel pix;
        pix.r = a;
        pix.g = b;
        pix.b = c;
        pix.a = d;

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


    Pixel lerpColor(const Pixel& from, const Pixel& to, double f) noexcept
    {
        uint8_t r = (uint8_t)maths::Lerp(f, from.r, to.r);
        uint8_t g = (uint8_t)maths::Lerp(f, from.g, to.g);
        uint8_t b = (uint8_t)maths::Lerp(f, from.b, to.b);
        uint8_t a = (uint8_t)maths::Lerp(f, from.a, to.a);

        return color((int)r, (int)g, (int)b, (int)a);
    }



    void fill(const BLGradient& g) noexcept
    {
        gAppSurface->fill(g);
    }

    void fill(const Pixel& pix) noexcept
    {
        gAppSurface->fill(pix);
    }

    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept
    {
        Pixel c;
        c.r = r;
        c.g = g;
        c.b = b;
        c.a = alpha;
        fill(c);
    }

    void fill(Pixel pix, uint8_t alpha) noexcept
    {
        Pixel c = pix;
        c.a = alpha;
        fill(c);
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) noexcept
    {
        Pixel c;
        c.r = r;
        c.g = g;
        c.b = b;
        c.a = 255;
        fill(c);
    }

    void fill(uint8_t gray, uint8_t alpha) noexcept
    {
        Pixel c;
        c.r = gray;
        c.g = gray;
        c.b = gray;
        c.a = alpha;
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

    // Change the alpha of an existing color
    void stroke(Pixel pix, int alpha) noexcept
    {
        Pixel c = pix;
        c.a = alpha;
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

    void frameRate(int newRate) noexcept
    {
        gFPS = newRate;
        gTickTopic.setFrequency(newRate);
    }

    int getFrameRate() noexcept
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
    void line(double x1, double y1, double x2, double y2) noexcept
    {
        gAppSurface->line(x1, y1, x2, y2);
    }

    // Draw rectangle
    // if using fill, then fill first
    // if using stroke, then draw outline second
    void rect(double x, double y, double width, double height, double xradius, double yradius) noexcept
    {
        gAppSurface->rect(x, y, width, height, xradius, yradius);
    }

    void rect(double x, double y, double width, double height) noexcept
    {
        gAppSurface->rect(x, y, width, height);
    }

    void ellipse(double a, double b, double c, double d) noexcept
    {
        gAppSurface->ellipse(a, b, c, d);
    }

    void circle(double cx, double cy, double diameter) noexcept
    {
        ellipse(cx, cy, diameter / 2.0, diameter / 2.0);
    }

    void triangle(double x1, double y1, double x2, double y2, double x3, double y3) noexcept
    {
        gAppSurface->triangle(x1, y1, x2, y2, x3, y3);
    }

    void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) noexcept
    {
        gAppSurface->quad(x1, y1, x2, y2, x3, y3, x4, y4);
    }


    //
    // Curves
    //
    void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) noexcept
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
        double srcX, double srcY, double srcWidth, double srcHeight,
        double dstX, double dstY, double dstWidth, double dstHeight) noexcept
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

    void textFont(const char* fontname) noexcept
    {
        gAppSurface->textFont(fontname);
    }

    void textSize(double size) noexcept
    {
        gAppSurface->textSize(size);
    }

    void text(const char* txt, double x, double y) noexcept
    {
        gAppSurface->text(txt, x, y);
    }


    void beginShape(SHAPEMODE shapeKind) noexcept
    {
        gAppSurface->beginShape(shapeKind);
    }

    void vertex(double x, double y) noexcept
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
        width = aWidth;
        height = aHeight;

        createAppWindow(aWidth, aHeight, "p5 Window");
        //showAppWindow();

        gWindowManager = std::make_shared<WindowManager>(aWidth, aHeight);
    }

    void fullscreen() noexcept
    {

        createCanvas(displayWidth, displayHeight);
        setWindowPosition(0, 0);
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
        pixels = gAppSurface->getData();
    }

    void updatePixels() noexcept
    {
        gAppSurface->updatePixels();
    }



    // Random number generator
    static TausPRNG mRandomNumberGenerator(5);
    double random(double low, double high) noexcept
    {
        return mRandomNumberGenerator.next(low, high);
    }

    double random() noexcept
    {
        return mRandomNumberGenerator.next();
    }

    double random(double high) noexcept
    {
        return mRandomNumberGenerator.next(0, high);
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
        gDrawHandler();
    }

    // If we have a window manager, tell it to draw
    if (nullptr != gWindowManager) {
        gWindowManager->draw(gAppSurface);
    }

    gAppSurface->flush();
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


static void p5frameTickSubscriber(const Topic<double>&p, const double t)
{
    //droppedFrames = p.droppedFrames();

    handleFrameTick(t);
}

static void p5keyboardSubscriber(const KeyboardEventTopic& p, const KeyboardEvent& e)
{
    handleKeyboardEvent(e);
}

static void p5mouseSubscriber(const MouseEventTopic &p, const MouseEvent& e)
{
    handleMouseEvent(e);
}

static void p5joystickSubscriber(const JoystickEventTopic& p, const JoystickEvent& e)
{
    handleJoystickEvent(e);
}

static void p5touchSubscriber(const TouchEventTopic& p, const TouchEvent& e)
{
    handleTouchEvent(e);
}

static void p5gestureSubscriber(const GestureEventTopic& p, const GestureEvent& e)
{
    handleGestureEvent(e);
}

static void p5pointerSubscriber(const PointerEventTopic& p, const PointerEvent& e)
{
    handlePointerEvent(e);
}

static void p5filedroppedSubscriber(const FileDropEventTopic& p, const FileDropEvent& e)
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
    gSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "setup");
    gUpdateHandler = (PFNDOUBLE1)GetProcAddress(hInst, "update");

    // Look for implementation of drawing handler
    gDrawHandler = (VOIDROUTINE)GetProcAddress(hInst, "draw");

    // Look for implementation of mouse events
    //gMouseEventHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseEvent");
    gMouseMovedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseMoved");
    gMouseClickedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseClicked");
    gMousePressedHandler = (MouseEventHandler)GetProcAddress(hInst, "mousePressed");
    gMouseReleasedHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseReleased");
    gMouseWheelHandler = (MouseEventHandler)GetProcAddress(hInst, "mouseWheel");
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



