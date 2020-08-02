#pragma once

/*
    Math routines are always very useful not only for graphics, but for
    machine learning and other domains.

    Herein lies some typical math routines to be used while fiddling about
    with graphics.  The routines here are meant to be simple and representative
    of typical usage.  If you're using an API such as p5, these routines
    will have convenient wrappers.

    References
    https://github.com/nfrechette/rtm
    https://github.com/HandmadeMath/Handmade-Math/blob/master/HandmadeMath.h

*/


#include <stdint.h>
#include <stdlib.h>
#include <cmath>

#include "bitbang.h"


namespace maths {
    typedef double Float;

    // Math constants
    // Some useful constants
    //static const float  PI32 = 3.14159265359f;
    static constexpr Float ShadowEpsilon = 0.0001f;
    static constexpr Float Pi = 3.14159265358979323846;
    static constexpr Float PiOver2 = 1.57079632679489661923;
    static constexpr Float PiOver4 = 0.78539816339744830961;
    static constexpr Float Pi2 = 6.28318530717958647693;
    static constexpr Float InvPi = 0.31830988618379067154;
    static constexpr Float Inv2Pi = 0.15915494309189533577;
    static constexpr Float Inv4Pi = 0.07957747154594766788;

    static constexpr Float Sqrt2 = 1.41421356237309504880;


    /*
    Routines to be found in here, typical of a shader
    language, or any other graphics library.
    In many cases, there's already something in standard
    math libraries, but here, the operation might apply to a vector
    of some type.

    The names start with a capital letter in order to reduce conflict
    with names that might exist in a global namespace.

    -abs         -clamp           exp2
    acos        clip            faceforward
    all         cos             firstbithigh
    any         cosh            firstbitlow
    asdouble    countbits       -floor
    asfloat     cross           fma
    asin        degrees         fmod
    asint       determinant     frac
    asuint      distance        frexp
    atan        dot             isfinite
    atan2       dst             isinf
    -ceil        exp             -isnan

    ldexp       length          lerp
    lit         log             log10
    log2        mad             -max
    -min         modf            msad4
    mul         noise           normalize
    pow         printf          radians
    rcp         reflect         refract
    reversebits round           rsqrt
    saturate    sign            sin
    sincos      sinh            smoothstep
    sqrt        step            tan
    -tanh        tex1D           tex2D
    tex3D       transpose       trunc

*/

    template <typename T>
    inline T Abs(const T v)
    {
        return v < 0 ? -v : v;
    }

    template <typename T>
    inline T Add(const T& a, const T& b)
    {
        return a + b;
    }

    template <typename T>
    inline T Ceil(const T a)
    {
        return (T)std::ceil(a);
    }

    template <typename T, typename U, typename V>
    inline T Clamp(T val, U low, V high)
    {
        if (val < low)
            return low;
        else if (val > high)
            return high;
        else
            return val;
    }


    inline double Cos(const double a)
    {
        return std::cos(a);
    }

    template <typename T>
    inline T Floor(T val)
    {
        return std::floor(val);
    }

    // isNaN only applies to floating point (float, double)
    template <typename T>
    inline bool isNaN(const T x) {
        return std::isnan(x);
    }

    template <>
    inline bool isNaN(const int x) {
        return false;
    }

    // Lerp
    // This implementation of LERP is the most accurate
    // and guarantees you get v1 at 0.0 and v2 at 1.0
    //template <typename U, typename T>
    template <typename T>
    inline T Lerp(const double t, T v1, T v2)
    {
        return (T)((1 - t) * v1 + t * v2);
    }

    template<>
    inline double Lerp(const double t, double v1, double v2)
    {
        return (1 - t) * v1 + t * v2;
    }
    /*
    template <typename U, typename T, typename V>
    inline double Map(U x, T olow, T ohigh, V rlow, V rhigh)
    {
        return rlow + (x - olow) * ((rhigh - rlow) / (ohigh - olow));
    }

    template<>
    inline double Map(double x, double olow, double ohigh, double rlow, double rhigh)
    {
        return rlow + (x - olow) * ((rhigh - rlow) / (ohigh - olow));
    }
    */
    
    inline double Map(double x, double olow, double ohigh, double rlow, double rhigh)
    {
        return rlow + (x - olow) * ((rhigh - rlow) / (ohigh - olow));
    }

    template <typename T>
    inline T Min(T a, T b) {
        return a < b ? a : b;
    }

    template <typename T>
    inline T Max(T a, T b) {
        return a > b ? a : b;
    }

        // Some useful routines
        // returns the sign of the value
        // value  < 0 --> -1
        // value  > 0 -->  1
        // value == 0 -->  0
        // this will only work in cases where 0 represents false
        // and 1 represents true
    template <typename T>
    inline int Sign(T val) { return ((0 < val) - (val < 0)); }

    template <typename T>
    inline T Degrees(T a) { return a * 57.29577951308232; }

    template<>
    inline double Degrees(double x) { return x * 57.29577951308232; }
    
    template <typename T>
    inline T Radians(T x) { return x * 0.017453292519943295; }

    template<>
    inline double Radians(double x) { return x * 0.017453292519943295; }

    
    inline double Sin(double x) { return std::sin(x); }


/*
    template <typename T>
    inline T ACos(const T a)
    {
        return (T)acos(a);
    }

    template <typename T>
    inline T ASin(const T a)
    {
        return (T)asin(a);
    }

    template <typename T>
    inline T Cosh(const T a)
    {
        return (T)cosh(a);
    }


    template <typename T>
    inline T Divide(const T a, const T b)
    {
        return (T)a / b;
    }

    // Exp


    template <typename T>
    inline T Multiply(const T a, const T b)
    {
        return (T)(a * b);
    }

    template <typename T>
    inline T Pow(const T a, const T b)
    {
        return pow(a, b);
    }



    template <typename T>
    inline T SmoothStep(const T mn, const T mx)
    {

    }
*/

    template <typename T>
    inline T Subtract(const T a, const T b)
    {
        return a - b;
    }

    template <typename T>
    inline T Tan(const T a)
    {
        return (T)std::tan(a);
    }

    template <typename T>
    inline T Tanh(const T a)
    {
        return (T)std::tanh(a);
    }
    
}