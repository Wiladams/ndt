#pragma once

#include "maths.hpp"

//=========================================
// DECLARATION AXIS ALIGNED BOUNDING BOX
//=========================================

namespace maths
{
struct bbox2f 
{
	vec2f min = { flt_max, flt_max };
	vec2f max = { flt_min,flt_min };

	inline vec2f&		operator[](int i);
	inline const vec2f& operator[](int i) const;
};

struct bbox3f
{
	vec3f min = { flt_max, flt_max, flt_max };
	vec3f max = { flt_min, flt_min, flt_min };

	inline vec3f&		operator[](int i);
	inline const vec3f& operator[](int i) const;
};

// Empty box constants
constexpr auto invalidb2f = bbox2f{};
constexpr auto invalidb3f = bbox3f{};

//
// Bounding box properties
inline vec2f center(const bbox2f& a);
inline vec2f size(const bbox2f& a);

// Bounding box comparisons
inline bool operator==(const bbox2f& a, const bbox2f& b);
inline bool operator!=(const bbox2f& a, const bbox2f& b);

// Bounding Box expansion
inline bbox2f merge(const bbox2f& a, const vec2f& b);
inline bbox2f merge(const bbox2f& a, const bbox2f& b);
inline void expand(bbox2f& a, const vec2f& b);
inline void expand(bbox2f& a, const bbox2f& b);

//
// Bounding box properties (vec3f)
inline vec3f center(const bbox3f& a);
inline vec3f size(const bbox3f& a);

// Bounding box comparisons (bbox3f)
inline bool operator==(const bbox3f& a, const bbox3f& b);
inline bool operator!=(const bbox3f& a, const bbox3f& b);

// Bounding Box expansion
inline bbox3f merge(const bbox3f& a, const vec3f& b);
inline bbox3f merge(const bbox3f& a, const bbox3f& b);
inline void expand(bbox3f& a, const vec3f& b);
inline void expand(bbox3f& a, const bbox3f& b);



}

namespace maths
{
	// Calculating normals
	// https://stackoverflow.com/questions/38499211/get-normal-of-bezier-curve-in-2d
	// 
	// Evaluate a cubic bezier curve along the specified
	// parameter 'u'
template <typename T>
inline T bezier_cubic_eval(const T& p0, const T& p1, const T& p2, const T& p3, float u)
{
	return p0 * (1.0f - u) * (1.0f - u) * (1.0f - u) + 
		p1 * 3 * u * (1.0f - u) * (1.0f - u) + 
		p2 * 3 * u * u * (1.0 - u) + 
		p3 * u * u * u;
}

template <typename T>
inline T interpolate_bezier_derivative(const T& p0, const T& p1, const T& p2, const T& p3, float u)
{
	return (p1 - p0) * 3 * (1.0f - u) * (1.0f - u) + 
		(p2 - p1) * 6 * u * (1.0f - u) +
		(p3 - p2) * 3 * u * u;
}

}