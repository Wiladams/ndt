#pragma once

#include <DirectXMath.h>

using namespace DirectX;

using vec3 = XMFLOAT3;
using point3 = XMFLOAT3;    // 3D point
using rtcolor = XMFLOAT3;   // RGB Color

//using vec3 = XMVECTORF32;      // general vector
//using point3 = XMVECTORF32;    // 3D point
//using rtcolor = XMVECTORF32;   // RGB Color



/*
#ifdef min
#undef min
#undef max
#endif


//
//    grmath.h
//
//    This file contains the data types and math routines
//    that are typically used in the creation of various 
//    kinds of graphics.

//    The data structures are C++, and templatized where it 
//    makes sense.




#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))


// pre-define matrix type so vector routines can use them
template<size_t DimCols, size_t DimRows, typename T> class mat;

//
// A homogenous array of a specified type and size
// this can be a generalized array.  Why not use
// std::array?  Because we don't want to draw in
// std library if we don't have to as it requires exception
// handling.
// DIM - typically 2, 3, or 4
//
template <size_t DIM, typename T> 
struct vec
{
private:
    T fData[DIM];

public:
    // default constructor, sets data values to 
    // default of the specified data type
    vec() 
    {
        for (size_t i = 0; i < DIM; i++)
            fData[i] = T();
    }

    // operator returning reference allows for 
    // the setting a value
    // vec[2] = value;
    T& operator[](const size_t i) 
    {
        assert(i < DIM);
        return fData[i];
    }

    // operator returning value based on index
    // value = vec[2];
    const T& operator[](const size_t i) const 
    {
        assert(i < DIM);
        return data_[i];
    }

    const double lengthSquared() const
    {
        double res=0;
        for (size_t i=0; i<DIMS; i++){
            res += fData[i]*fData[i];
        }

        return res;
    }

    double length() const
    {
        return std::sqrt(lengthSquared());
    }
};

// Two dimensional vector of specified type
template <typename T> 
struct vec<2, T>
{
    union {
        T data[2];
        struct {
            T x;
            T y;
        };
        struct {
            T u;
            T v;
        };
    };

    vec() 
        : x(T()), y(T()) 
    {}
    
    vec(T X, T Y) 
        : x(X), y(Y) 
    {}

    //template <class U> 
    //vec<2, T>(const vec<2, U>& v);
    //template<class U> vec<2,T>(const vec<2, U> &v) : x(v[0]), y(v[1]) {}

    // operator returning reference allows for 
    // the setting a value
    // vec[1] = value;
    T& operator[](const size_t i)
    {
        assert(i < 2);
        return i <= 0 ? x : y;
    }

    const T& operator[](const size_t i) const
    {
        assert(i < 2);
        return i <= 0 ? x : y;
    }
};


// Three dimensional vector of specified type
template <typename T> 
struct vec<3, T>
{
    union {
        T data[3];
    struct {
        T x;
        T y;
        T z;
    };
    struct {
        T r;
        T g;
        T b;
    };
    };

    vec() 
        : x(T()), y(T()), z(T())
    {}

    vec(T X, T Y, T Z) : x(X), y(Y), z(Z)
    {}

    // Copy constructor
    //template <class U> vec<3, T>(const vec<3, U>& v);

    // setting value
    T& operator[](const size_t i)
    {
        assert(i < 3);
        return i <= 0 ? x : (1 == i ? y : z);
    }

    // retrieving value
    const T& operator[](const size_t i) const
    {
        assert(i < 3);
        return i <= 0 ? x : (1 == i ? y : z);
    }

    vec<3, T>& operator +=(const vec<3, T>& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;

        return *this;
    }

    vec<3, T>& operator -=(const vec<3, T>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }

    vec<3, T>& operator *=(const vec<3, T>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;

        return *this;
    }

    vec<3, T>& operator *=(const float scale)
    {
        x *= scale;
        y *= scale;
        z *= scale;

        return *this;
    }

    vec<3, T>& operator /=(const float scale)
    {
        x /= scale;
        y /= scale;
        z /= scale;

        return *this;
    }

    // return linear interpolation between two vectors
    vec<3, T> lerp(const vec<3, T>& other, float t) const
    {
        vec<3, T> res();
        res.x = lerp(t, x, other.x);
        res.y = lerp(t, y, other.y);
        res.z = lerp(t, z, other.z);

        return res;
    }

    double length_squared() const
    {
        return (x * x + y * y + z * z);
    }

    float length() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    vec<3, T>& normalize(T l = 1)
    {
        *this = (*this) * (l / norm()); return *this;
    }
};


// Math constants
static const float  PI32 = 3.14159265359f;

static const double QUARTER_PI = 0.7853982;
static const double HALF_PI = 1.57079632679489661923;
static const double PI = 3.14159265358979323846;
static const double TWO_PI = 6.28318530717958647693;
static const double TAU = 6.28318530717958647693;
*/

// Math functions
/*
    Routines to be found in here, typical of a shader 
    language, or any other graphics library
    In many cases, there's already something in standard
    math libraries, but here, the operation might apply to a vector
    of some type.

    abs         clamp           exp2
    acos        clip            faceforward
    all         cos             firstbithigh
    any         cosh            firstbitlow
    asdouble    countbits       floor
    asfloat     cross           fma
    asin        degrees         fmod
    asint       determinant     frac
    asuint      distance        frexp
    atan        dot             isfinite
    atan2       dst             isinf
    ceil        exp             isnan

    ldexp       length          lerp
    lit         log             log10
    log2        mad             max 
    min         modf            msad4
    mul         noise           normalize
    pow         printf          radians
    rcp         reflect         refract
    reversebits round           rsqrt
    saturate    sign            sin
    sincos      sinh            smoothstep
    sqrt        step            tan 
    tanh        tex1D           tex2D
    tex3D       transpose       trunc

*/
/*
template <typename T>
inline T clamp(T x, T minValue, T maxValue) noexcept
{
    return MIN(MAX(x,minValue), maxValue);
}

template <typename T>
inline double degrees(T x) { return x * 57.29577951308232; }

// This lerp is the more traditional way of doing it 
// for graphics routines.  No constraint/clamp
// typically a t ranges [0..1], but it does not
// need to be limited to those values.
template <typename T>
inline T lerp(const T startValue, const T endValue, const float t)
{
    //return (1 - t) * startValue + t * endValue;
    return startValue + t*(endValue-startValue);
}

int mul(const int a, const int b) {return a*b;}
float mul(const float a, const float b) {return a*b;}

template <typename T>
vec<3,T> mul(const float a, const vec<3,T> &v)
{
    vec<3,T> res(v);
    return res *= a;
}

template <typename T>
inline double radians(T x) { return x * 0.017453292519943295; }

inline int sign(double val) { return ((0 < val) - (val < 0)); }

*/