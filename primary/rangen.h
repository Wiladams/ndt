#pragma once

//=============================================
// RANDOM NUMBER GENERATION
// ORIGINS
// The random number generation code here comes from yocto
// originally://yocto-gl
// Which in turn borrows from pcg-random.org
// 
// Code has been modified to fit style of ndt
//

namespace maths
{
	using std::vector;
}

//=============================================
// DECLARATION - PCG Random number generation
//=============================================
namespace maths
{
	// PCG random numbers from http://www.pcg-random.org/
	struct rng_state {
		uint64_t state = 0x853c49e6748fea9bULL;
		uint64_t inc = 0xda3e39cb94b95bdbULL;
	};

	inline rng_state make_rng(uint64_t seed, uint64_t seq = 1);

	// Getting the next random number in sequence
	// floats in [0..1)
	//  ints in [0..n)
	inline int rand1i(rng_state& rng, int n);
	inline float rand1f(rng_state& rng);
	inline vec2f rand2f(rng_state& rng);
	inline vec3f rand3f(rng_state& rng);

	// Shuffle a sequence of elements
	template <typename T>
	inline void shuffle(vector<T>& vals, rng_state& rng);
}


//====================================================
// IMPLEMENTATION - Random Number Generation routines
//====================================================

namespace maths
{

	// Next random number, for use within these routines only
	inline uint32_t _advance_rng(rng_state& rng)
	{
		uint64_t oldstate = rng.state;
		rng.state = oldstate * 6364136223846793005ULL + rng.inc;
		auto xorshifted = (uint32_t)(((oldstate >> 18u) ^ oldstate) >> 27u);
		auto rot = (uint32_t)(oldstate >> 59u);
		// return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
		return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
	}

	// Initialize a random number generator with a seed state from the sequence seq.
	inline rng_state make_rng(uint64_t seed, uint64_t seq)
	{
		auto rng = rng_state();
		rng.state = 0U;
		rng.inc = (seq << 1u) | 1u;
		_advance_rng(rng);
		rng.state += seed;
		_advance_rng(rng);
		return rng;
	}

	// Next random numbers: floats in [0,1), ints in [0,n).
	inline int   rand1i(rng_state& rng, int n) { return _advance_rng(rng) % n; }
	inline float rand1f(rng_state& rng) {
		union {
			uint32_t u;
			float    f;
		} x;
		x.u = (_advance_rng(rng) >> 9) | 0x3f800000u;
		return x.f - 1.0f;
		// alternate implementation
		// const static auto scale = (float)(1.0 / numeric_limits<uint32_t>::max());
		// return advance_rng(rng) * scale;
	}
	inline vec2f rand2f(rng_state& rng) {
		// force order of evaluation by using separate assignments.
		auto x = rand1f(rng);
		auto y = rand1f(rng);
		return { x, y };
	}
	inline vec3f rand3f(rng_state& rng) {
		// force order of evaluation by using separate assignments.
		auto x = rand1f(rng);
		auto y = rand1f(rng);
		auto z = rand1f(rng);
		return { x, y, z };
	}

	// Shuffles a sequence of elements
	template <typename T>
	inline void shuffle(vector<T>& vals, rng_state& rng)
	{
		// https://en.wikipedia.org/wiki/Fisher–Yates_shuffle
		for (auto i = (int)vals.size() - 1; i > 0; i--) {
			auto j = rand1i(rng, i + 1);
			std::swap(vals[j], vals[i]);
		}
	}
}
