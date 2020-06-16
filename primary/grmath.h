#pragma once

/*
//
//    grmath.h
//
//    This file contains the data types and math routines
//    that are typically used in the creation of various
//    kinds of graphics.
*/

#include <cmath>
#include <cassert>
#include <iostream>


using std::sqrt;


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
    T data[DIM];

public:
    // default constructor, sets data values to 
    // default of the specified data type
    vec() 
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] = T();
    }

    // operator returning reference allows for 
    // the setting a value
    // vec[2] = value;
    T& operator[](const size_t i) 
    {
        assert(i < DIM);
        return data[i];
    }

    // operator returning value based on index
    // value = vec[2];
    const T& operator[](const size_t i) const 
    {
        assert(i < DIM);
        return data[i];
    }

    const double lengthSquared() const
    {
        double res=0;
        for (size_t i=0; i<DIM; i++){
            res += data[i]* data[i];
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

    //real_t operator[](int i) const { return e[i]; }
    //real_t& operator[](int i) { return e[i]; }
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

    // Negation
    vec<3, T> operator-() const
    { 
        return vec<3, T>(-data[0], -data[1], -data[2]);
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

    double lengthSquared() const
    {
        return (x * x + y * y + z * z);
    }

    float length() const
    {
        return std::sqrt(lengthSquared());
    }

    // Normalize the current vector in place
    vec<3, T>& normalize(T l = 1)
    {
        *this = (*this) * (l / length()); 
        return *this;
    }

    // Return the normalized version of the current vector
    vec<3,T> unit() const 
    {
        vec<3, T> res(*this);
        return res.normalize();
        //double len = length();
        //return vec3(x / len, y / len, z / len);
    }
};


/*
    Data structure representing Determinant of a matrix
*/
template<size_t DIM, typename T> struct dt {
    static T det(const mat<DIM, DIM, T>& src) {
        T ret = 0;
        for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<typename T> struct dt<1, T> {
    static T det(const mat<1, 1, T>& src) {
        return src[0][0];
    }
};


/*
    basis for matrix structure
    You can create a type of matrix of particular dimensions like this:
    typedef mat<4,4,double> Mat4x4;
*/
template<size_t DimRows, size_t DimCols, typename T>
class mat {
    vec<DimCols, T> rows[DimRows];
public:
    mat() {}

    vec<DimCols, T>& operator[] (const size_t idx) {
        assert(idx < DimRows);
        return rows[idx];
    }

    const vec<DimCols, T>& operator[] (const size_t idx) const {
        assert(idx < DimRows);
        return rows[idx];
    }

    // Get a column value of a matrix
    vec<DimRows, T> col(const size_t idx) const {
        assert(idx < DimCols);
        vec<DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    // Set a particular column of a matrix
    // mat.set_col(1, vec);
    //
    void set_col(size_t idx, vec<DimRows, T> v) {
        assert(idx < DimCols);
        for (size_t i = DimRows; i--; rows[i][idx] = v[i]);
    }

    // create an identity matrix
    // mat res = mat::identity();
    //
    static mat<DimRows, DimCols, T> identity() {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = (i == j));
        return ret;
    }

    // Determinant of a matrix
    T det() const {
        return dt<DimCols, T>::det(*this);
    }

    mat<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const {
        mat<DimRows - 1, DimCols - 1, T> ret;
        for (size_t i = DimRows - 1; i--; )
            for (size_t j = DimCols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    // Cofactor matrix of a matrix
    T cofactor(size_t row, size_t col) const {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    mat<DimRows, DimCols, T> adjugate() const {
        mat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    mat<DimRows, DimCols, T> invert_transpose() {
        mat<DimRows, DimCols, T> ret = adjugate();
        T tmp = ret[0] * rows[0];
        return ret / tmp;
    }

    mat<DimRows, DimCols, T> invert() {
        return invert_transpose().transpose();
    }

    mat<DimCols, DimRows, T> transpose() {
        mat<DimCols, DimRows, T> ret;
        for (size_t i = DimCols; i--; ret[i] = this->col(i));
        return ret;
    }
};


/*
    Some matrix arithmetic
*/
template<size_t DimRows, size_t DimCols, typename T> vec<DimRows, T> operator*(const mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs) {
    vec<DimRows, T> ret;
    for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs);
    return ret;
}

template<size_t R1, size_t C1, size_t C2, typename T>mat<R1, C2, T> operator*(const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs) {
    mat<R1, C2, T> result;
    for (size_t i = R1; i--; )
        for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
    return result;
}

template<size_t DimRows, size_t DimCols, typename T>mat<DimCols, DimRows, T> operator/(mat<DimRows, DimCols, T> lhs, const T& rhs) {
    for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs);
    return lhs;
}

template <size_t DimRows, size_t DimCols, class T> std::ostream& operator<<(std::ostream& out, mat<DimRows, DimCols, T>& m) {
    for (size_t i = 0; i < DimRows; i++) out << m[i] << std::endl;
    return out;
}

// Some concrete types
typedef vec<2, float> vec2f;
typedef vec<2, int>   vec2i;

//typedef vec<3, double> Vec3;
//typedef vec<4, uint8_t> Vec4b;

using vec3f = vec<3, float>;
using vec3i =  vec<3, int>;
using vec3 = vec<3, float>;

using vec4f = vec<4, float>;
using Matrix = mat<4, 4, float>;
using Matrix3 = mat<3, 3, float>;


using point3 = vec3;
using rtcolor = vec3;




template <typename T>
inline std::ostream& operator<<(std::ostream& out, const vec<3,T>& v) {
    return out << v.data[0] << ' ' << v.data[1] << ' ' << v.data[2];
}

// Simple arithmetic operators
template <typename T>
inline vec<3,T> operator*(const double t, const vec<3,T> & v) {
    return vec<3,T>(t * v.data[0], t * v.data[1], t * v.data[2]);
}

template <typename T>
inline vec<3, T> operator*(const vec<3, T>& v, const double t) 
{
    return vec<3, T>(t * v.data[0], t * v.data[1], t * v.data[2]);
}

template <typename T>
inline vec<3,T> operator*(const vec<3, T>& u, const vec<3, T>& v) {
    return vec3(u.data[0] * v.data[0], u.data[1] * v.data[1], u.data[2] * v.data[2]);
}

template <typename T>
inline vec<3,T> operator/(const vec<3, T>& v, const double t) 
{
    return (1 / t) * v;
}

template <typename T>
inline vec<3, T> operator +(const vec<3, T>& a, const vec<3, T>& b)
{
    vec<3,T> res(a);
    return res += b;
}

template <typename T>
inline vec<3, T> operator -(const vec<3, T>& a, const vec<3, T>& b)
{
    vec<3, T> res(a);
    return res -= b;
}

template<size_t LEN, size_t DIM, typename T> vec<LEN, T> embed(const vec<DIM, T>& v, T fill = 1) {
    vec<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = (i < DIM ? v[i] : fill));

    return ret;
}

template<size_t LEN, size_t DIM, typename T> 
vec<LEN, T> proj(const vec<DIM, T>& v) 
{
    vec<LEN, T> ret;
    for (size_t i = LEN; i--; ret[i] = v[i]);

    return ret;
}




// Math constants
/*
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


template <typename T>
inline T Max(const T a, const T b)
{
    return a < b ? b : a;
}

template <typename T>
inline T Min(const T a, const T b)
{
    return a > b ? b : a;
}


template <typename T>
inline T Abs(const T v)
{
    return v < 0 ? -v : v;
}

template <typename T>
inline T Add(const T a, T b)
{
    return a + b;
}

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
inline T Ceil(const T a)
{
    return (T)ceil(a);
}

template <typename T>
inline T Clamp(T x, T minValue, T maxValue) noexcept
{
    return Min(Max(x, minValue), maxValue);
}

template <typename T>
inline T Cos(const T a)
{
    return (T)cos(a);
}

template <typename T>
inline T Cosh(const T a)
{
    return (T)cosh(a);
}

template <typename T>
inline T Degrees(T a) { return a * 57.29577951308232; }

template <typename T>
inline T Divide(const T a, const T b)
{
    return (T)a / b;
}

// Exp

template <typename T>
inline T Floor(const T a)
{
    return floor(a);
}


// This lerp is the more traditional way of doing it 
// for graphics routines.  No constraint/clamp
// typically a t ranges [0..1], but it does not
// need to be limited to those values.
template <typename T>
inline T lerp(const T startValue, const T endValue, const float t)
{
    //return (1 - t) * startValue + t * endValue;
    return startValue + t * (endValue - startValue);
}


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
inline double Radians(T a) { return a * 0.017453292519943295; }

template <typename T>
inline int Sign(T val) { return ((0 < val) - (val < 0)); }

template <typename T>
inline T SmoothStep(const T mn, const T mx)
{

}

template <typename T>
inline T Subtract(const T a, const T b)
{
    return a - b;
}

template <typename T>
inline T Tan(const T a)
{
    return (T)tan(a);
}

template <typename T>
inline T Tanh(const T a)
{
    return (T)tanh(a);
}



inline double dot(const vec3& u, const vec3& v) {
    return u.data[0] * v.data[0]
        + u.data[1] * v.data[1]
        + u.data[2] * v.data[2];
}

// Vector operations
inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u.data[1] * v.data[2] - u.data[2] * v.data[1],
        u.data[2] * v.data[0] - u.data[0] * v.data[2],
        u.data[0] * v.data[1] - u.data[1] * v.data[0]);
}

// Utility Functions
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double_range(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int(int low, int high) {
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double_range(low, high + 1));
}

inline vec3 random_vec3() {
    return vec3(random_double(), random_double(), random_double());
}

inline vec3 random_vec3_range(float min, float max) {
    return vec3(random_double_range(min, max), random_double_range(min, max), random_double_range(min, max));
}


vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double_range(-1, 1), random_double_range(-1, 1), 0);
        if (p.lengthSquared() >= 1) continue;
        return p;
    }
}

vec3 random_unit_vector() {
    auto a = random_double_range(0, 2 * PI);
    auto z = random_double_range(-1, 1);
    auto r = sqrt(1 - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}

vec3 random_in_unit_sphere() {
    while (true) {
        //auto p = vec3::random(-1, 1);
        auto p = random_vec3_range(-1, 1);
        if (p.lengthSquared() >= 1) continue;
        return p;
    }
}

vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2 * dot(v, n) * n;
}

vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_parallel = etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_perp = -sqrt(1.0 - r_out_parallel.lengthSquared()) * n;
    return r_out_parallel + r_out_perp;
}


/*


template <typename T>
vec<3,T> mul(const float a, const vec<3,T> &v)
{
    vec<3,T> res(v);
    return res *= a;
}

*/