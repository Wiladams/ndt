#include "studio.hpp"
#include "coloring.h"
#include "perlin.h"

#include <functional>


//using namespace maths;

template <typename Shader>
static bool draw_proc_image(PixelAccessor<maths::vec4b> &img, bool linear, Shader&& shader) 
{
    auto scale = 1.0f / maths::max(img.width(), img.height());
    for (auto j : maths::range(img.height())) {
        for (auto i : maths::range(img.width())) {
            auto uv = maths::vec2f{ i * scale, j * scale };
            img.setPixel(i, j, maths::float_to_byte(shader(uv)));
        }
    }
    return true;
}



bool draw_ramp(PixelAccessor<maths::vec4b>& img, float scale, 
    const maths::vec4f& color0, const maths::vec4f& color1)
{
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= scale;
    uv -= maths::vec2f{ (float)(int)uv.x, (float)(int)uv.y };
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
    const maths::vec4f& color0= { 0.2f, 0.2f, 0.2f, 1.0f },
    const maths::vec4f& color1= { 0.5f, 0.5f, 0.5f, 1.0f })
{
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= 4 * scale;
    uv -= maths::vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    auto thick = 0.125f;
    auto center = maths::vec2f{
        uv.x <= 0.5f ? 0.25f : 0.75f,
        uv.y <= 0.5f ? 0.25f : 0.75f,
    };
    auto dist = maths::clamp(length(uv - center), 0.0f, thick) / thick;
    auto val = uv.x <= 0.5f != uv.y <= 0.5f ? (1 + maths::sqrt(1 - dist)) / 2
        : (dist * dist) / 2;
    return lerp(color0, color1, val);
        });
}

bool draw_uvramp(PixelAccessor<maths::vec4b>& img, float scale) 
{
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= scale;
    uv -= maths::vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    return maths::vec4f{ uv.x, uv.y, 0, 1 };
        });
}


bool draw_noisemap(PixelAccessor<maths::vec4b>& img, float scale,
    const maths::vec4f& color0, const maths::vec4f& color1) {
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= 8 * scale;
    auto v = maths::perlin_noise(maths::vec3f{ uv.x, uv.y, 0 });
    v = maths::clamp(v, 0.0f, 1.0f);
    return maths::lerp(color0, color1, v);
        });
}

bool draw_turbulencemap(PixelAccessor<maths::vec4b>& img, float scale, const maths::vec4f& noise, const maths::vec4f& color0, const maths::vec4f& color1)
{
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= 8 * scale;
    auto v = maths::perlin_turbulence({ uv.x, uv.y, 0 }, noise.x, noise.y, (int)noise.z);
    v = maths::clamp(v, 0.0f, 1.0f);
    return lerp(color0, color1, v);
        });
}

bool draw_blackbodyramp(PixelAccessor<maths::vec4b>& img, float scale=1, float from=1000, float to=12000) 
{
    return draw_proc_image(img, true, [=](maths::vec2f uv) {
        uv *= scale;
    uv -= maths::vec2f{ (float)(int)uv.x, (float)(int)uv.y };
    auto rgb = maths::blackbody_to_rgb(maths::lerp(from, to, uv.x));
    return maths::vec4f{ rgb.x, rgb.y, rgb.z, 1 };
        });
}



