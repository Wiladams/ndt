#pragma once

#include "appconfig.hpp"
#include "maths.hpp"



int GetAlignedByteCount(const int width, const int bitsperpixel, const int alignment)
{
    return (((width * (bitsperpixel / 8)) + (alignment - 1)) & ~(alignment - 1));
}

typedef union PixRGBA {
    uint32_t intValue;
    uint8_t data[4];
    struct {
        uint8_t blue, green, red, alpha;
    };


    // default constructor, transparent
    PixRGBA():intValue(0){}

    // Easy constructor
    PixRGBA(const PixRGBA &rhs):red(rhs.red),blue(rhs.blue),green(rhs.green),alpha(rhs.alpha){}
    PixRGBA(uint32_t val) : intValue(val){}
    PixRGBA(int r, int g, int b, int a):red(r), green(g), blue(b), alpha(a) {}
} PixRGBA;

typedef union PixRGB {
    uint8_t data[3];
    struct {
        uint8_t blue, green, red;
    };

    // default constructor, transparent
    PixRGB():red(0), green(0), blue(0){}

    // Easy constructor
    PixRGB(const PixRGB &rhs):red(rhs.red),blue(rhs.blue),green(rhs.green){}
    PixRGB(int r, int g, int b):red(r), green(g), blue(b) {}
} PixRGB;


// Some well known types
typedef float	float32;
typedef double	float64;
typedef float32 REAL;
typedef float coord;

typedef uint32_t GRSIZE;
typedef uint16_t GRCOORD;

// Declaration of 2D Point structure
// This is convenient when representing multiple
// coordinates in an array in particular
struct Point2D {
    GRCOORD x, y;
    
    // Convenient constructor, but if you create this constructor,
    // you MUST then explicitly create a default constructor as well
    Point2D(GRCOORD ax, GRCOORD ay) : x(ax), y(ay) {}

    // default constructor
    Point2D() :x(0), y(0) {}

    // Operator Overloads
    // If you do not specify the GRCOORD, you should get an error regarding the need
    // for a narrowing conversion.  This is because the result of x + b.x is
    // an int, and GRCOORD might be some unsigned value.

    // This causes an interesting design consideration.  If we leave GRCOORD as unsigned
    // then we need to consider what happens when we perform addition or subtraction beyond
    // the range of the values.  If the result is negative, for example, should we clip
    // to zero?
    Point2D operator + (const Point2D &b) {return Point2D{GRCOORD(x + b.x), GRCOORD(y + b.y)};}
    Point2D operator += (const Point2D &b) { x = x + b.x; y=y+b.y; return *this;}
    Point2D operator - (const Point2D &b) {return Point2D{GRCOORD(x - b.x), GRCOORD(y - b.y)};}
    Point2D operator -= (const Point2D &b) { x = x - b.x; y=y-b.y; return *this;}
    Point2D operator - () {return Point2D(-x, -y);}
};



/*
  GRTriangle

  A geometric triangle
*/
struct GRTriangle  {
    Point2D verts[3];   // A triangle has 3 vertices

    // Basic constructor take coordinates in any order, sorts them
    // so that subsequent drawing will be faster.
    GRTriangle(GRCOORD x1, GRCOORD y1,
      GRCOORD x2, GRCOORD y2,
      GRCOORD x3, GRCOORD y3)
    {

      verts[0] = {x1, y1};
      verts[1] = {x2, y2};
      verts[2] = {x3, y3};

      // sort the coordinates from topmost
      // to bottommost, so drawing is easier
      // This is essentially a 3 element bubble sort
      Point2D tmp;
      if (verts[0].y > verts[1].y) {
        tmp = verts[0];
        verts[0] = verts[1];
        verts[1] = tmp;
      }

      if (verts[1].y > verts[2].y) {
        tmp = verts[1];
        verts[1] = verts[2];
        verts[2] = tmp;
      }

      // One more round to ensure the second isn't the smallest
      if (verts[0].y > verts[1].y) {
        tmp = verts[0];
        verts[0] = verts[1];
        verts[1] = tmp;
      }
    }
} ;



