#include "apphost.h"


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

#include "box.h"
#include "bvh.h"
#include "camera.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"
#include "texture.h"
#include "guistyle.hpp"

#include "p5.hpp"
//#include "MonthTile.hpp"

#include <iostream>



const auto aspect_ratio = 4.0 / 3.0;
const int image_width = 640;    // 1200;
const int image_height = static_cast<int>(image_width / aspect_ratio);

int lowSamples = 10;
int highSamples = 5000;
int samples_per_pixel = lowSamples;
const int max_depth = 50;
rtcolor bkgnd = rtcolor(0.0, 0.80, 1.00);

Surface* hudSurface;
Surface* imageSurface;

int row;
hittable_list world;
camera cam;

//CalendarMonthTile mayTile(2020, 5, 8, 8);

GUIStyle gs;

ImageTexture createImageTexture(const char* filename)
{
    auto img = p5::loadImage(filename);
    BLImageData info;
    img.getData(&info);

    return ImageTexture(info.pixelData, info.size.w, info.size.h, info.stride);
}

rtcolor ray_color(const ray& r, const rtcolor& bkgnd, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0)
        return rtcolor(0, 0, 0);

    // If the ray hits nothing, return the bkgnd rtcolor.
    if (!world.hit(r, 0.001, INFD, rec))
        return bkgnd;

    ray scattered;
    rtcolor attenuation;
    rtcolor emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, bkgnd, world, depth - 1);
}

// F1 - Random Scene
hittable_list random_scene() {
    hittable_list world;

    auto checker = make_shared<checker_texture>(
        make_shared<solid_color>(0.2, 0.3, 0.1),
        make_shared<solid_color>(0.9, 0.9, 0.9)
        );

    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = rtcolor::random() * rtcolor::random();
                    sphere_material = make_shared<lambertian>(make_shared<solid_color>(albedo));
                    auto center2 = center + vec3(0, random_double_range(0, .5), 0);
                    world.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = rtcolor::random(0.5, 1);
                    auto fuzz = random_double_range(0, 0.5);
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

    auto material2 = make_shared<lambertian>(make_shared<solid_color>(rtcolor(0.4, 0.2, 0.1)));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(rtcolor(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return hittable_list(make_shared<bvh_node>(world, 0.0, 1.0));
}

// F2 - Two Spheres
hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(
        make_shared<solid_color>(0.2, 0.3, 0.1),
        make_shared<solid_color>(0.9, 0.9, 0.9)
        );

    objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
    objects.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    return objects;
}

// F3 - Perlin Spheres
hittable_list two_perlin_spheres() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    return objects;
}

// F4 - Earth
hittable_list earth() {
    auto earth_texture = make_shared<ImageTexture>("earthmap2k.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

    return hittable_list(globe);
}

// F5 - Simple Light
hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(make_shared<solid_color>(4, 4, 4));
    //objects.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
    

    
    return objects;
}

// F6 - Cornell Box
hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    //auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto green = make_shared<dielectric>(1.25);
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));
    auto b2dlogo = make_shared<ImageTexture>("blend2d_logo_flipped.png");
    auto b2dlogo_surface = make_shared<lambertian>(b2dlogo);

    
    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));

    //objects.add(make_shared<xz_rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    //objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, b2dlogo_surface)));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    objects.add(box2);

    return objects;
}

// F7 - Cornell Balls
hittable_list cornell_balls() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(5, 5, 5));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    auto boundary = make_shared<sphere>(point3(160, 100, 145), 100, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.1, make_shared<solid_color>(1, 1, 1)));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    return objects;
}

// F8 - Cornell Smoke
hittable_list cornell_smoke() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));

    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130, 0, 65));

    objects.add(make_shared<constant_medium>(box1, 0.01, make_shared<solid_color>(0, 0, 0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, make_shared<solid_color>(1, 1, 1)));

    return objects;
}

