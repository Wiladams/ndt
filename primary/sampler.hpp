#pragma once



#include <algorithm>
#include <array>
#include <utility>
#include <vector>

#include "maths.hpp"
#include "rangen.h"

namespace maths
{
	using std::vector;
}

//============================================
// DECLARATION - MONTECARLO SAMPLING
//============================================
// 
namespace maths 
{
	inline vec3f sample_hemisphere(const vec2f& ruv);
	inline float sample_hemisphere_pdf(const vec3f& direction);

	// Sample a hemispherical directin with uniform distribution
	inline vec3f sample_hemisphere(const vec3f& normal, const vec2f& ruv);
	inline float sample_hemisphere_pdf(const vec3f& normal, const vec3f& direction);

	// Sample a spherical direction with uniform distribution
	inline vec3f sample_sphere(vec2f& ruv);
	inline float sample_sphere_pdf(const vec3f& w);

	// Sample an hemispherical direction with cosine distribution.
	inline vec3f sample_hemisphere_cos(const vec2f& ruv);
	inline float sample_hemisphere_cos_pdf(const vec3f& direction);

	// Sample an hemispherical direction with cosine distribution.
	inline vec3f sample_hemisphere_cos(const vec3f& normal, const vec2f& ruv);
	inline float sample_hemisphere_cos_pdf(
		const vec3f& normal, const vec3f& direction);

	// Sample an hemispherical direction with cosine power distribution.
	inline vec3f sample_hemisphere_cospower(float exponent, const vec2f& ruv);
	inline float sample_hemisphere_cospower_pdf(
		float exponent, const vec3f& direction);

	// Sample an hemispherical direction with cosine power distribution.
	inline vec3f sample_hemisphere_cospower(
		float exponent, const vec3f& normal, const vec2f& ruv);
	inline float sample_hemisphere_cospower_pdf(
		float exponent, const vec3f& normal, const vec3f& direction);

	// Sample a point uniformly on a disk.
	inline vec2f sample_disk(const vec2f& ruv);
	inline float sample_disk_pdf(const vec2f& point);

	// Sample a point uniformly on a cylinder, without caps.
	inline vec3f sample_cylinder(const vec2f& ruv);
	inline float sample_cylinder_pdf(const vec3f& point);

	// Sample a point uniformly on a triangle returning the baricentric coordinates.
	inline vec2f sample_triangle(const vec2f& ruv);

	// Sample a point uniformly on a triangle.
	inline vec3f sample_triangle(
		const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec2f& ruv);
	// Pdf for uniform triangle sampling, i.e. triangle area.
	inline float sample_triangle_pdf(
		const vec3f& p0, const vec3f& p1, const vec3f& p2);

	// Sample an index with uniform distribution.
	inline int   sample_uniform(int size, float r);
	inline float sample_uniform_pdf(int size);

	// Sample an index with uniform distribution.
	inline float sample_uniform(const vector<float>& elements, float r);
	inline float sample_uniform_pdf(const vector<float>& elements);

	// Sample a discrete distribution represented by its cdf.
	inline int sample_discrete(const vector<float>& cdf, float r);
	// Pdf for uniform discrete distribution sampling.
	inline float sample_discrete_pdf(const vector<float>& cdf, int idx);

}


//====================================================
// IMPLEMENTATION - Montecarlo Sampling
//====================================================
// 
namespace maths
{
	// Sample an hemispherical direction with uniform distribution.
	inline vec3f sample_hemisphere(const vec2f& ruv) {
		auto z = ruv.y;
		auto r = sqrt(clamp(1 - z * z, 0.0f, 1.0f));
		auto phi = 2 * pif * ruv.x;
		return { r * cos(phi), r * sin(phi), z };
	}
	inline float sample_hemisphere_pdf(const vec3f& direction) {
		return (direction.z <= 0) ? 0 : 1 / (2 *pif);
	}

	// Sample an hemispherical direction with uniform distribution.
	inline vec3f sample_hemisphere(const vec3f& normal, const vec2f& ruv) {
		auto z = ruv.y;
		auto r = sqrt(clamp(1 - z * z, 0.0f, 1.0f));
		auto phi = 2 * pif * ruv.x;
		auto local_direction = vec3f{ r * cos(phi), r * sin(phi), z };
		return transform_direction(basis_fromz(normal), local_direction);
	}


	inline float sample_hemisphere_pdf(
		const vec3f& normal, const vec3f& direction) {
		return (dot(normal, direction) <= 0) ? 0 : 1 / (2 * pif);
	}

	// Sample a spherical direction with uniform distribution.
	inline vec3f sample_sphere(const vec2f& ruv) {
		auto z = 2 * ruv.y - 1;
		auto r = sqrt(clamp(1 - z * z, 0.0f, 1.0f));
		auto phi = 2 * pif * ruv.x;
		return { r * cos(phi), r * sin(phi), z };
	}
	inline float sample_sphere_pdf(const vec3f& w) { return 1 / (4 * pif); }

	// Sample an hemispherical direction with cosine distribution.
	inline vec3f sample_hemisphere_cos(const vec2f& ruv) {
		auto z = sqrt(ruv.y);
		auto r = sqrt(1 - z * z);
		auto phi = 2 * pif * ruv.x;
		return { r * cos(phi), r * sin(phi), z };
	}
	inline float sample_hemisphere_cos_pdf(const vec3f& direction) {
		return (direction.z <= 0) ? 0 : direction.z / pif;
	}

