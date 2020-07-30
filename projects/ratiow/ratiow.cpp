#include "apphost.h"


#include <iostream>
#include "blend2d.h"
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
#include "raytracer.h"
#include "raytracehud.h"

#include "p5.hpp"


// Determine the size of the image
const auto aspect_ratio = 4.0 / 3.0;
const int image_width = 800;    // 1200;
//const int image_width = 320;    // 1200;
const int image_height = static_cast<int>(image_width / aspect_ratio);


shared_ptr<RayTracer> tracer = make_shared<RayTracer>(image_width, image_height, 10, 50);
RaytraceHUD HUD(image_width, image_height, tracer);


// F1 - Random Scene
hittable_list random_scene() {
    hittable_list scene;

    auto checker = make_shared<checker_texture>(
        make_shared<SolidColorTexture>(0.2, 0.3, 0.1),
        make_shared<SolidColorTexture>(0.9, 0.9, 0.9)
        );

    scene.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    //auto albedo = rtcolor::random() * rtcolor::random();
                    auto albedo = random_vec3()* random_vec3();
                    sphere_material = make_shared<lambertian>(make_shared<SolidColorTexture>(albedo));
                    auto center2 = center + vec3(0, random_double_range(0, .5), 0);
                    scene.add(make_shared<moving_sphere>(
                        center, center2, 0.0, 1.0, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    //auto albedo = rtcolor::random(0.5, 1);
                    auto albedo = random_vec3_range(0.5, 1);
                    auto fuzz = random_double_range(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    scene.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    scene.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    scene.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(make_shared<SolidColorTexture>(rtcolor(0.4, 0.2, 0.1)));
    scene.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(rtcolor(0.7, 0.6, 0.5), 0.0);
    scene.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return hittable_list(make_shared<bvh_node>(scene, 0.0, 1.0));
}

// F2 - Two Spheres
hittable_list two_spheres() {
    hittable_list objects;

    auto checker = make_shared<checker_texture>(
        make_shared<SolidColorTexture>(0.2, 0.3, 0.1),
        make_shared<SolidColorTexture>(0.9, 0.9, 0.9)
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
    hittable_list objects;

    auto earth_texture = make_shared<ImageTexture>("earthmap2k.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);
    objects.add(globe);

    return objects;
}

// F5 - Simple Light
hittable_list simple_light() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(4);
    objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
    objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

    auto difflight = make_shared<diffuse_light>(make_shared<SolidColorTexture>(4, 4, 4));
    //objects.add(make_shared<sphere>(point3(0, 7, 0), 2, difflight));
    objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, difflight));
    

    return objects;
}

// F6 - Cornell Box
hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<SolidColorTexture>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<SolidColorTexture>(.73, .73, .73));
    //auto green = make_shared<lambertian>(make_shared<SolidColorTexture>(.12, .45, .15));
    auto green = make_shared<dielectric>(1.25);
    auto light = make_shared<diffuse_light>(make_shared<SolidColorTexture>(7, 7, 7));
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

    auto red = make_shared<lambertian>(make_shared<SolidColorTexture>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<SolidColorTexture>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<SolidColorTexture>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<SolidColorTexture>(5, 5, 5));

    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, white)));

    auto boundary = make_shared<sphere>(point3(160, 100, 145), 100, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.1, make_shared<SolidColorTexture>(1, 1, 1)));

    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    return objects;
}

// F8 - Cornell Smoke
hittable_list cornell_smoke() {
    hittable_list objects;

    auto red = make_shared<lambertian>(make_shared<SolidColorTexture>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<SolidColorTexture>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<SolidColorTexture>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<SolidColorTexture>(7, 7, 7));

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

    objects.add(make_shared<constant_medium>(box1, 0.01, make_shared<SolidColorTexture>(0, 0, 0)));
    objects.add(make_shared<constant_medium>(box2, 0.01, make_shared<SolidColorTexture>(1, 1, 1)));

    return objects;
}

// F9 - Cornell Final
hittable_list cornell_final() {
    hittable_list objects;

    auto pertext = make_shared<noise_texture>(0.1);

    auto mat = make_shared<lambertian>(make_shared<ImageTexture>("earthmap2k.jpg"));

    auto red = make_shared<lambertian>(make_shared<SolidColorTexture>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<SolidColorTexture>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<SolidColorTexture>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<SolidColorTexture>(7, 7, 7));

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

    auto tex = make_shared<SolidColorTexture>(0.9, 0.9, 0.9);

    objects.add(boundary2);
    objects.add(make_shared<constant_medium>(boundary2, 0.2, tex));

    return objects;
}

