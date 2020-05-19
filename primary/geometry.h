#pragma once

/*
	Borrowed from tiny renderer
	https://github.com/ssloy/tinyrenderer
*/

#include <cmath>
#include <vector>
#include <cassert>
#include <iostream>

//namespace geometry {
	// predefine matrix so matrix to vector calculations
	// can occur.
	template<size_t DimCols, size_t DimRows, typename T> class mat;

	/*
		General Vector structure.  Can be specialized to
		any single dimension.
	*/
	template <size_t DIM, typename T> struct vec
	{
		// default constructor, sets data values to 
		// default of the specified data type
		vec() {
			for (size_t i = 0; i < DIM; i++)
				data_[i] = T();
			//for (size_t i = DIM; i--; data_[i] = T()); 
		}

		// allows setting a value
		// vec[2] = value;
		T& operator[](const size_t i) {
			assert(i < DIM);
			return data_[i];
		}

		// allows getting a value
		// value = vec[2];
		const T& operator[](const size_t i) const {
			assert(i < DIM);
			return data_[i];
		}

	private:
		T data_[DIM];
	};
	


	/////////////////////////////////////////////////////////////////////////////////
	// Two dimensional vector of specified type
	template <typename T> struct vec<2, T>
	{
		vec() : x(T()), y(T()) {}
		vec(T X, T Y) : x(X), y(Y) {}

		template <class U> vec<2, T>(const vec<2, U>& v);
		//template<class U> vec<2,T>(const vec<2, U> &v) : x(v[0]), y(v[1]) {}

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

		T x, y;
	};

	/*
	// Need something like the following to make the above compile
	template<typename T>
	template<class U>
	inline vec<2, T>::vec(const vec<2, U>& v)
	{

	}
	*/

	/////////////////////////////////////////////////////////////////////////////////
	// Three dimensional vector of specified type
	template <typename T> struct vec<3, T>
	{
		vec() : x(T()), y(T()), z(T())
		{}

		vec(T X, T Y, T Z) : x(X), y(Y), z(Z)
		{}

		// Copy constructor
		template <class U> vec<3, T>(const vec<3, U>& v);

		T& operator[](const size_t i)
		{
			assert(i < 3);
			return i <= 0 ? x : (1 == i ? y : z);
		}

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

		vec<3, T> lerp(const vec<3, T>& other, float t) const
		{
			vec<3, T> res();
			return res;
		}

		float norm() const
		{
			return std::sqrt(x * x + y * y + z * z);
		}

		vec<3, T>& normalize(T l = 1)
		{
			*this = (*this) * (l / norm()); return *this;
		}

		T x, y, z;
	};

	/////////////////////////////////////////////////////////////////////////////////
	/*
		Operations on vectors
	*/
	// Negation
	template <typename T> vec<3, T> operator -(const vec<3, T> rhs)
	{
		vec <3, T> res(-rhs.x, -rhs.y, -rhs.z);
		return res;
	}


	template<size_t DIM, typename T> T operator*(const vec<DIM, T>& lhs, const vec<DIM, T>& rhs) {
		T ret = T();
		for (size_t i = DIM; i--; ret += lhs[i] * rhs[i]);

		return ret;
	}



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

	template<size_t DIM, typename T, typename U> vec<DIM, T> operator/(vec<DIM, T> lhs, const U& rhs) {
		for (size_t i = DIM; i--; lhs[i] /= rhs);

		return lhs;
	}

	template<size_t LEN, size_t DIM, typename T> vec<LEN, T> embed(const vec<DIM, T>& v, T fill = 1) {
		vec<LEN, T> ret;
		for (size_t i = LEN; i--; ret[i] = (i < DIM ? v[i] : fill));

		return ret;
	}

	template<size_t LEN, size_t DIM, typename T> vec<LEN, T> proj(const vec<DIM, T>& v) {
		vec<LEN, T> ret;
		for (size_t i = LEN; i--; ret[i] = v[i]);

		return ret;
	}




	// Cross product of two matrices
	// returns a vector
	// vec v3 = cross(v1, v2);
	// 
	template <typename T> vec<3, T> cross(vec<3, T> v1, vec<3, T> v2) {
		return vec<3, T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
	}

	// Dot product of two matrices
	// returns a single value
	// double dot(v1, v2);
	//
	template <typename T> T dot(vec<3, T> v1, vec<3, T> v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template <size_t DIM, typename T> std::ostream& operator<<(std::ostream& out, vec<DIM, T>& v) {
		for (unsigned int i = 0; i < DIM; i++) {
			out << v[i] << " ";
		}
		return out;
	}


	/////////////////////////////////////////////////////////////////////////////////

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
	template<size_t DimRows, size_t DimCols, typename T> class mat {
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
	typedef vec<2, float> Vec2f;
	typedef vec<2, int>   Vec2i;
	typedef vec<3, float> Vec3f;
	typedef vec<3, int>   Vec3i;
	typedef vec<4, float> Vec4f;
	typedef mat<4, 4, float> Matrix;
	typedef mat<3, 3, float> Matrix3;

//}
