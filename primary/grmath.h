#pragma once

#ifndef VEC3_H
#define VEC3_H

/*
//
//    grmath.h
//
//    This file contains the data types and math routines
//    that are typically used in the creation of various
//    kinds of graphics.
*/

#include <cmath>
#include <iostream>
#include <cassert>

#include "maths.hpp"

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

    // returning const allows you to get the value
    // and does not allow changing it
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

template<size_t DIM, typename T>
vec<DIM, T> operator-(const vec<DIM, T> &a, const vec<DIM, T>& b) 
{
    vec<DIM, T> res;

    for (size_t i = DIM; i--; res[i] = a[i] - b[i]);

    return res;
}

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

    vec() : x(T()), y(T()) {}
    vec(T X, T Y) : x(X), y(Y) {}

    template <class U> vec<2, T>(const vec<2, U>& v);

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

    T length() { return std::sqrt(x * x + y * y); }
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


    vec() : data{0,0,0} {}
    vec(T X, T Y, T Z) : data{ X,Y,Z } {}

    // Copy constructor from other type
    template <class U> vec<3, T>(const vec<3, U>& v);

    // Negation
    vec<3, T> operator-() const {return vec<3, T>(-data[0], -data[1], -data[2]);}
    T operator[](const size_t i) const {return data[i];}        // retrieving value
    T& operator[](const size_t i) { return data[i]; }              // setting value

    vec<3, T>& operator +=(const vec<3, T>& rhs)
    {
        data[0] += rhs.data[0];
        data[1] += rhs.data[1];
        data[2] += rhs.data[2];

        return *this;
    }
    /*
    vec<3, T>& operator -=(const vec<3, T>& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;

        return *this;
    }
    */
    
    vec<3, T>& operator *=(const vec<3, T>& rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;

        return *this;
    }
    
    vec<3, T>& operator *=(const double t)
    {
        data[0] *= t;
        data[1] *= t;
        data[2] *= t;

        return *this;
    }

    vec<3, T>& operator /=(const double t)
    {
        return *this *= 1 / t;
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

    T lengthSquared() const
    {
        return (data[0] * data[0] + data[1] * data[1] + data[2] * data[2]);
    }

    T length() const
    {
        return std::sqrt(lengthSquared());
    }

    // Normalize the current vector in place
    vec<3, T>& normalize(T l = 1)
    {
        *this = (*this) * (l / length()); 
        return *this;
    }


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
    return vec<3,T>(u.data[0] * v.data[0], u.data[1] * v.data[1], u.data[2] * v.data[2]);
}

template <typename T>
inline vec<3, T> operator*(T t, const vec<3, T>& v) {
    return vec<3, T>(t * v.data[0], t * v.data[1], t * v.data[2]);
}

template <typename T>
inline vec<3, T> operator*(const vec<3, T>& v, T t)
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
*/
template<size_t DIM, typename T, typename U> 
vec<DIM, T> operator/(vec<DIM, T> lhs, const U& rhs) {
    for (size_t i = DIM; i--; lhs[i] /= rhs);

    return lhs;
}



/*
// Simple arithmetic operators



*/
// hadamard
//template <typename T>
//inline vec<3,T> mul(const vec<3, T>& u, const vec<3, T>& v) {
//    return vec<3,T>(u.data[0] * v.data[0], u.data[1] * v.data[1], u.data[2] * v.data[2]);
//}


//template <typename T>
//inline vec<3,T> operator/(const vec<3, T>& v, const double t)
//{
//    return (1 / t) * v;
//}



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
//using vec2 = vec<2, float>; 
//using ivec2 = vec<2, int>;
//using vec2i = vec<2, int>;
//using vec2f = vec<2, float>;
//using dvec2  = vec<2, double>;


//using vec3i = vec<3, int>;
//using vec3f = vec<3, float>;
//using vec3  = vec<3, double>;

//using vec4i = vec<4, int>;
//using vec4f = vec<4, float>;
//using vec4  = vec<4, double>;

//using mat3f = mat<3, 3, float>;
//using mat3  = mat<3, 3, double>;

//using mat4f = mat<4, 4, float>;
//using mat4  = mat<4, 4, double>;

//using point3 = vec3;
//using rtcolor = vec3;


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
/*
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
*/
#endif
