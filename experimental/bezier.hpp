#pragma once

#include "maths.hpp"

namespace ndt {

	// Value of curve at parametric position 'u'
	// control points are P0, P1, P2, P3
	// This function calculates a single component (x, y, or whatever)
	// use another function to combine
	float bezier_cubic(const float u, float p1, float p2, float p3, float p4)
	{
		float oneminusu = 1.0 - u;
		float BEZ03 = oneminusu* oneminusu* oneminusu;				// (1-u)^3
		float BEZ13 = 3 * u * (oneminusu * oneminusu);	// 3u(1-u)^2
		float BEZ23 = 3 * u * u * oneminusu;				// 3u^2(1-u)
		float BEZ33 = u * u * u;							// u^3

		return BEZ03 * p1 + BEZ13 * p2 + BEZ23 * p3 + BEZ33 * p4;
	}

	maths::vec2f bezier_point(double u, const maths::vec2f &p1, const maths::vec2f &p2, const maths::vec2f &p3, const maths::vec2f &p4)
	{
		float x = bezier_cubic(u, p1.x, p2.x, p3.x, p4.x);
		float y = bezier_cubic(u, p1.y, p2.y, p3.y, p4.y);

		return { x,y };
	}


}



