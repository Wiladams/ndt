#pragma once

/*
    Math routines are always very useful not only for graphics, but for
    machine learning and other domains.

    Herein lies some typical math routines to be used while fiddling about
    with graphics.  The routines here are meant to be simple and representative
    of typical usage.  If you're using an API such as p5, these routines
    will have convenient wrappers.

    Aside from the typical trig functions, there are also a few key data
    structures, such as vectors, matrices, and quaternions.  They are
    here so you don't have to invent those from scratch for each new
    program.

    Some specialization, such as 3D modeling, is outside this file, because that
    is outside the scope of what we want in here.

    The routines in here might not be the fastest, but they're fairly compact, 
    and should not be embarrassing.

    References
    https://github.com/nfrechette/rtm
    https://github.com/HandmadeMath/Handmade-Math/blob/master/HandmadeMath.h
    yocto-gl

    And many others
*/


#include <stdint.h>
#include <stdlib.h>
#include <cmath>
#include <limits>

#include "bitbang.h"


/*
Routines to be found in here, typical of a shader
language, or any other graphics library.
In many cases, there's already something in standard
math libraries, but here, the operation might apply to a vector
of some type.

The names start with a capital letter in order to reduce conflict
with names that might exist in a global namespace.


        clip            faceforward
all                      firstbithigh
any         cosh            firstbitlow
asdouble    countbits       -floor
asfloat     cross           fma
asint       determinant     frac
asuint      distance        frexp
        dot         
       dst             isinf
-ceil                     -isnan

ldexp       length          
lit                     log10
log2        mad             
        modf            msad4
mul         noise           normalize
         printf          
rcp         reflect         refract
reversebits round           rsqrt
saturate               
sincos      sinh            
       step            
       tex1D           tex2D
tex3D       transpose       trunc

*/


namespace maths {
    typedef double Float;

    // Math constants
    // Some useful constants

    constexpr auto Pi = 3.14159265358979323846;
    constexpr auto Pif = (float)Pi;
    constexpr auto PiOver2 = 1.57079632679489661923;
    constexpr auto PiOver4 = 0.78539816339744830961;
    constexpr auto Pi2 = 6.28318530717958647693;
    constexpr auto InvPi = 0.31830988618379067154;
    constexpr auto Inv2Pi = 0.15915494309189533577;
    constexpr auto Inv4Pi = 0.07957747154594766788;

    constexpr auto Sqrt2 = 1.41421356237309504880;

    constexpr auto int_max = std::numeric_limits<int>::max();
    constexpr auto int_min = std::numeric_limits<int>::min();
    constexpr auto flt_max = std::numeric_limits<float>::max();
    constexpr auto flt_min = std::numeric_limits<float>::min();
    constexpr auto flt_eps = std::numeric_limits<float>::epsilon();

}   // namespace maths


// Function declarations for floats
namespace maths
{
    inline float Abs(float a);
    inline float ACos(float a);
    inline float ASin(float a);

    inline float ATan(float a);
    inline float ATan2(float y, float x);

    inline float Clamp(float a, float min, float max);
    inline float Cos(float a);
    inline float Exp(float a);
    inline float Exp2(float a);
    inline float FMod(float a, float b);
    inline float Log(float a);
    inline float Log2(float a);
    inline float Max(float a, float b);
    inline float Min(float a, float b);

    inline float Pow(float a, float b);

    inline float Sign(float a);
    inline float Sin(float a);
    inline float Sqr(float a);
    inline float Sqrt(float a);
    inline float Tan(float a);

    inline float IsFinite(float a);
    inline float Radians(float a);
    inline float Degrees(float a);
    inline float Lerp(float a, float b, float u);
    inline void Swap(float& a, float& b);
    inline float SmoothStep(float a, float b, float u);
    inline float Bias(float a, float bias);
    inline float Gain(float a, float gain);
}

// Function implementations
namespace maths 
{
    inline float Abs(float a) { return a < 0 ? -a : a; }
    inline float ACos(float a) { return std::acos(a); }
    inline float ASin(float a) { return std::asin(a); }

    inline float ATan(float a) { return std::atan(a); }
    inline float ATan2(float y, float x) { return std::atan2(y, x); }

    inline float Clamp(float a, float min_, float max_) { return Min(Max(a, min_), max_); }
    inline float Cos(float a) { return std::cos(a); }
    inline float Exp(float a) { return std::exp(a); }
    inline float Exp2(float a) { return std::exp2(a); }
    inline float FMod(float a, float b) { return std::fmod(a, b); }
    inline float Log(float a) { return std::log(a); }
    inline float Log2(float a) { return std::log2(a); }
    inline float Max(float a, float b) { return (a > b) ? a : b; }
    inline float Min(float a, float b) { return (a < b) ? a : b; }

    inline float Pow(float a, float b) { return std::pow(a, b); }

