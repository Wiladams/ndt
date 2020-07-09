#pragma once

#include <cmath>
#include <cstdint>
#include "maths.hpp"

/*
	Yet another way to do the 3D math library
*/

using namespace maths;

namespace math3d {

	/*
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
	};
	*/
	struct Matrix2x2 {
		FLOAT m[2][2];

		FLOAT determinant()
		{
			return m[0][0] * m[1][1] - m[0][1] * m[1][0];
		}
	};

	struct Matrix3x3 {
		FLOAT m[3][3];


		Matrix3x3 transpose() const
		{
			Matrix3x3 res;
			for (size_t row = 0; row < 3; row++)
				for (size_t column = 0; column < 3; column++)
					res.m[row][column] = m[column][row];
			return res;
		}

		FLOAT determinant() const
		{
			
		}
	};

	struct Matrix4x4 {
		FLOAT m[4][4];

		// Initialize to identity matrix by default
		Matrix4x4()
			:m{{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}
		{
		}

		Matrix4x4(FLOAT val[4][4]);


		Matrix4x4(
			FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
			FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
			FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
			FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33);

		
		Matrix4x4 transpose() const;

	};


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


	// Lerp
	// This implementation of LERP is the most accurate
	// and guarantees you get v1 at 0.0 and v2 at 1.0
	template <typename T>
	inline T Lerp(T t, T v1, T v2)
	{
		return (1 - t) * v1 + t * v2;
	}


	// This is a replacement for '%'
	// particularly for integer types
	// 
	template <typename T>
	inline T Mod(T a, T b) {
		T result = a - (a / b) * b;
		return (T)((result < 0) ? result + b : result);
	}

	// specialization for floats
	template <>
	inline FLOAT Mod(FLOAT a, FLOAT b) {
		return std::fmod(a, b);
	}

	inline FLOAT Degrees(FLOAT rad) { return rad * 57.29577951308232; } // rad * (180/Pi)
	inline FLOAT Radians(FLOAT x) { return x * 0.017453292519943295; }	// deg * (Pi/180)

	template <typename T>
	inline constexpr bool IsPowerOf2(T v) {return v && !(v & (v - 1));}
}