// F10 - Final Scene
hittable_list final_scene() {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(make_shared<SolidColorTexture>(0.48, 0.83, 0.53));

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

    auto light = make_shared<diffuse_light>(make_shared<SolidColorTexture>(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    auto center1 = point3(400, 400, 200);
    auto center2 = center1 + vec3(30, 0, 0);
    auto moving_sphere_material =
        make_shared<lambertian>(make_shared<SolidColorTexture>(0.7, 0.3, 0.1));
    objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    objects.add(make_shared<sphere>(
        point3(0, 150, 145), 50, make_shared<metal>(rtcolor(0.8, 0.8, 0.9), 10.0)
        ));

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(
        boundary, 0.2, make_shared<SolidColorTexture>(0.2, 0.4, 0.9)
        ));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, make_shared<SolidColorTexture>(1, 1, 1)));

    auto emat = make_shared<lambertian>(make_shared<ImageTexture>("earthmap.jpg"));
    objects.add(make_shared<sphere>(point3(400, 200, 400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.1);
    objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(pertext)));

    hittable_list spheres;
    auto white = make_shared<lambertian>(make_shared<SolidColorTexture>(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        spheres.add(make_shared<sphere>(random_vec3_range(0, 165), 10, white));
    }

    objects.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(spheres, 0.0, 1.0), 15),
        vec3(-100, 270, 395)
        )
    );

    return objects;
}

// F11 - Blend2D and mirrors
hittable_list blend_mirrors() {
    hittable_list objects;

    auto b2dlogo = make_shared<ImageTexture>("blend2d_logo_flipped.png");
    auto b2dlogo_surface = make_shared<lambertian>(b2dlogo);

    //auto mirror = make_shared<dielectric>(1.25);
    auto mirror = make_shared<dielectric>(2.00);    // zinc sulfide
    auto red = make_shared<lambertian>(make_shared<SolidColorTexture>(.65, .05, .05));
    auto white = make_shared<lambertian>(make_shared<SolidColorTexture>(.73, .73, .73));
    auto green = make_shared<lambertian>(make_shared<SolidColorTexture>(.12, .45, .15));
    auto light = make_shared<diffuse_light>(make_shared<SolidColorTexture>(7, 7, 7));




    // ceiling
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(0, 555, 0, 555, 555, white)));
    // light on the ceiling
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    // left wall
    objects.add(make_shared<flip_face>(make_shared<yz_rect>(0, 555, 0, 555, 555, green)));

    // right wall
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));

    // mirror tile floor
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    for (int z = 0; z < 4; z++) {
        for (int x = 0; x < 4; x++) {
            //objects.add(make_shared<xz_rect>(x*140, (x*140)+130, z*140, (z*140)+130, -1, mirror));
        
            shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(120, 10, 120), white);
            box1 = make_shared<translate>(box1, vec3(x * 140, 15, z*140));
            objects.add(box1);
        }
    }
    

    // Back wall
    objects.add(make_shared<flip_face>(make_shared<xy_rect>(0, 555, 0, 555, 555, b2dlogo_surface)));


    // Mirrored sphere
    objects.add(make_shared<sphere>(point3(273, 273, 273), 200, mirror));

    return objects;
}


void keyReleased(const KeyboardEvent& e)
{
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto vfov = 40.0;
    auto aperture = 0.0;
    auto dist_to_focus = 10.0;
    rtcolor bkgnd(0, 0, 0);
    hittable_list world;

    switch (e.keyCode) {
    case VK_ADD:
    case VK_OEM_PLUS:
        tracer->setSamplesPerPixel(tracer->getSamplesPerPixel() + 100);
        return;
        break;

    case VK_SUBTRACT:
    case VK_OEM_MINUS:
        tracer->setSamplesPerPixel(tracer->getSamplesPerPixel() - 100);
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

    case VK_F11:
        world = blend_mirrors();
        lookfrom = point3(478, 278, -600);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;

    default:
        return;
    }

    // reset the camera, and reset
    // the row to the top of the image
    tracer->setBackground(bkgnd);
    tracer->setWorld(world);
    tracer->setCamera(Camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0));
}

void keyPressed(const KeyboardEvent& e)
{
    switch (e.keyCode) {
        case 's':
        case 'S':
            BLImageCodec codec;
            codec.findByName("BMP");
            gAppSurface->getBlend2dImage().writeToFile("ratiow.bmp", codec);
            tracer->getImage().writeToFile("image.bmp", codec);
        break;
    }
}


void setup() 
{
    p5::createCanvas(image_width, image_height);
    HUD.draw();
}


void draw()
{
    tracer->renderRow();

    // Draw the raytraced image
    p5::blendMode(BL_COMP_OP_SRC_COPY);
    p5::image(tracer->getImage(), 0, 0);
    
    // Draw a progress bar
    p5::stroke(255, 0, 0);
    p5::line(0, (double)p5::height - 1.0 -tracer->getCurrentRow(), p5::width - 1.0, p5::height - 1.0-tracer->getCurrentRow());

    // Composite the HUD on top
    p5::blendMode(BL_COMP_OP_SRC_OVER);
    HUD.draw();
    p5::image(HUD.getImage(), 0, 0);

    //p5::flush();
}
