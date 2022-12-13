#pragma once

#ifndef SVGXFORM_H
#define SVGXFORM_H

#include "maths.hpp"

//
// A class to represent a transformation matrix for SVG graphics
// The matrix is represented by an array of float values (float[6])
// 

// Forward declarations
namespace svg 
{
	static INLINE void svg_xformIdentity(float* t) noexcept;
	static INLINE void svg_xformSetTranslation(float* t, float tx, float ty) noexcept;
	static INLINE void svg_xformSetScale(float* t, float sx, float sy) noexcept;
	static INLINE void svg_xformSetSkewX(float* t, float a) noexcept;
	static INLINE void svg_xformSetSkewY(float* t, float a) noexcept;
	static INLINE void svg_xformSetRotation(float* t, float a) noexcept;
	static INLINE void svg_xformMultiply(float* t, const float* s) noexcept;
	static INLINE void svg_xformPremultiply(float* t, const float* s) noexcept;
	static INLINE void svg_xformInverse(const float* t, float* inv) noexcept;
	static INLINE void svg_xformPoint(float* t, float* dx, float* dy, float x, float y) noexcept;
	static INLINE void svg_xformVec(float* t, float* dx, float* dy, float x, float y) noexcept;

	
}


namespace svg {

using namespace maths;

static INLINE void svg_xformIdentity(float* t) noexcept
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static INLINE void svg_xformSetTranslation(float* t, float tx, float ty) noexcept
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = tx; t[5] = ty;
}

static INLINE void svg_xformSetScale(float* t, float sx, float sy) noexcept
{
	t[0] = sx; t[1] = 0.0f;
	t[2] = 0.0f; t[3] = sy;
	t[4] = 0.0f; t[5] = 0.0f;
}

static INLINE void svg_xformSetSkewX(float* t, float a) noexcept
{
	t[0] = 1.0f; t[1] = 0.0f;
	t[2] = tanf(a); t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static INLINE void svg_xformSetSkewY(float* t, float a) noexcept
{
	t[0] = 1.0f; t[1] = tanf(a);
	t[2] = 0.0f; t[3] = 1.0f;
	t[4] = 0.0f; t[5] = 0.0f;
}

static INLINE void svg_xformSetRotation(float* t, float a) noexcept
{
	float cs = cosf(a), sn = sinf(a);
	t[0] = cs; t[1] = sn;
	t[2] = -sn; t[3] = cs;
	t[4] = 0.0f; t[5] = 0.0f;
}

static INLINE void svg_xformMultiply(float* t, float* s) noexcept
{
	float t0 = t[0] * s[0] + t[1] * s[2];
	float t2 = t[2] * s[0] + t[3] * s[2];
	float t4 = t[4] * s[0] + t[5] * s[2] + s[4];
	t[1] = t[0] * s[1] + t[1] * s[3];
	t[3] = t[2] * s[1] + t[3] * s[3];
	t[5] = t[4] * s[1] + t[5] * s[3] + s[5];
	t[0] = t0;
	t[2] = t2;
	t[4] = t4;
}

static INLINE void svg_xformInverse(float* t, float* inv) noexcept
{
	double invdet, det = (double)t[0] * t[3] - (double)t[2] * t[1];
	if (det > -1e-6 && det < 1e-6) {
		svg_xformIdentity(t);
		return;
	}
	invdet = 1.0 / det;
	inv[0] = (float)(t[3] * invdet);
	inv[2] = (float)(-t[2] * invdet);
	inv[4] = (float)(((double)t[2] * t[5] - (double)t[3] * t[4]) * invdet);
	inv[1] = (float)(-t[1] * invdet);
	inv[3] = (float)(t[0] * invdet);
	inv[5] = (float)(((double)t[1] * t[4] - (double)t[0] * t[5]) * invdet);
}

static INLINE void svg_xformPremultiply(float* t, float* s) noexcept
{
	float s2[6];
	memcpy(s2, s, sizeof(float) * 6);
	svg_xformMultiply(s2, t);
	memcpy(t, s2, sizeof(float) * 6);
}

static INLINE void svg_xformPoint(float* t, float* dx, float* dy, float x, float y ) noexcept
{
	*dx = x * t[0] + y * t[2] + t[4];
	*dy = x * t[1] + y * t[3] + t[5];
}

// A vector, no need to apply translation
static INLINE void svg_xformVec(float* t, float* dx, float* dy, float x, float y) noexcept
{
	*dx = x * t[0] + y * t[2];
	*dy = x * t[1] + y * t[3];
}
}


#endif	// SVGXFORM_H