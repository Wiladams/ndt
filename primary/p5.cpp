#include "p5.hpp"
#include "random.hpp"
#include "stopwatch.hpp"

namespace p5 {

    int width = 0;              // width of the canvas
    int height = 0;             // height of the canvas
    int frameCount = 0;         // how many frames drawn so far

    Color* pixels = nullptr;    // a pointer to the raw pixels of app canvas


    static StopWatch SWatch;    // Stopwatch used for time 

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

    void strokeWeight(int weight) noexcept
    {
        gAppSurface->strokeWeight(weight);
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

    // Color parts
    int blue(const Color& c) noexcept
    {
        return c.b;
    }

    int green(const Color& c) noexcept
    {
        return c.g;
    }

    int red(const Color& c) noexcept
    {
        return c.r;
    }

    int alpha(const Color& c) noexcept
    {
        return c.a;
    }

    // general color construction from components
    Color color(int a, int b, int c, int d) noexcept
    {
        Color pix;
        pix.r = a;
        pix.g = b;
        pix.b = c;
        pix.a = d;

        return pix;
    }

    Color color(int r, int g, int b) noexcept
    {
        return color(r, g, b, 255);
    }

    Color color(int gray, int alpha) noexcept
    {
        return color(gray, gray, gray, alpha);
    }

    Color color(int gray) noexcept
    {
        return color(gray, gray, gray, 255);
    }


    Color lerpColor(const Color& from, const Color& to, double f) noexcept
    {
        uint8_t r = (uint8_t)lerp(from.r, to.r, f);
        uint8_t g = (uint8_t)lerp(from.g, to.g, f);
        uint8_t b = (uint8_t)lerp(from.b, to.b, f);
        uint8_t a = (uint8_t)lerp(from.a, to.a, f);

        return color((int)r, (int)g, (int)b, (int)a);
    }



    void fill(const BLGradient& g) noexcept
    {
        gAppSurface->fill(g);
    }

    void fill(const Color& pix) noexcept
    {
        gAppSurface->fill(pix);
    }

    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept
    {
        Color c;
        c.r = r;
        c.g = g;
        c.b = b;
        c.a = alpha;
        fill(c);
    }

    void fill(Color pix, uint8_t alpha) noexcept
    {
        Color c = pix;
        c.a = alpha;
        fill(c);
    }

    void fill(uint8_t r, uint8_t g, uint8_t b) noexcept
    {
        Color c;
        c.r = r;
        c.g = g;
        c.b = b;
        c.a = 255;
        fill(c);
    }

    void fill(uint8_t gray, uint8_t alpha) noexcept
    {
        Color c;
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
    void stroke(const Color& pix) noexcept
    {
        gAppSurface->stroke(pix);
    }

    // Change the alpha of an existing color
    void stroke(Color pix, int alpha) noexcept
    {
        Color c = pix;
        c.a = alpha;
        stroke(c);
    }

    void stroke(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) noexcept
    {
        stroke(Color(r, g, b, alpha));
    }

    void stroke(uint8_t r, uint8_t g, uint8_t b) noexcept
    {
        stroke(Color(r, g, b, 255));
    }

    // create color with gray and alpha
    void stroke(uint8_t gray, uint8_t alpha) noexcept
    {
        stroke(Color(gray, gray, gray, alpha));
    }

    void stroke(uint8_t gray) noexcept
    {
        stroke(Color(gray, gray, gray, 255));
    }

    void noStroke() noexcept
    {
        gAppSurface->noStroke();
    }


    void frameRate(int newRate) noexcept
    {
        setFrameRate(newRate);
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
    void background(const Color& pix) noexcept
    {
        gAppSurface->background(pix);
    }

    void background(int a, int b, int c, int d) noexcept
    {
        background(color(a, b, c, d));
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
    void set(int x1, int y1, const Color& c) noexcept
    {
        gAppSurface->set(x1, y1, c);
    }

    Color get(int x, int y) noexcept
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


    void image(BLImage& img, int x, int y) noexcept
    {
        gAppSurface->image(img, x, y);
    }

    void scaleImage(BLImage& src,
        double srcX, double srcY, double srcWidth, double srcHeight,
        double dstX, double dstY, double dstWidth, double dstHeight) noexcept
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

    void createCanvas(long aWidth, long aHeight) noexcept
    {
        width = aWidth;
        height = aHeight;

        setCanvasSize(aWidth, aHeight);

        gAppWindow->setCanvasSize(aWidth, aHeight);
        gAppWindow->show();
    }

    void flush() noexcept
    {
        gAppSurface->flush();
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

void onFrame()
{
    p5::frameCount = p5::frameCount + 1;
}