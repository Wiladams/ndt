#include "p5.hpp"
#include "coloring.h"

#include <functional>

using namespace p5;
using namespace maths;




template <typename Shader>
static bool draw_proc_image(PixelAccessor<vec4b> &img, bool linear, Shader&& shader) 
{
    auto scale = 1.0f / maths::max(img.width(), img.height());
    for (auto j : range(img.height())) {
        for (auto i : range(img.width())) {
            auto uv = vec2f{ i * scale, j * scale };
            img.setPixel(i, j, float_to_byte(shader(uv)));
        }
    }
    return true;
}

bool draw_ramp(PixelAccessor<maths::vec4b>& img, float scale, 
    const vec4f& color0, const vec4f& color1) 
{
    return draw_proc_image(img, true, [=](vec2f uv) {
        uv *= scale;
    uv -= vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    return lerp(color0, color1, uv.x);
        });
}

// draw grid image
bool draw_grid(PixelAccessor<maths::vec4b>& img, float scale, const maths::vec4f& color0, const maths::vec4f& color1)
{
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= 4 * scale;
    uv -= maths::vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    auto thick = 0.01f / 2;
    auto c = uv.x <= thick || uv.x >= 1 - thick || uv.y <= thick ||
        uv.y >= 1 - thick ||
        (uv.x >= 0.5f - thick && uv.x <= 0.5f + thick) ||
        (uv.y >= 0.5f - thick && uv.y <= 0.5f + thick);
    return c ? color0 : color1; });
}

bool draw_uvramp(PixelAccessor<maths::vec4b>& img, float scale) 
{
    return draw_proc_image(img, true, [=](vec2f uv) {
        uv *= scale;
    uv -= vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    return vec4f{ uv.x, uv.y, 0, 1 };
        });
}

// Make an image of multiple lights.
bool draw_lights(PixelAccessor<maths::vec4b>& img, const vec3f& le, int nlights,
    float langle, float lwidth, float lheight) {
    //auto img = make_image(width, height, true);
    for (auto j = 0; j < img.height() / 2; j++) {
        auto theta = pif * ((j + 0.5f) / img.height());
        theta = clamp(theta, 0.0f, pif / 2 - 0.00001f);
        if (fabs(theta - langle) > lheight / 2) continue;
        for (int i = 0; i < img.width(); i++) {
            auto phi = 2 * pif * (float(i + 0.5f) / img.width());
            auto inlight = false;
            for (auto l : range(nlights)) {
                auto lphi = 2 * pif * (l + 0.5f) / nlights;
                inlight = inlight || fabs(phi - lphi) < lwidth / 2;
            }
            //img.pixels[j * width + i] = { le.x, le.y, le.z, 1 };
            img.setPixel(i, j, float_to_byte({ le.x, le.y, le.z, 1 }));
        }
    }
    return true;
}


void setup()
{
	createCanvas(640,480, "imaging");
    //layered();

    draw_grid(gAppFrameBuffer, 4, { 1,0,0,0.75f }, { 1,1,1,1 });

    //draw_ramp(gAppFrameBuffer, 2, { 0.0,0.0,1.0,1.0 }, { 0,1.0,0,1.0 });

    //draw_uvramp(gAppFrameBuffer, 2);

    //draw_lights(gAppFrameBuffer, {0,1,1}, 1, maths::radians(30), 0.025, 0.025);
}


//draw_grid(gAppFrameBuffer, 8, { 1.0,0,0,1.0 }, { 0,1.0,0,1.0 });

