#include "apphost.h"

int width;
int height;

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

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"


#include <iostream>


const auto aspect_ratio = 16.0 / 9.0;
const int image_width = 1200;
const int image_height = static_cast<int>(image_width / aspect_ratio);
const int samples_per_pixel = 10;
const int max_depth = 50;
int row;
hittable_list world;
camera *cam;

void createCanvas(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;

    setCanvasSize(aWidth, aHeight);
    gAppWindow->setCanvasSize(aWidth, aHeight);
    gAppWindow->show();
}

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }

    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}


hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

void set(const BLImageData &info, int x, int y, const color& c)
{
    auto r = c.x();
    auto g = c.y();
    auto b = c.z();

    // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    int ri = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    int gi = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    int bi = static_cast<int>(256 * clamp(b, 0.0, 0.999));

    y = info.size.h - 1 - y;
    ((BLRgba32 *)info.pixelData)[(y * info.size.w) + x] = BLRgba32(ri, gi, bi);
}

void draw()
{
    if (row < 0)
        return;
    
    BLImageData info;
    gAppSurface->getBlend2dImage().getData(&info);

    //printf("row: %d\n", row);
    for (int i = 0; i < image_width; ++i) {
        color pixel_color;
        for (int s = 0; s < samples_per_pixel; ++s) {
            auto u = (i + random_double()) / (width - 1);
            auto v = (row + random_double()) / (height - 1);
            ray r = cam->get_ray(u, v);
            pixel_color += ray_color(r, world, max_depth);
        }
        set(info, i, row, pixel_color);     // Set pixel
    }

    row = row - 1;
}

void setup() 
{
    createCanvas(image_width, image_height);

    gAppSurface->flush();
    world = random_scene();

    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    cam = new camera(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    row = image_height - 1;
/*
    // Do the rendering
    for (int j = image_height - 1; j >= 0; --j) {
        printf("row: %d\n", j);
        for (int i = 0; i < image_width; ++i) {
            color pixel_color;
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            set(info, i, j, pixel_color);     // Set pixel
        }
    }
*/

}
