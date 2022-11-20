#include "p5.hpp"
#include "coloring.h"
#include "perlin.h"

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
    return draw_proc_image(img, true, [=](vec2f uv) {
        uv *= 4 * scale;
    uv -= maths::vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    auto thick = 0.01f / 2;
    auto c = uv.x <= thick || uv.x >= 1 - thick || uv.y <= thick ||
        uv.y >= 1 - thick ||
        (uv.x >= 0.5f - thick && uv.x <= 0.5f + thick) ||
        (uv.y >= 0.5f - thick && uv.y <= 0.5f + thick);
    return c ? color0 : color1; });
}

bool draw_checker(PixelAccessor<maths::vec4b>& img, float scale = 1,
    const maths::vec4f& color0 = { 0.2f, 0.2f, 0.2f, 1.0f },
    const maths::vec4f& color1 = { 0.5f, 0.5f, 0.5f, 1.0f })
{
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= 4 * scale;
    uv -= maths::vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    auto c = uv.x <= 0.5f != uv.y <= 0.5f;
    return c ? color0 : color1;
        });
}


bool draw_bumps(PixelAccessor<maths::vec4b>& img, float scale=1, 
    const vec4f& color0= { 0.2f, 0.2f, 0.2f, 1.0f },
    const vec4f& color1= { 0.5f, 0.5f, 0.5f, 1.0f }) 
{
    return draw_proc_image(img, true, [=](vec2f uv) {
        uv *= 4 * scale;
    uv -= vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    auto thick = 0.125f;
    auto center = vec2f{
        uv.x <= 0.5f ? 0.25f : 0.75f,
        uv.y <= 0.5f ? 0.25f : 0.75f,
    };
    auto dist = clamp(length(uv - center), 0.0f, thick) / thick;
    auto val = uv.x <= 0.5f != uv.y <= 0.5f ? (1 + maths::sqrt(1 - dist)) / 2
        : (dist * dist) / 2;
    return lerp(color0, color1, val);
        });
}

bool draw_uvramp(PixelAccessor<maths::vec4b>& img, float scale) 
{
    return draw_proc_image(img, true, [=](vec2f uv) {
        uv *= scale;
    uv -= vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    return vec4f{ uv.x, uv.y, 0, 1 };
        });
}



bool draw_turbulencemap(PixelAccessor<maths::vec4b>& img, float scale, const vec4f& noise, const vec4f& color0, const vec4f& color1) 
{
    return draw_proc_image(img, true, [=](vec2f uv) {
        uv *= 8 * scale;
    auto v = perlin_turbulence({ uv.x, uv.y, 0 }, noise.x, noise.y, (int)noise.z);
    v = clamp(v, 0.0f, 1.0f);
    return lerp(color0, color1, v);
        });
}

bool draw_blackbodyramp(PixelAccessor<maths::vec4b>& img, float scale=1, float from=1000, float to=12000) 
{
    return draw_proc_image(img, true, [=](vec2f uv) {
        uv *= scale;
    uv -= vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    auto rgb = blackbody_to_rgb(maths::lerp(from, to, uv.x));
    return vec4f{ rgb.x, rgb.y, rgb.z, 1 };
        });
}


void setup()
{
	createCanvas(640,640, "imaging");
    //layered();

    //draw_grid(gAppFrameBuffer, 4, { 1,0,0,0.75f }, { 1,1,1,1 });

    draw_checker(gAppFrameBuffer, 4);

    //draw_bumps(gAppFrameBuffer,8);

    //draw_ramp(gAppFrameBuffer, 2, { 0.0,0.0,1.0,1.0 }, { 0,1.0,0,1.0 });

    //draw_uvramp(gAppFrameBuffer, 2);

    //draw_turbulencemap(gAppFrameBuffer, 4, { 1, 0.5, 8,1 }, { 0.0,0.0,0.0,1.0 }, { 1,1,1,1 });
    
    //draw_blackbodyramp(gAppFrameBuffer, 1, 100, 12000);
}


