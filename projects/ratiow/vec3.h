#pragma once
#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;

class vec3 {
public:
    vec3() : e{ 0,0,0 } {}
    vec3(double e0, double e1, double e2) : e{ e0, e1, e2 } {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    vec3& operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(const double t) {
        return *this *= 1 / t;
    }

    double length() const {
        return sqrt(length_squared());
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
/*
    void write_color(std::ostream& out, int samples_per_pixel) {
        // Replace NaN component values with zero.
        // See explanation in Ray Tracing: The Rest of Your Life.
        if (e[0] != e[0]) e[0] = 0.0;
        if (e[1] != e[1]) e[1] = 0.0;
        if (e[2] != e[2]) e[2] = 0.0;

        // Divide the color total by the number of samples and gamma-correct
        // for a gamma value of 2.0.
        auto scale = 1.0 / samples_per_pixel;
        auto r = sqrt(scale * e[0]);
        auto g = sqrt(scale * e[1]);
        auto b = sqrt(scale * e[2]);

        // Write the translated [0,255] value of each color component.
        out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
            << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
    }
*/
    inline static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    inline static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }

public:
    double e[3];
};



/////////////////////////////////////////////////////////////////////////////////
/*
        Operations on vectors
*/
template <typename T>
inline std::ostream& operator<<(std::ostream& out, const vec<3, T>& v)
{
    return out << v.data[0] << ' ' << v.data[1] << ' ' << v.data[2];
}

template <typename T>
inline vec<3, T> operator+(const vec<3, T>& u, const vec<3, T>& v)
{
    return vec<3, T>(u.data[0] + v.data[0], u.data[1] + v.data[1], u.data[2] + v.data[2]);
}

template <typename T>
inline vec<3, T> operator-(const vec<3, T>& u, const vec<3, T>& v)
{
    return vec<3, T>(u.data[0] - v.data[0], u.data[1] - v.data[1], u.data[2] - v.data[2]);
}

template <typename T>
inline vec<3, T> operator*(const vec<3, T>& u, const vec<3, T>& v) {
    return vec<3, T>(u.data[0] * v.data[0], u.data[1] * v.data[1], u.data[2] * v.data[2]);
}

template <typename T>
inline vec<3, T> operator*(double t, const vec<3, T>& v) {
    return vec<3, T>(t * v.data[0], t * v.data[1], t * v.data[2]);
}

template <typename T>
inline vec<3, T> operator*(const vec<3, T>& v, double t)
{
    return t * v;
}

template <typename T>
inline vec<3, T> operator/(vec<3, T> v, double t) {
    return (1 / t) * v;
}

template <typename T>
inline double dot(const vec<3, T>& u, const vec<3, T>& v) {
    return u.data[0] * v.data[0]
        + u.data[1] * v.data[1]
        + u.data[2] * v.data[2];
}

// Vector operations
template <typename T>
inline vec<3, T> cross(const vec<3, T>& u, const vec<3, T>& v) {
    return vec<3, T>(u.data[1] * v.data[2] - u.data[2] * v.data[1],
        u.data[2] * v.data[0] - u.data[0] * v.data[2],
        u.data[0] * v.data[1] - u.data[1] * v.data[0]);
}

// Return the normalized version of the current vector
template <typename T>
inline vec<3, T> unit_vector(vec<3, T> v) {
    return v / v.length();
}



// Negation
//template <typename T> vec<3, T> operator -(const vec<3, T> rhs)
//{
//    vec <3, T> res(-rhs.x, -rhs.y, -rhs.z);
//    return res;
//}

// return new instance
//template<size_t DIM, typename T> T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
//    T ret = T();
//    for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);

//    return ret;
//}

/*
template<size_t DIM, typename T> vec<DIM, T> operator+(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
    vec<DIM, T> ret;
    for (size_t i = 0; i < DIM; i++)
        ret[i] = lhs[i] + rhs[i];

    return ret;
}

// Modify in place
template<size_t DIM, typename T>vec<DIM, T> operator+(vec<DIM, T> lhs, const vec<DIM, T>& rhs) {
    for (size_t i = DIM; i--; lhs[i] += rhs[i]);

    return lhs;
}

template<size_t DIM, typename T>vec<DIM, T> operator-(vec<DIM, T> lhs, const vec<DIM, T>& rhs) {
    for (size_t i = DIM; i--; lhs[i] -= rhs[i]);

    return lhs;
}



template<size_t DIM, typename T, typename U> vec<DIM, T> operator*(vec<DIM, T> lhs, const U& rhs) {
    for (size_t i = DIM; i--; lhs[i] *= rhs);

    return lhs;
}

template<size_t DIM, typename T, typename U> vec<DIM, T> operator*(const U& rhs, vec<DIM, T> lhs) {
    for (size_t i = DIM; i--; lhs[i] *= rhs);

    return lhs;
}

template<size_t DIM, typename T, typename U> vec<DIM, T> operator/(vec<DIM, T> lhs, const U& rhs) {
    for (size_t i = DIM; i--; lhs[i] /= rhs);

    return lhs;
}
*/


/*
// Simple arithmetic operators



*/

//template <typename T>
//inline vec<3,T> mul(const vec<3, T>& u, const vec<3, T>& v) {
//    return vec<3,T>(u.data[0] * v.data[0], u.data[1] * v.data[1], u.data[2] * v.data[2]);
//}

/*
template <typename T>
inline vec<3,T> operator/(const vec<3, T>& v, const double t)
{
    return (1 / t) * v;
}

template <typename T>
inline vec<3, T> operator +(const vec<3, T>& a, const vec<3, T>& b)
{
    return vec<3, T>(a.x + b.x, a.y + b.y, a.z + b.z);
}
*/
//template <typename T>
//inline vec<3, T> operator +(const vec<3, T>& a, const vec<3, T>& b)
//{
//    vec<3,T> res(a);
//    return res += b;
//}
/*
template <typename T>
inline vec<3, T> operator -(const vec<3, T>& a, const vec<3, T>& b)
{
    vec<3, T> res(a);
    return res -= b;
}
*/
/*
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
*/
/*
    Data structure representing Determinant of a matrix
*/
template<size_t DIM, typename T>
struct dt {
    static T det(const mat<DIM, DIM, T>& src) {
        T ret = 0;
        for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<typename T>
struct dt<1, T> {
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
using vec2i = vec<2, int>;
using vec2f = vec<2, float>;
using vec2 = vec<2, double>;

using vec3i = vec<3, int>;
using vec3f = vec<3, float>;
using vec3 = vec<3, double>;

using vec4i = vec<4, int>;
using vec4f = vec<4, float>;
using vec4 = vec<4, double>;

using mat3f = mat<3, 3, float>;
using mat3 = mat<3, 3, double>;

using mat4f = mat<4, 4, float>;
using mat4 = mat<4, 4, double>;

using point3 = vec3;
using rtcolor = vec3;






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
inline T Lerp(const T startValue, const T endValue, const float t)
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




// Utility Functions
inline double random_double()
{
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double_range(double min, double max)
{
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

inline int random_int(int low, int high)
{
    // Returns a random integer in [min,max].
    return static_cast<int>(random_double_range(low, high + 1));
}

inline vec3 random_vec3() {
    return vec3(random_double(), random_double(), random_double());
}

inline vec3 random_vec3_range(float min, float max) {
    return vec3(random_double_range(min, max), random_double_range(min, max), random_double_range(min, max));
}


vec3 random_in_unit_disk();
vec3 random_unit_vector();
vec3 random_in_unit_sphere();
vec3 random_in_hemisphere(const vec3& normal);

template <typename T>
vec<3, T> reflect(const vec<3, T>& v, const vec<3, T>& n) {
    return v - 2 * dot(v, n) * n;
}

template <typename T>
vec<3, T> refract(const vec<3, T>& uv, const vec<3, T>& n, double etai_over_etat)
{
    auto cos_theta = fmin(dot(-uv, n), 1.0);
    vec<3, T> r_out_parallel = etai_over_etat * (uv + cos_theta * n);
    vec<3, T> r_out_perp = -sqrt(1.0 - r_out_parallel.lengthSquared()) * n;
    return r_out_parallel + r_out_perp;
}

#endif