// Implementation of sunsky modified heavily from pbrt
bool draw_sunsky(PixelAccessor<maths::vec4b>& img, float theta_sun=maths::pif/4, float turbidity=3,
    bool has_sun=true, float sun_intensity=1, float sun_radius=1,
    const maths::vec3f& ground_albedo={0.2f,0.2f,0.2f}) 
{
    auto zenith_xyY = maths::vec3f{
        (+0.00165f * pow(theta_sun, 3.f) - 0.00374f * pow(theta_sun, 2.f) +
            0.00208f * theta_sun + 0.00000f) *
                pow(turbidity, 2.f) +
            (-0.02902f * pow(theta_sun, 3.f) + 0.06377f * pow(theta_sun, 2.f) -
                0.03202f * theta_sun + 0.00394f) *
                turbidity +
            (+0.11693f * pow(theta_sun, 3.f) - 0.21196f * pow(theta_sun, 2.f) +
                0.06052f * theta_sun + 0.25885f),
        (+0.00275f * pow(theta_sun, 3.f) - 0.00610f * pow(theta_sun, 2.f) +
            0.00316f * theta_sun + 0.00000f) *
                pow(turbidity, 2.f) +
            (-0.04214f * pow(theta_sun, 3.f) + 0.08970f * pow(theta_sun, 2.f) -
                0.04153f * theta_sun + 0.00515f) *
                turbidity +
            (+0.15346f * pow(theta_sun, 3.f) - 0.26756f * pow(theta_sun, 2.f) +
                0.06669f * theta_sun + 0.26688f),
        1000 * (4.0453f * turbidity - 4.9710f) *
                tan((4.0f / 9.0f - turbidity / 120.0f) * (maths::pif - 2 * theta_sun)) -
            .2155f * turbidity + 2.4192f,
    };

    auto perez_A_xyY = maths::vec3f{ -0.01925f * turbidity - 0.25922f,
        -0.01669f * turbidity - 0.26078f, +0.17872f * turbidity - 1.46303f };
    auto perez_B_xyY = maths::vec3f{ -0.06651f * turbidity + 0.00081f,
        -0.09495f * turbidity + 0.00921f, -0.35540f * turbidity + 0.42749f };
    auto perez_C_xyY = maths::vec3f{ -0.00041f * turbidity + 0.21247f,
        -0.00792f * turbidity + 0.21023f, -0.02266f * turbidity + 5.32505f };
    auto perez_D_xyY = maths::vec3f{ -0.06409f * turbidity - 0.89887f,
        -0.04405f * turbidity - 1.65369f, +0.12064f * turbidity - 2.57705f };
    auto perez_E_xyY = maths::vec3f{ -0.00325f * turbidity + 0.04517f,
        -0.01092f * turbidity + 0.05291f, -0.06696f * turbidity + 0.37027f };

    auto perez_f = [](maths::vec3f A, maths::vec3f B, maths::vec3f C, maths::vec3f D, maths::vec3f E, float theta,
        float gamma, float theta_sun, maths::vec3f zenith) -> maths::vec3f {
            auto num = ((1 + A * exp(B / cos(theta))) *
                (1 + C * exp(D * gamma) + E * cos(gamma) * cos(gamma)));
            auto den = ((1 + A * exp(B)) * (1 + C * exp(D * theta_sun) +
                E * cos(theta_sun) * cos(theta_sun)));
            return zenith * num / den;
    };

    auto sky = [&perez_f, perez_A_xyY, perez_B_xyY, perez_C_xyY, perez_D_xyY,
        perez_E_xyY, zenith_xyY](
            float theta, float gamma, float theta_sun) -> maths::vec3f {
                return xyz_to_rgb(xyY_to_xyz(
                    perez_f(perez_A_xyY, perez_B_xyY, perez_C_xyY, perez_D_xyY,
                        perez_E_xyY, theta, gamma, theta_sun, zenith_xyY))) /
                    10000;
    };

    // compute sun luminance
    auto sun_ko = maths::vec3f{ 0.48f, 0.75f, 0.14f };
    auto sun_kg = maths::vec3f{ 0.1f, 0.0f, 0.0f };
    auto sun_kwa = maths::vec3f{ 0.02f, 0.0f, 0.0f };
    auto sun_sol = maths::vec3f{ 20000.0f, 27000.0f, 30000.0f };
    auto sun_lambda = maths::vec3f{ 680, 530, 480 };
    auto sun_beta = 0.04608365822050f * turbidity - 0.04586025928522f;
    auto sun_m = 1.0f /
        (cos(theta_sun) + 0.000940f * pow(1.6386f - theta_sun, -1.253f));

    auto tauR = exp(-sun_m * 0.008735f * pow(sun_lambda / 1000, -4.08f));
    auto tauA = exp(-sun_m * sun_beta * pow(sun_lambda / 1000, -1.3f));
    auto tauO = exp(-sun_m * sun_ko * .35f);
    auto tauG = exp(
        -1.41f * sun_kg * sun_m / pow(1 + 118.93f * sun_kg * sun_m, 0.45f));
    auto tauWA = exp(-0.2385f * sun_kwa * 2.0f * sun_m /
        pow(1 + 20.07f * sun_kwa * 2.0f * sun_m, 0.45f));
    auto sun_le = sun_sol * tauR * tauA * tauO * tauG * tauWA * 10000;

    // rescale by user
    sun_le *= sun_intensity;

    // sun scale from Wikipedia scaled by user quantity and rescaled to at
    // the minimum 5 pixel diamater
    auto sun_angular_radius = 9.35e-03f / 2;  // Wikipedia
    sun_angular_radius *= sun_radius;
    sun_angular_radius = maths::max(sun_angular_radius, 2 * maths::pif / img.height());

    // sun direction
    auto sun_direction = maths::vec3f{ 0, cos(theta_sun), sin(theta_sun) };

    auto sun = [has_sun, sun_angular_radius, sun_le](auto theta, auto gamma) {
        return (has_sun && gamma < sun_angular_radius) ? sun_le / 10000
            : maths::vec3f{ 0, 0, 0 };
    };

    // Make the sun sky image
    //auto img = make_image(width, height, true);
    for (auto j = 0; j < img.height() / 2; j++) {
        auto theta = maths::pif * ((j + 0.5f) / img.height());
        theta = maths::clamp(theta, 0.0f, maths::pif / 2 - maths::flt_eps);
        for (int i = 0; i < img.width(); i++) {
            auto phi = 2 * maths::pif * (float(i + 0.5f) / img.width());
            auto w = maths::vec3f{ cos(phi) * sin(theta), cos(theta), sin(phi) * sin(theta) };
            auto gamma = acos(maths::clamp(dot(w, sun_direction), -1.0f, 1.0f));
            auto sky_col = sky(theta, gamma, theta_sun);
            auto sun_col = sun(theta, gamma);
            auto col = sky_col + sun_col;

            img.setPixel(i, j, maths::float_to_byte(maths::vec4f{ col.x, col.y, col.z, 1 }));
            //img.pixels[j * width + i] = { col.x, col.y, col.z, 1 };
        }
    }

    if (ground_albedo != maths::vec3f{ 0, 0, 0 }) {
        auto ground = maths::vec3f{ 0, 0, 0 };
        for (auto j = 0; j < img.height() / 2; j++) {
            auto theta = maths::pif * ((j + 0.5f) / img.height());
            for (int i = 0; i < img.width(); i++) {
                //auto pxl = img.pixels[j * width + i];
                auto pxl = maths::byte_to_float(img.getPixel(i, j));
                auto le = maths::vec3f{ pxl.x, pxl.y, pxl.z };
                auto angle = sin(theta) * 4 * maths::pif / (img.width() * img.height());
                ground += le * (ground_albedo / maths::pif) * cos(theta) * angle;
            }
        }
        for (auto j = img.height() / 2; j < img.height(); j++) {
            for (int i = 0; i < img.width(); i++) {
                img.setPixel(i,j, maths::float_to_byte({ ground.x, ground.y, ground.z, 1 }));
            }
        }
    }
    else {
        for (auto j = img.height() / 2; j < img.height(); j++) {
            for (int i = 0; i < img.width(); i++) {
                img.setPixel(i, j, maths::float_to_byte({ 0, 0, 0, 1 }));
            }
        }
    }

    // done
    return true;
}