	// Sample an hemispherical direction with cosine distribution.
	
	inline vec3f sample_hemisphere_cos(const vec3f& normal, const vec2f& ruv) {
		auto z = sqrt(ruv.y);
		auto r = sqrt(1 - z * z);
		auto phi = 2 * pif * ruv.x;
		auto local_direction = vec3f{ r * cos(phi), r * sin(phi), z };
		return transform_direction(basis_fromz(normal), local_direction);
	}
	

	inline float sample_hemisphere_cos_pdf(
		const vec3f& normal, const vec3f& direction) {
		auto cosw = dot(normal, direction);
		return (cosw <= 0) ? 0 : cosw / pif;
	}

	// Sample an hemispherical direction with cosine power distribution.
	inline vec3f sample_hemisphere_cospower(float exponent, const vec2f& ruv) {
		auto z = pow(ruv.y, 1 / (exponent + 1));
		auto r = sqrt(1 - z * z);
		auto phi = 2 * pif * ruv.x;
		return { r * cos(phi), r * sin(phi), z };
	}
	inline float sample_hemisphere_cospower_pdf(
		float exponent, const vec3f& direction) {
		return (direction.z <= 0)
			? 0
			: pow(direction.z, exponent) * (exponent + 1) / (2 * pif);
	}

	// Sample an hemispherical direction with cosine power distribution.
	
	inline vec3f sample_hemisphere_cospower(
		float exponent, const vec3f& normal, const vec2f& ruv) {
		auto z = pow(ruv.y, 1 / (exponent + 1));
		auto r = sqrt(1 - z * z);
		auto phi = 2 * pif * ruv.x;
		auto local_direction = vec3f{ r * cos(phi), r * sin(phi), z };
		return transform_direction(basis_fromz(normal), local_direction);
	}
	

	inline float sample_hemisphere_cospower_pdf(
		float exponent, const vec3f& normal, const vec3f& direction) {
		auto cosw = dot(normal, direction);
		return (cosw <= 0) ? 0 : pow(cosw, exponent) * (exponent + 1) / (2 * pif);
	}

	// Sample a point uniformly on a disk.
	inline vec2f sample_disk(const vec2f& ruv) {
		auto r = sqrt(ruv.y);
		auto phi = 2 * pif * ruv.x;
		return { cos(phi) * r, sin(phi) * r };
	}
	inline float sample_disk_pdf() { return 1 / pif; }

	// Sample a point uniformly on a cylinder, without caps.
	inline vec3f sample_cylinder(const vec2f& ruv) {
		auto phi = 2 * pif * ruv.x;
		return { sin(phi), cos(phi), ruv.y * 2 - 1 };
	}
	inline float sample_cylinder_pdf(const vec3f& point) { return 1 / pif; }

	// Sample a point uniformly on a triangle returning the baricentric coordinates.
	inline vec2f sample_triangle(const vec2f& ruv) {
		return { 1 - sqrt(ruv.x), ruv.y * sqrt(ruv.x) };
	}

	// Sample a point uniformly on a triangle.
	inline vec3f sample_triangle(
		const vec3f& p0, const vec3f& p1, const vec3f& p2, const vec2f& ruv) {
		auto uv = sample_triangle(ruv);
		return p0 * (1 - uv.x - uv.y) + p1 * uv.x + p2 * uv.y;
	}
	// Pdf for uniform triangle sampling, i.e. triangle area.
	inline float sample_triangle_pdf(
		const vec3f& p0, const vec3f& p1, const vec3f& p2) {
		return 2 / length(cross(p1 - p0, p2 - p0));
	}

	// Sample an index with uniform distribution.
	inline int sample_uniform(int size, float r) {
		return clamp((int)(r * size), 0, size - 1);
	}
	inline float sample_uniform_pdf(int size) { return (float)1 / (float)size; }

	// Sample an index with uniform distribution.
	inline float sample_uniform(const vector<float>& elements, float r) {
		if (elements.empty()) return {};
		auto size = (int)elements.size();
		return elements[clamp((int)(r * size), 0, size - 1)];
	}
	inline float sample_uniform_pdf(const vector<float>& elements) {
		if (elements.empty()) return 0;
		return 1.0f / (int)elements.size();
	}

	// Sample a discrete distribution represented by its cdf.
	inline int sample_discrete(const vector<float>& cdf, float r) {
		r = clamp(r * cdf.back(), (float)0, cdf.back() - (float)0.00001);
		auto idx = (int)(std::upper_bound(cdf.data(), cdf.data() + cdf.size(), r) -
			cdf.data());
		return clamp(idx, 0, (int)cdf.size() - 1);
	}
	// Pdf for uniform discrete distribution sampling.
	inline float sample_discrete_pdf(const vector<float>& cdf, int idx) {
		if (idx == 0) return cdf.at(0);
		return cdf.at(idx) - cdf.at(idx - 1);
	}


}

// Sampling Data structures
template <typename T>
struct ISampler1D
{
	virtual T operator()(float u) = 0;
};

template <typename T>
struct ISampler2D
{
	virtual T operator()(float u, float v) = 0;
};

template <typename T>
struct ISampler3D
{
	virtual T operator()(float u, float v, float w) = 0;
};

