#pragma once

//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "grmath.h"


class aabb {
public:
    point3 _min;
    point3 _max;

public:
    aabb() {}
    aabb(const point3& a, const point3& b) 
    { 
        _min = a; 
        _max = b; 
    }

    point3 Min() const { return _min; }
    point3 Max() const { return _max; }

    // Determine whether a ray hits anywhere in the box
    bool hit(const Ray& r, double tmin, double tmax) const {
        for (int a = 0; a < 3; a++) {
            auto t0 = fmin((_min[a] - r.origin()[a]) / r.direction()[a],
                (_max[a] - r.origin()[a]) / r.direction()[a]);
            auto t1 = fmax((_min[a] - r.origin()[a]) / r.direction()[a],
                (_max[a] - r.origin()[a]) / r.direction()[a]);
            tmin = fmax(t0, tmin);
            tmax = fmin(t1, tmax);
            if (tmax <= tmin)
                return false;
        }
        return true;
    }

    // Area of the surface of the box
    double area() const {
        auto a = _max.x - _min.x;
        auto b = _max.y - _min.y;
        auto c = _max.z - _min.z;
        return 2 * (a * b + b * c + c * a);
    }

    //
    // 0 - x-axis
    // 1 - y-axis
    // 2 - z-axis
    int longestAxis() const {
        auto a = _max.x - _min.x;
        auto b = _max.y - _min.y;
        auto c = _max.z - _min.z;
        if (a > b && a > c)
            return 0;
        else if (b > c)
            return 1;
        else
            return 2;
    }


};

// Create an AABB that is the union of the two specified boxes
aabb SurroundingBox(aabb box0, aabb box1) {
    vec3 small(fmin(box0.Min().x, box1.Min().x),
        fmin(box0.Min().y, box1.Min().y),
        fmin(box0.Min().z, box1.Min().z));

    vec3 big(fmax(box0.Max().x, box1.Max().x),
        fmax(box0.Max().y, box1.Max().y),
        fmax(box0.Max().z, box1.Max().z));

    return aabb(small, big);
}

