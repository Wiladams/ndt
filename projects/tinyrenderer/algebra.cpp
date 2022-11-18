#include "algebra.hpp"

vec3f cross(const vec3f &v1, const vec3f &v2) {
    return vec<3>{v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
}

