#pragma once

#include "blend2d.h"
#include "sampler.hpp"
#include "maths.hpp"
#include "coloring.h"

namespace ndt {
	// Given an array of gradient stops, return the highest index that is
	// lower than the specified offset
	inline size_t searchClosestLast(const BLGradientStop* array, size_t size, const double& value) noexcept
	{
		if (!size)
			return 0;

		const BLGradientStop* base = array;
		while (size_t half = size / 2u) {
			const BLGradientStop* middle = base + half;
			size -= half;
			if (middle[0].offset <= value)
				base = middle;
		}

		return size_t(base - array);
	}

	BLRgba32 lerpColor(const BLRgba32& from, const BLRgba32& to, float f) noexcept
	{
		uint32_t r = (uint32_t)maths::lerp(from.r(), to.r(),f);
		uint32_t g = (uint32_t)maths::lerp(from.g(), to.g(),f);
		uint32_t b = (uint32_t)maths::lerp(from.b(), to.b(),f);
		uint32_t a = (uint32_t)maths::lerp(from.a(), to.a(),f);

		return BLRgba32(r, g, b, a);
	}
}

// A sampler of visible light
// Humanly visible light ranges from a frequency of 
// 380 to 780
// This sampler takes an input parameter from 0 to 1 and
// returns an RGB value that ranges from the low
// to the high
class VisibleLightSampler : public ISampler1D<BLRgba32>
{
public:
	BLRgba32 operator()(float u) override
	{
		auto wl = maths::map(u, 0.0, 1.0, 380.0, 780.0);
		auto c = ndt::ColorRGBAFromWavelength(wl);
		return BLRgba32((uint32_t)(c.r * 255), (uint32_t)(c.g * 255), (uint32_t)(c.b * 255));
	}
};

// Take a gradient, and do the interpolation to get a 
// color for a specific parameter between 0..1
class GradientSampler1D : public ISampler1D<BLRgba32>
{
	BLGradient fGradient;

public:
	GradientSampler1D(const BLGradient& grad)
		: fGradient(grad)
	{}

	BLRgba32 operator()(float u) override
	{
		// ensure we're in the range 0..1 inclusive
		u = maths::clamp(u, 0.0f, 1.0f);

		const BLGradientStop* stops = fGradient.stops();
		size_t lowIndex = ndt::searchClosestLast(stops, fGradient.size(), u);
		size_t highIndex = lowIndex < fGradient.size() - 1 ? lowIndex + 1 : lowIndex;

		//printf("lowIndex: %zd  highIndex: %zd\n", lowIndex, highIndex);
		BLRgba32 from = BLRgba32(stops[lowIndex].rgba);
		BLRgba32 to = BLRgba32(stops[highIndex].rgba);
		double f = maths::map(u, (float)stops[lowIndex].offset, (float)stops[highIndex].offset, 0.0f, 1.0f);

		return ndt::lerpColor(from, to, f);
	}
};

class GradientSampler2D : public ISampler2D<BLRgba32>
{
	BLGradient fGradient1;
	BLGradient fGradient2;

public:
	GradientSampler2D(const BLGradient& grad1, const BLGradient& grad2)
		:fGradient1(grad1),
		fGradient2(grad2)
	{}

	BLRgba32 operator()(float u, float v) override
	{
		return BLRgba32();
	}
};