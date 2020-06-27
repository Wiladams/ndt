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

#include "rtweekend.h"

#include "hittable.h"
#include "texture.h"


double schlick(double cosine, double ref_idx) {
    double r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}


class material {
public:
    virtual rtcolor emitted(double u, double v, const point3& p) const {
        return rtcolor(0, 0, 0);
    }

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, rtcolor& attenuation, Ray& scattered
    ) const = 0;
};


class dielectric : public material {
public:
    double ref_idx;

public:
    dielectric(double ri) : ref_idx(ri) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, rtcolor& attenuation, Ray& scattered
    ) const {
        attenuation = rtcolor(1.0, 1.0, 1.0);
        double etai_over_etat = (rec.front_face) ? (1.0 / ref_idx) : (ref_idx);

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);
        if (etai_over_etat * sin_theta > 1.0) {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = Ray(rec.p, reflected, r_in.time());
            return true;
        }

        double reflect_prob = schlick(cos_theta, etai_over_etat);
        if (random_double() < reflect_prob)
        {
            vec3 reflected = reflect(unit_direction, rec.normal);
            scattered = Ray(rec.p, reflected, r_in.time());
            return true;
        }

        vec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);
        scattered = Ray(rec.p, refracted, r_in.time());
        return true;
    }


};


class diffuse_light : public material {
public:
    diffuse_light(shared_ptr<Texture> a) : emit(a) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, rtcolor& attenuation, Ray& scattered
    ) const {
        return false;
    }

    virtual rtcolor emitted(double u, double v, const point3& p) const {
        return emit->value(u, v, p);
    }

public:
    shared_ptr<Texture> emit;
};


class isotropic : public material {
public:
    isotropic(shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, rtcolor& attenuation, Ray& scattered
    ) const {
        scattered = Ray(rec.p, random_in_unit_sphere(), r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<Texture> albedo;
};


class lambertian : public material {
public:
    lambertian(shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, rtcolor& attenuation, Ray& scattered
    ) const {
        vec3 scatter_direction = rec.normal + random_unit_vector();
        scattered = Ray(rec.p, scatter_direction, r_in.time());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

public:
    shared_ptr<Texture> albedo;
};


class metal : public material {
public:
    metal(const rtcolor& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(
        const Ray& r_in, const hit_record& rec, rtcolor& attenuation, Ray& scattered
    ) const {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }

public:
    rtcolor albedo;
    double fuzz;
};


