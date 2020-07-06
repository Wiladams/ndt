#ifndef RTTYPES_H
#define RTTYPES_H

#pragma once
/*
    Data Types used for ray tracing
*/

#include <cstdint>
#include <cmath>
#include <iostream>
#include <cassert>
#include <limits>

using std::sqrt;

typedef float FLOAT;

// pre-define matrix type so vector routines can use them
//template<size_t DimCols, size_t DimRows, typename T> class mat;

template <size_t DIM, typename T>
struct RTvec
{
private:
    T data[DIM];

public:
    // default constructor, sets data values to 
    // default of the specified data type
    RTvec()
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


    RTvec<DIM, T>& operator+=(const RTvec<DIM, T>& rhs)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] += rhs[i];

        return *this;
    }

    RTvec<DIM, T>& operator-=(const RTvec<DIM, T>& rhs)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] -= rhs[i];

        return *this;
    }

    RTvec<DIM, T>& operator*=(const T s)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] *= s;

        return *this;
    }

    RTvec<DIM, T>& operator/=(const T s)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] /= s;

        return *this;
    }

    const double lengthSquared() const
    {
        double res = 0;
        for (size_t i = 0; i < DIM; i++) {
            res += data[i] * data[i];
        }

        return res;
    }

    double length() const
    {
        return std::sqrt(lengthSquared());
    }
};

// Some basic arithmetic overloads
// Negation
template <size_t DIM, typename T>
inline RTvec<DIM,T> operator-(const RTvec<DIM, T>& a)
{
    RTvec<DIM, T> res;
    for (size_t i = 0; i < DIM; i++)
        res[i] = -a[i];

    return res;
}

// Arithmetic - Addition
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator+(const RTvec<DIM, T>& a, const RTvec<DIM, T>& b)
{
    RTvec<DIM, T> res(a);
    return res += b;
}

// Arithmetic - Subtraction
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator-(const RTvec<DIM, T>& a, const RTvec<DIM, T>& b)
{
    RTvec<DIM, T> res(a);
    return res -= b;
}

// Arithmetic - Scalar Multiplication
// scalar on the right
// RTvec * s;
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator*(const RTvec<DIM,T>& a, const T s)
{
    RTvec<DIM, T> res(a);
    return res *= s;
}

// Arithmetic - Scalar Multiplication
// scalar on the left
// s * RTvec;
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator*(const T s, const RTvec<DIM,T>& a)
{
    RTvec<DIM, T> res(a);
    return res *= s;
}

// Arithmetic - Scalar division
// scalar on the right
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator/(const RTvec<DIM, T>& a, const T s)
{
    RTvec<DIM, T> res(a);
    return res /= s;
}

// Relational operators
// Test for equality, using epsilon
template <size_t DIM, typename T>
inline bool operator==(const RTvec<DIM,T>&lhs, const RTvec<DIM, T>& rhs)
{
    for (size_t i = 0; i < DIM; i++) {
        if (std::abs(lhs[0] - rhs[0]) < FLT_EPSILON)
            return false;
    }

    return true;
}


// any-dimensional dot product
template <size_t DIM, typename T>
inline T dot(const RTvec<DIM, T>& a, const RTvec<DIM, T>& b)
{
    T res = T();
    for (size_t i = 0; i < DIM; i++)
        res += a.data[i] * b.data[i];

    return res;
}

// magnitude, or length of vector
template <size_t DIM, typename T>
inline T magnitude(const RTvec<DIM,T> & a)
{
    return std::sqrt(dot(a, a));
}

// Return a unit vector
template <size_t DIM, typename T>
inline RTvec<DIM,T> unit_vector(const RTvec<DIM, T>& a)
{
    auto magmul = 1.0 / magnitude(a);
    return a * magmul;
}

// Hadamard, or Schur product
// used for color combination
template <size_t DIM, typename T>
inline RTvec<DIM, T> hadamard_product(const RTvec<DIM,T> & a, const RTvec<DIM, T>& b)
{
    RTvec<DIM, T> res(a);
    for (size_t i = 0; i < DIM; i++)
        res[i] *= b[i];
    
    return res;
}


typedef RTvec<2, FLOAT> RTFloat2;
typedef RTvec<3, FLOAT> RTFloat3;



struct RTFloat4: public RTvec<4,FLOAT>
{
    RTFloat4()
        : RTvec<4,FLOAT>()
    {
    }

    RTFloat4(const RTFloat4& other)
    {
        this->operator[](0) = other[0];
        this->operator[](1) = other[1];
        this->operator[](2) = other[2];
        this->operator[](3) = other[3];
    }

    RTFloat4(FLOAT a, FLOAT b, FLOAT c, FLOAT d)
    {
        this->operator[](0) = a;
        this->operator[](1) = b;
        this->operator[](2) = c;
        this->operator[](3) = d;
    }


    bool isPoint() { return this->operator[](3) != 0; }

    static RTFloat4 Point(FLOAT x, FLOAT y, FLOAT z) { return RTFloat4(x, y, z, 1); }
    static RTFloat4 Vector(FLOAT x, FLOAT y, FLOAT z) { return RTFloat4(x, y, z, 0); }
    static RTFloat4 Color(FLOAT x, FLOAT y, FLOAT z) { return RTFloat4(x, y, z, 0); }

};