struct WIN : public GraphicElement
{
    WIN(const maths::rectf& box)
        :GraphicElement(box)
    {}

    void draw(IGraphics& ctx)
    {
        ctx.noStroke();
        ctx.fill(0, 255, 255);
        ctx.rect(frameX(), frameY(), frameWidth(), frameHeight());
    }
};

void setup()
{

    
    //draw_grid(appFrameBuffer(), 4, { 1,0,0,0.75f }, { 1,1,1,1 });

    //draw_checker(appFrameBuffer(), 4);

    //draw_bumps(appFrameBuffer(),8);

    //draw_ramp(appFrameBuffer(), 2, { 0.0,0.0,1.0,1.0 }, { 0,1.0,0,1.0 });

    //draw_uvramp(appFrameBuffer(), 2);

    //draw_noisemap(appFrameBuffer(), 2, { 0.0,0.0,0.0,1.0 }, { 1,1,1,1 });

    //draw_turbulencemap(appFrameBuffer(), 4, { 1, 0.5, 8,1 }, { 0.0,0.0,0.0,1.0 }, { 1,1,1,1 });
    
    //draw_blackbodyramp(appFrameBuffer(), 1, 100, 12000);

    //draw_sunsky(appFrameBuffer());

    ///*
    auto win1 = std::make_shared<WIN>(maths::rectf{ 20, 20, 320, 240 });
    win1->setDrawingRoutine([win1](IGraphics& ctx) {
        ctx.noStroke();
        ctx.fill(255, 0, 255);
        ctx.rect(win1->frameX(), win1->frameY(), win1->boundsWidth(), win1->boundsHeight());
        });

    auto win2 = std::make_shared<WIN>(maths::rectf{ 360, 100, 320, 240 });
    win2->setDrawingRoutine([win2](IGraphics& ctx) {
        ctx.noStroke();
    ctx.fill(255, 255, 0);
    ctx.rect(win2->frameX(), win2->frameY(), win2->boundsWidth(), win2->boundsHeight());
        });
    
    setDesktopDrawing([](IGraphics& ctx) {
        draw_noisemap(appFrameBuffer(), 2, { 0.0,0.0,0.0,1 }, { 1,1,1,1 });});

    //setDesktopDrawing([](IGraphics& ctx) {
    //    draw_sunsky(appFrameBuffer(), maths::pif/3, 3, true, 2.0, 20);});

    addGraphic(win1);
    addGraphic(win2);
    
    frameRate(2);
}


