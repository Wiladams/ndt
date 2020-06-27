#pragma once

/*
    Math routines are always very useful not only for graphics, but for
    machine learning and other domains.

    Herein lies some typical math routines to be used while fiddling about
    with graphics.

    References
    https://github.com/nfrechette/rtm
    https://github.com/HandmadeMath/Handmade-Math/blob/master/HandmadeMath.h

*/


#include <stdint.h>
#include <stdlib.h>
#include <cmath>

#include "bitbang.hpp"
#include "random.hpp"

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

// doesn't deal with -0 vs +0
#define ABS(a) ((a) > 0 ? (a) : -(a))



// turn a division by 255 into something 
// much cheaper to calculate
// for values between 0 and 65534
#define div255(num) ((num + (num >> 8)) >> 8)

// perform a linear interpolation between a value 'a'
// a background value, and a foreground value, using
// fast div255
#define lerp255(bg, fg, a) ((uint8_t)div255((fg*a+bg*(255-a))))

#define swap2(a, b) { int16_t t = a; a = b; b = t; }

// Math constants
static const float  PI32 = 3.14159265359f;

static const double QUARTER_PI = 0.7853982;
static const double HALF_PI = 1.57079632679489661923;
static const double PI = 3.14159265358979323846;
static const double TWO_PI = 6.28318530717958647693;
static const double TAU = 6.28318530717958647693;


#ifdef __cplusplus
extern "C" {
#endif

// clamp() and constrain() do the exact same thing
//inline double clamp(double x, double min, double max) 
//{
//    if (x < min) return min;
//    if (x > max) return max;
//    return x;
//}

inline double constrain(double x, double low, double high)
{
    return MIN(MAX(x, low), high);
}

inline double map(double x, double olow, double ohigh, double rlow, double rhigh, bool tight= false)
{
    if (tight) {
        x = constrain(x, olow, ohigh);
    }

    return rlow + (x-olow)*((rhigh-rlow)/(ohigh-olow));
}


// Some useful routines
// returns the sign of the value
// value  < 0 --> -1
// value  > 0 -->  1
// value == 0 -->  0
// this will only work in cases where 0 represents false
// and 1 represents true
inline int sgn(double val) { return ((0 < val) - (val < 0)); }

inline double sq(double x) {return x*x;}


// Math functions
inline double degrees(double x) { return x * 57.29577951308232; }
inline double radians(double x) { return x * 0.017453292519943295; }



inline double dist(double x1, double y1, double x2, double y2)
{
    return sqrt(sq(x2-x1) + sq(y2-y1));
}

// This lerp is the more traditional way of doing it 
// for graphics routines.  No constraint/clamp
// typically a t ranges [0..1], but it does not
// need to be limited to those values.
inline double lerp(const double startValue, const double endValue, const double t)
{
    return (1 - t) * startValue + t * endValue;
    //return low + x*(high-low);
}

inline double mag2(double x, double y)
{
    return sqrt(x*x +y*y);
}

inline double mag3(double a, double b, double c)
{
    return sqrt(a*a + b*b + c*c);
}



#ifdef __cplusplus
}
#endif