// F9 - Cornell Final
hittable_list cornell_final() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(0.1);

    auto mat = make_shared<lambertian>(make_shared<ImageTexture>("earthmap2k.jpg"));

    auto red = make_shared<lambertian>(make_shared<solid_color>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<solid_color>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    shared_ptr<hittable> boundary2 =
        make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), make_shared<dielectric>(1.5));
    boundary2 = make_shared<rotate_y>(boundary2, -18);
    boundary2 = make_shared<translate>(boundary2, vec3(130, 0, 65));

    auto tex = make_shared<solid_color>(0.9, 0.9, 0.9);

    objects.add(boundary2);
    objects.add(make_shared<constant_medium>(boundary2, 0.2, tex));

    return objects;
}

// F10 - Final Scene
hittable_list final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(make_shared<solid_color>(0.48, 0.83, 0.53));

    const int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double_range(1, 101);
            auto z1 = z0 + w;

            boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    hittable_list objects;

    objects.add(make_shared<bvh_node>(boxes1, 0, 1));

    auto light = make_shared<diffuse_light>(make_shared<solid_color>(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material =
        make_shared<lambertian>(make_shared<solid_color>(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(rtcolor(0.8, 0.8, 0.9), 10.0)
        ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(
        boundary, 0.2, make_shared<solid_color>(0.2, 0.4, 0.9)
        ));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, make_shared<solid_color>(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<ImageTexture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(make_shared<solid_color>(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(point3::random(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2, 0.0, 1.0), 15),
        vec3(-100, 270, 395)
        )
    );

    return objects;
}


void keyReleased(const KeyEvent& e)
{
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto vfov = 40.0;
    auto aperture = 0.0;
    auto dist_to_focus = 10.0;
    bkgnd = rtcolor(0, 0, 0);

    switch (e.keyCode) {
    case VK_ADD:
    case VK_OEM_PLUS:
        samples_per_pixel += 100;
        return;
        break;

    case VK_SUBTRACT:
    case VK_OEM_MINUS:
        samples_per_pixel = (samples_per_pixel > 100) ? samples_per_pixel - 100 : samples_per_pixel;
        return;
    case VK_F1:
        world = random_scene();
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        bkgnd = rtcolor(0.70, 0.80, 1.00);
        break;

    case VK_F2:
        world = two_spheres();
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        bkgnd = rtcolor(0.70, 0.80, 1.00);
        break;

    case VK_F3:
        world = two_perlin_spheres();
        lookfrom = point3(13, 2, 3);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        bkgnd = rtcolor(0.70, 0.80, 1.00);
        break;

    case VK_F4:
        world = earth();
        lookfrom = point3(0, 0, 12);
        lookat = point3(0, 0, 0);
        vfov = 20.0;
        bkgnd = rtcolor(0.70, 0.80, 1.00);
        break;

    case VK_F5:
        world = simple_light();
        lookfrom = point3(26, 3, 1);
        lookat = point3(0, 2, 0);
        vfov = 20.0;
        break;

    case VK_F6:
        world = cornell_box();
        row = image_height - 1;
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;

    case VK_F7:
        world = cornell_balls();
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;

    case VK_F8:
        world = cornell_smoke();
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;

    case VK_F9:
        world = cornell_final();
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;

    case VK_F10:
        world = final_scene();
        lookfrom = point3(478, 278, -600);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;

    default:
        return;
    }

    // reset the camera, and reset
    // the row to the top of the image
    cam = camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);
    row = image_height - 1;

}

void keyPressed(const KeyEvent& e)
{
    switch (e.keyCode) {
        case 's':
        case 'S':
            BLImageCodec codec;
            codec.findByName("BMP");
            gAppSurface->getBlend2dImage().writeToFile("ratiow.bmp", codec);
        break;
    }
}

void set(const BLImageData& info, int x, int y, const rtcolor& c)
{
    auto r = c.r;
    auto g = c.g;
    auto b = c.b;

    // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Divide the rtcolor by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    int ri = static_cast<int>(256 * clamp(r, 0.0, 0.999));
    int gi = static_cast<int>(256 * clamp(g, 0.0, 0.999));
    int bi = static_cast<int>(256 * clamp(b, 0.0, 0.999));

    y = info.size.h - 1 - y;
    ((BLRgba32*)info.pixelData)[(y * info.size.w) + x] = BLRgba32(ri, gi, bi);
}

void textRaised(IGraphics *ctx, const char * txt, int x, int y, int w, int h)
{
    ctx->push();
    ctx->translate(x, y);
    gs.drawRaisedRect(ctx, 0, 0, w, h);
    ctx->noStroke();
    ctx->fill(0, 0, 0);
    ctx->text(txt, 4, h-6);
    ctx->pop();
}

void textSunken(IGraphics* ctx, const char* txt, int x, int y, int w, int h)
{
    ctx->push();
    ctx->translate(x, y);
    gs.drawSunkenRect(ctx, 0, 0, w, h);
    ctx->noStroke();
    ctx->fill(0, 0, 0);
    ctx->text(txt, 4, h - 6);
    ctx->pop();
}

void draw()
{   
    // if we've already rendered every row
    // don't bother doing any more rendering
        // Doing a variable number of samples deapending on where
        // we are in the image
    double frac = map(row, image_height - 1, 0, 0, 1);
    //samples_per_pixel = (int)lerp(lowSamples, highSamples, frac);


    imageSurface->loadPixels();
    if (row >= 0) {

        BLImageData info;
        imageSurface->getBlend2dImage().getData(&info);
        
        for (int i = 0; i < image_width; ++i)
        {
            rtcolor pixel_color;
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / ((double)image_width - 1);
                auto v = (row + random_double()) / ((double)image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, bkgnd, world, max_depth);
            }
            set(info, i, row, pixel_color);     // Set pixel
        }

        row = row - 1;
    }
    imageSurface->updatePixels();
    
    hudSurface->push();
    hudSurface->clear();
    hudSurface->noStroke();
    hudSurface->fill(Pixel(220, 220, 220, 180));
    hudSurface->translate(2, (double)p5::height - 62-1);
    hudSurface->rect(0, 0, (double)p5::width - 6, 60, 2, 2);

    hudSurface->translate(2, 2);
    hudSurface->fill(Pixel(0, 0, 0));
    // First row
    textRaised(hudSurface, "F1 - Random", 0, 2, 116, 18);
    textRaised(hudSurface, "F2 - Two Spheres", 120, 2, 116, 18);
    textRaised(hudSurface, "F3 - Perlin Spheres", 240, 2, 116, 18);
    textRaised(hudSurface, "F4 - Earth", 360, 2, 116, 18);
    textRaised(hudSurface, "F5 - Simple Light", 480, 2, 116, 18);

    // Second row
    textSunken(hudSurface, "F6 - Cornell Box", 0, 24, 116, 18);
    textRaised(hudSurface, "F7 - Cornell Balls", 120, 24, 116, 18);
    textRaised(hudSurface, "F8 - Cornell Smoke", 240, 24, 116, 18);
    textRaised(hudSurface, "F9 - Cornell Final", 360, 24, 116, 18);
    textRaised(hudSurface, "F10 - Final Scene", 480, 24, 116, 18);


    // Third Row - commands
    hudSurface->text(" S - Save", 0, 54);
    char buff[32];
    int buffLen = 32;
    sprintf_s(buff, buffLen, "+/-  Samples (%d)", samples_per_pixel);
    hudSurface->text(buff, 120, 54);

    hudSurface->pop();

    hudSurface->flush();

    gAppSurface->blendMode(BL_COMP_OP_SRC_COPY);
    gAppSurface->image(imageSurface->getBlend2dImage(), 0, 0);

    gAppSurface->blendMode(BL_COMP_OP_SRC_OVER);
    gAppSurface->image(hudSurface->getBlend2dImage(), 0, 0);


    //gAppSurface->stroke(Pixel(255, 0, 0));
    //gAppSurface->text("F1 - Random", 0, 14);
    //gAppSurface->line(p5::width / 2, 0, p5::width / 2, p5::height - 1);
    gAppSurface->flush();
}


void setup() 
{
    p5::createCanvas(image_width, image_height);

    imageSurface = new Surface(p5::width, p5::height, 1);
    hudSurface = new Surface(p5::width, p5::height, 1);
    hudSurface->clear();

    row = -1;
}
