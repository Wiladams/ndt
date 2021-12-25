#pragma once

#include <functional>
#include <vector>


// A templatized parametric function that takes a single double
// value, sets a parametric value.
// return true if the function succeeds, false otherwise
// The value of 'u' typically ranges from 0 to 1, but it
// can be anything
template <typename T>
using PFunc1 = std::function< T (const double u) >;

using PFuncDouble = PFunc1<double>;

/*
* A generalized structure for holding linear gradients.
	The general idea is to add 'stops' at various intervals
	Then, when you ask for a piece of data between offsets,
	it will do the interpolation to return that value.
*/
template <typename T>
struct InterpolatorStop
{
	double offset;
	T data;

	LinearStop() noexcept = default
	LinearStop(const LinearStop<T> &other) noexcept = default

	LinearStop(const double off, const T& value) noexcept
		:offset(off),
		data(value) {}

	inline bool equals(const InterpolatorStop<T>& other)
	{
		return (offset == other.offset) && (data == other.data);
	}

	inline bool operator==(const InterpolatorStop<T>& other) const noexcept { return  equals(other); }
	inline bool operator!=(const InterpolatorStop<T>& other) const noexcept { return !equals(other); }
};




template <typename T>
struct Interpolator
{
	std::vector<T> stops;
	
	inline size_t searchClosestLast(const double& value) noexcept
	{
		size_t size = stops.size();
		if (stops.size() < 1)
			return 0;

		const T* base = stops;
		while (size_t half = size / 2u) {
			const T* middle = base + half;
			size -= half;
			if (middle[0].offset <= value)
				base = middle;
		}

		return size_t(base - stops);
	}

public:
	inline bool addStop(const double offset, const T& value) {

	}
};