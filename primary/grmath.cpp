#include "grmath.h"

vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double_range(-1, 1), random_double_range(-1, 1), 0);
        if (p.lengthSquared() >= 1) continue;
        return p;
    }
}

vec3 random_unit_vector() {
    auto a = random_double_range(0, 2 * PI);
    auto z = random_double_range(-1, 1);
    auto r = sqrt(1 - z * z);
    return vec3(r * cos(a), r * sin(a), z);
}

vec3 random_in_unit_sphere() {
    while (true) {
        //auto p = vec3::random(-1, 1);
        auto p = random_vec3_range(-1, 1);
        if (p.lengthSquared() >= 1) continue;
        return p;
    }
}

vec3 random_in_hemisphere(const vec3& normal) {
    vec3 in_unit_sphere = random_in_unit_sphere();
    if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}


