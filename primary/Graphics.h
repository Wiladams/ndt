#pragma once


#include "blend2d.h"
#include "geometry.h"

// Enumerations for all the commands
// 
enum  GRCOMMANDS {
GCMD_NONE,
GCMD_ANGLEMODE,
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

GCMD_FILL_NONE, 
GCMD_FILL_STYLE,
GCMD_FILL_COLOR,
GCMD_FILL_GRADIENT,
GCMD_FILL_PATTERN,


GCMD_STROKE_NONE, 
GCMD_STROKE_COLOR,
GCMD_STROKE_STYLE,


GCMD_FLUSH,
GCMD_LOADPIXELS,
GCMD_UPDATEPIXELS,
GCMD_CLEAR,
GCMD_CLEARRECT,
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
GCMD_PATH,

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

// RectMode
enum class RECTMODE : unsigned
{
    CORNER,
    CORNERS,
    CENTER,
};

// EllipseMode
enum class ELLIPSEMODE : unsigned
{
    CORNER,
    CORNERS,
    CENTER,
    RADIUS
};

// Text Alignment
enum class ALIGNMENT : unsigned
{
    CENTER = 0x01,

    LEFT = 0x02,
    RIGHT = 0x04,

    TOP         = 0x10, 
    BASELINE    = 0x20,
    BOTTOM      = 0x40,
    MIDLINE     = 0x80,

} ;

// Text Wrap
// 
enum class TEXTWRAP : unsigned
{
    WORD,
    CHAR
};

// are angles specified in degrees
// or in radians
enum class ANGLEMODE : unsigned
{
    DEGREES,
    RADIANS
};

enum class SHAPEMODE : unsigned
{
    NONE = 0,
    OPEN = 1,
    POINTS = 2,
    LINES = 3,
    TRIANGLES = 4,
    TRIANGLE_STRIP = 5,
    TRIANGLE_FAN = 6,
    QUADS = 7,
    QUAD_STRIP=8,

};


enum class SHAPEEND : unsigned
{
    STROKE = 0,
    CLOSE = 1,
};

enum class DRAWSTYLE : unsigned
{
  Fill,
  Stroke,
  FillAndStroke,
  StrokeAndFill
};


typedef BLRgba32 Pixel;
//using Pixel = maths::vec4b;

/*
    IGraphics defines the essential interface for doing vector graphics
    This is a pure virtual interface for the most part, so a sub-class must 
    implement a fair bit of stuff.
*/
class IGraphics
{
public:
    virtual ~IGraphics() = default;

    virtual void setDpiUnits(const int dpi, const float units) = 0;

    virtual void angleMode(const ANGLEMODE mode) = 0;
    virtual void ellipseMode(const ELLIPSEMODE mode) = 0;
    virtual void rectMode(const RECTMODE mode) = 0;
    virtual void blendMode(int mode) = 0;
    virtual void strokeCaps(int caps) = 0;
    virtual void strokeJoin(int join) = 0;
    virtual void strokeWeight(float weight) = 0;

    virtual void push()=0;
    virtual void pop()=0;
    virtual void flush() = 0;

    virtual void translate(double dx, double dy)=0;
    virtual void scale(double sx, double sy)=0;
    virtual void scale(double sxy) { scale(sxy, sxy); }
    virtual void rotate(double angle, double cx, double cy) = 0;
    virtual void rotate(double angle) { rotate(angle, 0, 0); }

    virtual int blue(const Pixel& c) { return c.b(); }
    virtual int green(const Pixel& c) {return c.g(); }
    virtual int red(const Pixel& c) {return c.r(); }
    virtual int alpha(const Pixel& c) {return c.a(); }

    // BUGBUG, only handles RGB mode, not HSL
    virtual Pixel color(int a, int b, int c, int d) { return BLRgba32(a, b, c, d); }
    virtual Pixel color(int r, int g, int b) {return color(r, g, b, 255);}
    virtual Pixel color(int gray, int alpha) {return color(gray, gray, gray, alpha);}
    virtual Pixel color(int gray) {return color(gray, gray, gray, 255);}

    virtual Pixel lerpColor(const Pixel& from, const Pixel& to, float f) {
        uint8_t r = (uint8_t)maths::lerp((float)from.r(), (float)to.r(), f);
        uint8_t g = (uint8_t)maths::lerp((float)from.g(), (float)to.g(),f);
        uint8_t b = (uint8_t)maths::lerp((float)from.b(), (float)to.b(),f);
        uint8_t a = (uint8_t)maths::lerp((float)from.a(), (float)to.a(),f);

        return Pixel((int)r, (int)g, (int)b, (int)a);
    }

