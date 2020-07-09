

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

    // Copy constructor
    RTvec(const RTvec<DIM, T>& other)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] = other[i];
    }

    // Assignment operator
    RTvec<DIM, T>& operator=(const RTvec<DIM, T>& rhs)
    {
        for (size_t i = 0; i < DIM; i++)
            data[i] = rhs[i];

        return *this;
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

    RTvec<DIM, T> operator-() const;                // Unary minus

    T operator*(const RTvec<DIM, T>& rhs) const;    // dot product

    RTvec<DIM, T>& operator*=(const T s);
    RTvec<DIM, T>& operator+=(const RTvec<DIM, T>& rhs);
    RTvec<DIM, T>& operator-=(const RTvec<DIM, T>& rhs);



    double lengthSquared() const;   // length squared used for magnitude
    double length() const;          // length or magnitude
    RTvec<DIM,T>& normalize();               // normalize the vector in place
};


template <size_t DIM, typename T>
inline RTvec<DIM, T>& RTvec<DIM, T>::normalize()
{
    double magmul = 1.0/length();
    for (size_t i = 0; i < DIM; i++)
        data[i] *= magmul;

    return *this;
}

// Convenient output
template <size_t DIM, typename T>
inline std::ostream& operator<<(std::ostream& out, const RTvec<DIM, T>& v)
{
    for (size_t i = 0; i < DIM; i++)
        out << v[i] << ' ';

    return out;
}

// Some basic arithmetic overloads
// Negation
// return new vector
template <size_t DIM, typename T>
inline RTvec<DIM,T> RTvec<DIM, T>::operator-() const
{
    RTvec<DIM, T> res;
    for (size_t i = 0; i < DIM; i++)
        res[i] = -data[i];

    return res;
}

template <size_t DIM, typename T>
inline RTvec<DIM, T>& RTvec<DIM, T>::operator+=(const RTvec<DIM, T>& rhs)
{
    for (size_t i = 0; i < DIM; i++)
        data[i] += rhs[i];

    return *this;
}

template <size_t DIM, typename T>
inline RTvec<DIM, T>& RTvec<DIM, T>::operator-=(const RTvec<DIM, T>& rhs)
{
    for (size_t i = 0; i < DIM; i++)
        data[i] -= rhs[i];

    return *this;
}

// multiply by scalar
// return lhs
template <size_t DIM, typename T>
inline RTvec<DIM, T>& RTvec<DIM, T>::operator*=(const T s)
{
    for (size_t i = 0; i < DIM; i++)
        data[i] *= s;

    return *this;
}

// in-place
// divide by scalar
template <size_t DIM, typename T>
inline RTvec<DIM, T>& operator/=(const RTvec<DIM, T>& lhs, const T s)
{
    for (size_t i = 0; i < DIM; i++)
        lhs[i] /= s;

    return lhs;
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
// return new vector
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator*(const RTvec<DIM,T>& a, const T s)
{
    RTvec<DIM, T> res(a);
    return res *= s;
}

// Arithmetic - Scalar Multiplication
// scalar on the left
// s * RTvec;
// return new vector
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator*(const T s, const RTvec<DIM,T>& a)
{
    RTvec<DIM, T> res(a);
    return res *= s;
}

// Arithmetic - Scalar division
// scalar on the right
// return new vector
template <size_t DIM, typename T>
inline RTvec<DIM, T> operator/(const RTvec<DIM, T>& a, const T s)
{
    RTvec<DIM, T> res(a);
    return res /= s;
}

// Relational operators
// Test for equality, using epsilon
// return true/false
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
// return single value
template <size_t DIM, typename T>
inline T RTvec<DIM,T>::operator*(const RTvec<DIM, T>& b) const 
{
    T res = T();
    for (size_t i = 0; i < DIM; i++)
        res += data[i] * b[i];

    return res;
}

template <size_t DIM, typename T>
inline T dot(const RTvec<DIM, T>& a, const RTvec<DIM, T>& b)
{
    T res = T();
    for (size_t i = 0; i < DIM; i++)
        res += a[i] * b[i];

    return res;
}

template <size_t DIM, typename T>
inline double RTvec<DIM, T>::lengthSquared() const
{
    double res = 0;
    for (size_t i = 0; i < DIM; i++) {
        res += data[i] * data[i];
    }

    return res;
}

/*
// magnitude, or length of vector
template <size_t DIM, typename T>
inline T magnitude(const RTvec<DIM,T> & a)
{
    return std::sqrt(lengthSquared(a));
}
*/


template <size_t DIM, typename T>
inline double RTvec<DIM, T>::length() const
{
    return std::sqrt(lengthSquared());
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

struct RTFloat3 : public RTvec<3, FLOAT>
{
    RTFloat3()
        : RTvec<3,FLOAT>()
    {}

    RTFloat3(const RTFloat3& other)
    {
        this->operator[](0) = other[0];
        this->operator[](1) = other[1];
        this->operator[](2) = other[2];
    }

    RTFloat3(const FLOAT a, const FLOAT b, const FLOAT c)
    {
        this->operator[](0) = a;
        this->operator[](1) = b;
        this->operator[](2) = c;
    }
};


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
protected:
    RTvec<DimCols, T> rows[DimRows];

public:
    RTmat() {}

    // Convenient initializer
    RTmat(T values[DimRows][DimCols])
    {
        for (size_t row = 0; row < DimRows; row++)
            for (size_t column = 0; column < DimCols; column++)
                rows[row][column] = values[row][column];
    }

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

//
// pre-multiplay, matrix on left, columnar vector on right
// perhaps it should be spelled out instead, but this
// makes very convenient expressions
//
// v' = m * v;
//
template<size_t DimRows, size_t DimCols, typename T> 
RTvec<DimRows, T> operator*(const RTmat<DimRows, DimCols, T>& lhs, const RTvec<DimCols, T>& rhs) {
    RTvec<DimRows, T> ret;
    //for (size_t i = DimRows; i--; ret[i] = lhs[i] * rhs);
    for (size_t i = DimRows; i--; ret[i] = dot(lhs[i], rhs));

    return ret;
}

template<size_t R1, size_t C1, size_t C2, typename T>
RTmat<R1, C2, T> operator*(const RTmat<R1, C1, T>& lhs, const RTmat<C1, C2, T>& rhs) 
{
    RTmat<R1, C2, T> result;
    for (size_t i = R1; i--; ) {
        //for (size_t j = C2; j--; result[i][j] = lhs[i] * rhs.col(j));
        for (size_t j = C2; j--; result[i][j] = dot(lhs[i], rhs.col(j)));
    }

    return result;
}

//
// Scalar division
//
// m = m / s;
//
template<size_t DimRows, size_t DimCols, typename T>
RTmat<DimCols, DimRows, T> operator/(RTmat<DimRows, DimCols, T> lhs, const T& rhs) 
{
    for (size_t i = DimRows; i--; lhs[i] = lhs[i] / rhs);
    return lhs;
}



// Mainly specialize for constructor
struct RTMatrix44 : public RTmat<4, 4, FLOAT>
{
    RTMatrix44(FLOAT values[4][4])
        : RTmat<4,4,FLOAT>(values)
    {
    }
};

//
// print out
// cout << m
template <size_t DimRows, size_t DimCols, class T>
std::ostream& operator<<(std::ostream& out, RTmat<DimRows, DimCols, T>& m)
{
    for (size_t i = 0; i < DimRows; i++)
        out << m[i] << std::endl;

    return out;
}
/*
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
*/

