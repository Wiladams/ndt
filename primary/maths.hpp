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
#include "bitbang.hpp"
#include "random.hpp"

#include <stdlib.h>
#include <math.h>


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

// The value of 't' ranges from 0..1 inclusive
// the return value will be between a (at t == 0)
// and b (at t==1), inclusive
inline double LERP(double t, double a, double b)
{
    // first constrain
    if (t<=0) return a;
    if (t>=1) return b;
    
    // linear interpolation
    return a + (t*(b-a)); 
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

inline double lerp(double low, double high, double x)
{
    return low + x*(high-low);
}

inline double mag(double x, double y)
{
    return sqrt(x*x +y*y);
}


#ifdef __cplusplus
}
#endif
