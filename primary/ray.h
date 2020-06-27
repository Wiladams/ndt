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

#define RAY_EPSILON .0001

class Ray
{
public:
    point3 orig;
    vec3 dir;
    double tm;

public:
    Ray(){}
    Ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction), tm(0)
    {}

    Ray(const point3& origin, const vec3& direction, double time)
        : orig(origin), dir(direction), tm(time)
    {}

    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    double time() const { return tm; }

    point3 at(double t) const {
        return orig + t * dir;
    }
};

/*
class Ray
{
public:
    point3 orig;
    vec3 dir;
    double mint;
    double maxt;
    double tm;
    #define RAY_EPSILON .0001

public:
    Ray()
        :mint(RAY_EPSILON),
        maxt(INFINITY),
        tm(0.0f) 
    {}
    Ray(const point3& origin, const vec3& direction, double time=0.0)
        : orig(origin), 
        dir(direction), 
        tm(time),
        mint((float)RAY_EPSILON),
        maxt((float)INFINITY)
    {}


    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
    double time() const { return tm; }

    point3 at(double t) const {
        return orig + t * dir;
    }
};
*/
