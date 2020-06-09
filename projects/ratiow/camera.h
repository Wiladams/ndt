#pragma once

/*
    Camera

    Determines the physics of the view.  The camera, and
    it's lense, allow you to set things like a focal length,
    aspect ratio, and other properties
*/

//#include "rtweekend.h"
#include "grmath.h"
#include "ray.h"

class Camera {
private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 u, v, w;           // basis vectors
    double lens_radius;
    double time0, time1;    // shutter open/close times

public:
    Camera() 
        : Camera(point3(0, 0, -1), point3(0, 0, 0), vec3(0, 1, 0), 40, 1, 0, 10) {}

    Camera(
        point3 lookfrom,    // Where are we located in space
        point3 lookat,      // What point in space are we looking at
        vec3   vup,         // Which direction is 'up'

        // Optical Attributes of the camera
        double vfov,        // vertical field-of-view in degrees
        double aspect_ratio,
        double aperture,
        double focus_dist,

        // Properties for motion blur filming
        double t0 = 0,
        double t1 = 0
    ) {
        auto theta = Radians(vfov);
        auto h = Tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        // calculate the basis vectors for camera
        // the basis vectors define the 3D coordinate space
        // for the camera
        w = (lookfrom - lookat).unit();
        u = cross(vup, w).unit();
        v = cross(w, u);

        origin = lookfrom;
        horizontal = focus_dist * viewport_width * u;
        vertical = focus_dist * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;

        lens_radius = aperture / 2;
        time0 = t0;
        time1 = t1;
    }

    Ray get_ray(double s, double t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x + v * rd.y;

        return Ray(
            origin + offset,
            lower_left_corner + s * horizontal + t * vertical - origin - offset,
            random_double_range(time0, time1));
    }


};
