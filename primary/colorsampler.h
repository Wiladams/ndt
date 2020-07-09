#pragma once

#include "blend2d.h"
#include "sampler.hpp"
#include "maths.hpp"

namespace ndt {
	// Given an array of gradient stops, return the highest index that is
// lower than the specified offset
	static inline size_t searchClosestLast(const BLGradientStop* array, size_t size, const double& value) noexcept
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

	static BLRgba32 lerpColor(const BLRgba32& from, const BLRgba32& to, double f) noexcept
	{
		uint32_t r = (uint32_t)maths::Lerp(f, from.r, to.r);
		uint32_t g = (uint32_t)maths::Lerp(f, from.g, to.g);
		uint32_t b = (uint32_t)maths::Lerp(f, from.b, to.b);
		uint32_t a = (uint32_t)maths::Lerp(f, from.a, to.a);

		return BLRgba32(r, g, b, a);
	}
}

// Take a gradient, and do the interpolation to get a 
// color for a specific parameter between 0..1
class GradientSampler1D : public ISampler1D<BLRgba32>
{
	BLGradient fGradient;

public:
	GradientSampler1D(const BLGradient& grad)
		: fGradient(grad)
	{}

	BLRgba32 operator()(double u) const
	{
		// ensure we're in the range 0..1 inclusive
		u = maths::Clamp(u, 0, 1.0);

		const BLGradientStop* stops = fGradient.stops();
		size_t lowIndex = ndt::searchClosestLast(stops, fGradient.size(), u);
		size_t highIndex = lowIndex < fGradient.size() - 1 ? lowIndex + 1 : lowIndex;

		//printf("lowIndex: %zd  highIndex: %zd\n", lowIndex, highIndex);
		BLRgba32 from = BLRgba32(stops[lowIndex].rgba);
		BLRgba32 to = BLRgba32(stops[highIndex].rgba);
		double f = maths::Map(u, stops[lowIndex].offset, stops[highIndex].offset, 0, 1);

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

	BLRgba32 operator()(double u, double v)
	{
		return BLRgba32();
	}
};