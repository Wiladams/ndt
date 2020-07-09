#pragma once

#include "grmath.h"
#include "ray.h"
#include "hittable.h"


class sphere : public hittable {
    point3 center;
    double radius;
    shared_ptr<material> mat_ptr;

    // Given a point on a sphere
// figure out the u,v for texture mapping
    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        auto phi = atan2(p.z, p.x);
        auto theta = asin(p.y);
        u = 1 - (phi + maths::Pi) / (maths::Pi2);
        v = (theta + PiOver2) / maths::Pi;
    }

public:
    sphere() :radius(0) {}
    sphere(point3 cen, double r, shared_ptr<material> m)
        : center(cen), radius(r), mat_ptr(m) {};

    // Hit testing
    // Determine where on the sphere a ray hits
    // return false if the ray misses the sphere
    virtual bool hit(const Ray& r, double t_min, double t_max, hit_record& rec) const {
        vec3 oc = r.origin() - center;
        auto a = r.direction().lengthSquared();
        auto half_b = dot(oc, r.direction());
        auto c = oc.lengthSquared() - radius * radius;

        auto discriminant = half_b * half_b - a * c;

        if (discriminant > 0) {
            auto root = sqrt(discriminant);

            auto temp = (-half_b - root) / a;
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.at(rec.t);
                vec3 outward_normal = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_normal);
                get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
                rec.mat_ptr = mat_ptr;
                return true;
            }

            temp = (-half_b + root) / a;
            if (temp < t_max && temp > t_min) {
                rec.t = temp;
                rec.p = r.at(rec.t);
                vec3 outward_normal = (rec.p - center) / radius;
                rec.set_face_normal(r, outward_normal);
                get_sphere_uv((rec.p - center) / radius, rec.u, rec.v);
                rec.mat_ptr = mat_ptr;
                return true;
            }
        }

        return false;
    }

    virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
        output_box = aabb(
            center - vec3(radius, radius, radius),
            center + vec3(radius, radius, radius));
        return true;
    }
};