    virtual void noFill() = 0;

    virtual void fill(const BLVarCore& s) = 0;
    virtual void fill(const BLGradientCore& g) = 0; // { fill(BLVar(g)); }
    virtual void fill(const Pixel& c) = 0;  // {fill(BLVar(c)); }
    //virtual void fill(const BLPatternCore& p) {}

    //virtual void fill(const BLVarCore& var) { fill(var); }

    virtual void fill(int r, int g, int b, int a) {fill(color(r,g,b,a));}
    virtual void fill(int r, int g, int b) { fill(color(r, g, b, 255)); }
    virtual void fill(int gray, int alpha) { fill(color(gray, gray, gray, alpha)); }
    virtual void fill(int gray) { fill(color(gray, gray, gray, 255)); }


    virtual void stroke(const BLVarCore& s) = 0;
    virtual void stroke(const BLGradientCore& g) = 0;
    virtual void stroke(const Pixel& c) = 0;// {stroke(BLVar(c)); };

    virtual void stroke(int r, int g, int b, int a) { stroke(color(r, g, b, a)); }
    virtual void stroke(int r, int g, int b) { stroke(color(r, g, b, 255)); }
    virtual void stroke(int gray, int alpha) { stroke(color(gray, gray, gray, alpha)); }
    virtual void stroke(int gray) { stroke(color(gray, gray, gray, 255)); }
    
    virtual void noStroke() = 0;


    // Background management
    virtual void clear() = 0;
    virtual void clearRect(double x, double y, double w, double h) = 0;
    virtual void background(const Pixel& c) = 0;

    // Clipping
    virtual void clip(const maths::bbox2f& bb) = 0;
    //{ clip(bb.min.x, bb.min.y, bb.max.x - bb.min.x, bb.max.y - bb.min.y); }
    //virtual void clip(double x, double y, double w, double h) = 0;
    virtual void noClip() = 0;

    // Geometry
    // hard set a specfic pixel value
    virtual void set(double x, double y, const Pixel& c) = 0;
    virtual void point(double x, double y) = 0;
    virtual void line(double x1, double y1, double x2, double y2) = 0;
    virtual void arc(double cx, double cy, double r, double start, double sweep) = 0;
    virtual void rect(double x, double y, double width, double height) { rect(x, y, width, height, 1, 1); }
    virtual void rect(double x, double y, double width, double height, double xradius, double yradius) = 0;
    virtual void ellipse(double a, double b, double c, double d) = 0;
    virtual void circle(double cx, double cy, double diameter) = 0;
    virtual void triangle(double x1, double y1, double x2, double y2, double x3, double y3) = 0;
    virtual void bezier(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) = 0;
    virtual void polyline(const BLPoint* pts, size_t n) = 0;
    virtual void polygon(const BLPoint* pts, size_t n) = 0;
    virtual void quad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4) = 0;
    
    virtual void path(const BLPath& path) = 0;
    //virtual void strokePath(const BLPath& path) = 0;
    //virtual void fillPath(const BLPath& path) = 0;

    virtual void beginShape(SHAPEMODE shapeKind) = 0;
    virtual void vertex(double x, double y) = 0;
    virtual void endShape(SHAPEEND endKind) = 0;

    // Bitmaps
    virtual void image(const BLImage& img, int x, int y) = 0;
    virtual void scaleImage(const BLImage& src,
        double srcX, double srcY, double srcWidth, double srcHeight,
        double dstX, double dstY, double dstWidth, double dstHeight)=0;

    virtual void textAlign(ALIGNMENT horizontal, ALIGNMENT vertical) = 0;
    virtual void textFace(const BLFontFace& face) = 0;
    virtual void textFont(const char* fontname) = 0;
    virtual void textSize(float size) = 0;
    virtual void text(const char* txt, float x, float y, float x2=0, float y2=0) = 0;
    virtual void textAtBaseline(const char* txt, float x, float y, float x2 = 0, float y2 = 0) = 0;
    virtual maths::vec2f textMeasure(const char* txt) = 0;
    virtual float textAscent() = 0;
    virtual float textDescent() = 0;

    virtual void vartext(float x, float y, const char* format, ...)
    {
        char txtBuff[512];
        va_list args = nullptr;
        va_start(args, format);

        vsprintf_s(txtBuff, format, args);

        text(txtBuff, x, y);
        va_end(args);

    }
};