    inline float Sign(float a) { return a < 0 ? -1.0f : 1.0f; }
    inline float Sin(float a) { return std::sin(a); }
    inline float Sqr(float a) { return a * a; }
    inline float Sqrt(float a) { return std::sqrt(a); }
    inline float Tan(float a) { return std::tan(a); }

    inline float IsFinite(float a) { return std::isfinite(a); }
    inline float Radians(float a) { return a * 0.017453292519943295; }
    inline float Degrees(float a) { return a * 57.29577951308232; }
    inline float Lerp(float a, float b, float u) { return a * (1 - u) + b * u; }
    inline void Swap(float& a, float& b) { return std::swap(a, b); }
    inline float SmoothStep(float a, float b, float u) {
        auto t = Clamp((u - a) / (b - a), 0.0f, 1.0f);
        return t * t * (3 - 2 * t);
    }
    inline float Bias(float a, float bias) {
        return a / ((1 / bias - 2) * (1 - a) + 1);
    }
    inline float Gain(float a, float gain)
    {
        return (a < 0.5f) ? Bias(a * 2, gain) / 2
            : Bias(a * 2 - 1, 1 - gain) / 2 + 0.5f;
    }
}



/*
// Function declarations for integer types
namespace maths
{
    inline int Abs(int a);
    inline int Min(int a, int b);
    inline int Max(int a, int b);
    inline int Clamp(int a, int min, int max);
    inline int Sign(int a);
    inline int Pow2(int a);
    inline void Swap(int& a, int& b);

    inline size_t Min(size_t a, size_t b);
    inline size_t Max(size_t a, size_t b);

}
*/

// Some useful types
// Vectors, matrices, etc
namespace maths {
    struct vec2f {
        float x = 0;
        float y = 0;

        inline float& operator[](int i);
        inline const float& operator[](int i) const;
    };

    struct vec3f {
        float x = 0;
        float y = 0;
        float z = 0;

        inline float& operator[](int i);
        inline const float& operator[](int i) const;
    };

    struct vec4f {
        float x = 0;
        float y = 0;
        float z = 0;
        float w = 0;

        inline float& operator[](int i);
        inline const float& operator[](int i) const;
    };

    // Element access
    inline vec3f xyz(const vec4f& a);

    // Comparison operators
    inline bool operator==(const vec2f& a, const vec2f& b);
    inline bool operator!=(const vec2f& a, const vec2f& b);

}

//==========================
// IMPLEMENTATION  Vectors
//==========================

namespace maths 
{
    // Vec2
    inline float& vec2f::operator[](int i) { return (&x)[i]; }
    inline const float& vec2f::operator[](int i)const { return (&x)[i]; }

    // Vec3
    inline float& vec3f::operator[](int i) { return (&x)[i]; }
    inline const float& vec3f::operator[](int i)const { return (&x)[i]; }

    // Vec4
    inline float& vec4f::operator[](int i) { return (&x)[i]; }
    inline const float& vec4f::operator[](int i)const { return (&x)[i]; }

    inline vec3f xyz(const vec4f& a) { return { a.x, a.y, a.z }; }

    inline bool operator==(const vec2f& a, const vec2f& b) { return ((a.x == b.x) && (a.y == b.y)); }
    inline bool operator!=(const vec2f& a, const vec2f& b) { return a.x != b.x || a.y != b.y; }
}


namespace maths {


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

    //template <typename T, typename U, typename V>
    //inline T Clamp(T val, U low, V high)
    //{
    //    if (val < low)
    //        return low;
    //    else if (val > high)
    //        return high;
    //    else
    //        return val;
    //}


    //inline double Cos(const double a)
    //{
    //    return std::cos(a);
    //}

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

    template <>
    inline double Min(double a, double b)
    {
        return a < b ? a : b;
    }

    template <typename T>
    inline T Max(T a, T b) {
        return a > b ? a : b;
    }

    template <>
    inline double Max(double a, double b)
    {
        return a > b ? a : b;
    }

        // Some useful routines
        // returns the sign of the value
        // value  < 0 --> -1
        // value  > 0 -->  1
        // value == 0 -->  0
        // this will only work in cases where 0 represents false
        // and 1 represents true
    //template <typename T>
    //inline int Sign(T val) { return ((0 < val) - (val < 0)); }

    //template <typename T>
    //inline T Degrees(T a) { return a * 57.29577951308232; }

    //template<>
    //inline double Degrees(double x) { return x * 57.29577951308232; }
    
    //template <typename T>
    //inline T Radians(T x) { return x * 0.017453292519943295; }

    //template<>
    //inline double Radians(double x) { return x * 0.017453292519943295; }

    
    //inline double Sin(double x) { return std::sin(x); }


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

    //template <typename T>
    //inline T Tan(const T a)
    //{
    //    return (T)std::tan(a);
    //}

    template <typename T>
    inline T Tanh(const T a)
    {
        return (T)std::tanh(a);
    }
    
}