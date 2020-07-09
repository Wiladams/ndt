#include "math3d.h"
#include <memory>

namespace math3d {
	Matrix4x4::Matrix4x4(FLOAT val[4][4])
	{
		memcpy(m, val, 16 * sizeof(FLOAT));
	}


	Matrix4x4::Matrix4x4(
		FLOAT m00, FLOAT m01, FLOAT m02, FLOAT m03,
		FLOAT m10, FLOAT m11, FLOAT m12, FLOAT m13,
		FLOAT m20, FLOAT m21, FLOAT m22, FLOAT m23,
		FLOAT m30, FLOAT m31, FLOAT m32, FLOAT m33)
	{
		m[0][0] = m00;
		m[0][1] = m01;
		m[0][2] = m02;
		m[0][3] = m03;

		m[1][0] = m10;
		m[1][1] = m11;
		m[1][2] = m12;
		m[1][3] = m13;

		m[2][0] = m20;
		m[2][1] = m21;
		m[2][2] = m22;
		m[2][3] = m23;

		m[3][0] = m30;
		m[3][1] = m31;
		m[3][2] = m32;
		m[3][3] = m33;
	}

	Matrix4x4 Matrix4x4::transpose() const
	{
		Matrix4x4 res;
		for (size_t row = 0; row < 4; row++)
			for (size_t column = 0; column < 4; column++)
				res.m[row][column] = m[column][row];
		return res;
	}
}
