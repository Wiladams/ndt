#pragma once

#include "blend2d.h"
#include "graphic.hpp"

namespace ndt {
	//static inline double BEZ03(double u) { return ((1 - u) * (1 - u) * (1 - u)); }            // (1-u)^3
	//static inline double BEZ13(double u) { return 3 * u * ((1 - u) * (1 - u)); }    // 3u(1-u)^2
	//static inline double BEZ23(double u) { return 3 * u * u * (1 - u); }          // 3u^2(1-u)
	//static inline double BEZ33(double u) { return u * u * u; }                  // u^3

	// Value of curve at parametric position 'u'
	// control points are P0, P1, P2, P3
	// This function calculates a single component (x, y, or whatever)
	// use another function to combine
	double bezier_cubic(const double u, double p1, double p2, double p3, double p4)
	{
		double oneminusu = 1 - u;
		double BEZ03 = oneminusu* oneminusu* oneminusu;				// (1-u)^3
		double BEZ13 = 3 * u * (oneminusu * oneminusu);	// 3u(1-u)^2
		double BEZ23 = 3 * u * u * oneminusu;				// 3u^2(1-u)
		double BEZ33 = u * u * u;							// u^3

		return BEZ03 * p1 + BEZ13 * p2 + BEZ23 * p3 + BEZ33 * p4;
	}

	BLPoint bezier_point(double u, const BLPoint &p1, const BLPoint &p2, const BLPoint &p3, const BLPoint &p4)
	{
		double x = bezier_cubic(u, p1.x, p2.x, p3.x, p4.x);
		double y = bezier_cubic(u, p1.y, p2.y, p3.y, p4.y);

		return { x,y };

	}


}