// 3-dimensional cross product
inline RTFloat4 cross(const RTFloat4& a, const RTFloat4& b)
{
    return RTFloat4::Vector(
        a[1]*b[2] - a[2]*b[1],
        a[2]*b[0] - a[0]*b[2],
        a[0]*b[1] - a[1]*b[0]);
}






/*
    Matrices are row major
*/
/*
    Data structure representing Determinant of a matrix
*/
template<size_t DIM, typename T>
struct RTdt {
    static T det(const mat<DIM, DIM, T>& src) {
        T ret = 0;
        for (size_t i = DIM; i--; ret += src[0][i] * src.cofactor(0, i));
        return ret;
    }
};

template<typename T>
struct RTdt<1, T> {
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
class RTmat {
    vec<DimCols, T> rows[DimRows];
public:
    RTmat() {}

    RTvec<DimCols, T>& operator[] (const size_t idx) {
        assert(idx < DimRows);
        return rows[idx];
    }

    const RTvec<DimCols, T>& operator[] (const size_t idx) const {
        assert(idx < DimRows);
        return rows[idx];
    }

    // Get a column value of a matrix
    RTvec<DimRows, T> col(const size_t idx) const {
        assert(idx < DimCols);
        RTvec<DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = rows[i][idx]);
        return ret;
    }

    // Set a particular column of a matrix
    // mat.set_col(1, vec);
    //
    void set_col(size_t idx, RTvec<DimRows, T> v) {
        assert(idx < DimCols);
        for (size_t i = DimRows; i--; rows[i][idx] = v[i]);
    }

    // create an identity matrix
    // mat res = mat::identity();
    //
    static RTmat<DimRows, DimCols, T> identity() {
        RTmat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = (i == j));
        return ret;
    }

    // Determinant of a matrix
    T det() const {
        return RTdt<DimCols, T>::det(*this);
    }

    RTmat<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const {
        RTmat<DimRows - 1, DimCols - 1, T> ret;
        for (size_t i = DimRows - 1; i--; )
            for (size_t j = DimCols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        return ret;
    }

    // Cofactor matrix of a matrix
    T cofactor(size_t row, size_t col) const {
        return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
    }

    RTmat<DimRows, DimCols, T> adjugate() const {
        RTmat<DimRows, DimCols, T> ret;
        for (size_t i = DimRows; i--; )
            for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j));
        return ret;
    }

    RTmat<DimRows, DimCols, T> invert_transpose() {
        RTmat<DimRows, DimCols, T> ret = adjugate();
        T tmp = ret[0] * rows[0];
        return ret / tmp;
    }

    RTmat<DimRows, DimCols, T> invert() {
        return invert_transpose().transpose();
    }

    RTmat<DimCols, DimRows, T> transpose() {
        RTmat<DimCols, DimRows, T> ret;
        for (size_t i = DimCols; i--; ret[i] = this->col(i));
        return ret;
    }
};

/*
struct RTMatrix44
{
    RTFloat4 rows[4];

    // Ability to set a value
    RTFloat4& operator[](const size_t row) { return rows[row]; }

    // Ability to retrieve a value
    const RTFloat4& operator[](const size_t row) const { return rows[row]; }

    RTMatrix33 get_minor(size_t row, size_t col) const 
    {
        RTMatrix33 ret;
        for (size_t i = 3; i--; )
            for (size_t j = 3; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1]);
        
        return ret;
    }

    // Cofactor matrix of a matrix
    FLOAT cofactor(size_t row, size_t col) const 
    {
        return get_minor(row, col).deteterminant() * ((row + col) % 2 ? -1 : 1);
    }
};

// Matrix multiply by columnar matrix on the right
// Apply the matrix transform to the column
RTFloat4 operator*(const RTMatrix44& A, const RTFloat4& b)
{
    RTFloat4 c;
    for (size_t row = 0; row < 4; row++) {
        c[row] =
            A[row][0] * b[0] +
            A[row][1] * b[1] +
            A[row][2] * b[2] +
            A[row][3] * b[3];
    }

    return c;
}

// Combining two matrices together
// composite transformations
RTMatrix44 matrix_multiply(const RTMatrix44& a, const RTMatrix44& b)
{
    RTMatrix44 M;
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            M[row][col] =
                a[row][0] * b[0][col] +
                a[row][1] * b[1][col] +
                a[row][2] * b[2][col] +
                a[row][3] * b[3][col];
        }
    }
    return M;
}

// Hadamard/Schur element-by-element multiplication
//
RTMatrix44 hadamard_product(const RTMatrix44& a, const RTMatrix44& b)
{
    RTMatrix44 M;
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            M[row][col] = a[row][col] * b[row][col];
        }
    }

    return M;
}

// Create certain transformation matrices
RTMatrix44 matrix_identity()
{
    // Assuming default constructor sets everything
    // to '0' initially;
    RTMatrix44 M;
    M[0][0] = 1;
    M[1][1] = 1;
    M[2][2] = 1;
    M[3][3] = 1;
    return M;
}

RTMatrix44 matrix_transpose(const RTMatrix44& a)
{
    RTMatrix44 M;
    for (size_t row = 0; row < 4; row++) {
        for (size_t col = 0; col < 4; col++) {
            M[row][col] = a[col][row];
        }
    }
    return M;
}
*/

#endif
