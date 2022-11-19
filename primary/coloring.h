#pragma once


#include "maths.hpp"

// DECLARATIONS
// Converting between bytes and floats
//
namespace maths
{
    inline vec4b float_to_byte(const vec4f& a);
    inline vec4f byte_to_float(const vec4b& a);
    inline byte float_to_byte(float a);
    inline float byte_to_float(byte a);

    // Luminance
    inline float luminance(const vec3f& a);
    
    // sRGB non-linear curve
    inline float srgb_to_rgb(float srgb);
    inline float rgb_to_srgb(float rgb);
    inline vec3f srgb_to_rgb(const vec3f& srgb);
    inline vec4f srgb_to_rgb(const vec4f& srgb);
    inline vec3f rgb_to_srgb(const vec3f& rgb);
    inline vec4f rgb_to_srgb(const vec4f& rgb);

    // Conversion between number of channels.
    inline vec4f rgb_to_rgba(const vec3f& rgb);
    inline vec3f rgba_to_rgb(const vec4f& rgba);

    // Composite colors
    inline vec4f composite(const vec4f& a, const vec4f& b);

    // Convert between CIE XYZ and RGB
    inline vec3f rgb_to_xyz(const vec3f& rgb);
    inline vec3f xyz_to_rgb(const vec3f& xyz);

    // Convert between CIE XYZ and xyY
    inline vec3f xyz_to_xyY(const vec3f& xyz);
    inline vec3f xyY_to_xyz(const vec3f& xyY);

    // Converts between HSV and RGB color spaces.
    inline vec3f hsv_to_rgb(const vec3f& hsv);
    inline vec3f rgb_to_hsv(const vec3f& rgb);

    // Approximate color of blackbody radiation from wavelength in nm.
    inline vec3f blackbody_to_rgb(float temperature);

}


//
// IMPLEMENTATION
// Color routines
//
namespace maths
{
    inline vec4b float_to_byte(const vec4f& a) {
        return { (byte)clamp(int(a.x * 256), 0, 255),
            (byte)clamp(int(a.y * 256), 0, 255), (byte)clamp(int(a.z * 256), 0, 255),
            (byte)clamp(int(a.w * 256), 0, 255) };
    }

    inline vec4f byte_to_float(const vec4b& a) {
        return { a.x / 255.0f, a.y / 255.0f, a.z / 255.0f, a.w / 255.0f };
    }
    inline byte float_to_byte(float a) { return (byte)clamp(int(a * 256), 0, 255); }
    inline float byte_to_float(byte a) { return a / 255.0f; }

    // Luminance
    inline float luminance(const vec3f& a) {
        return (0.2126f * a.x + 0.7152f * a.y + 0.0722f * a.z);
    }


    // sRGB non-linear curve
    inline float srgb_to_rgb(float srgb) {
        return (srgb <= 0.04045) ? srgb / 12.92f
            : pow((srgb + 0.055f) / (1.0f + 0.055f), 2.4f);
    }
    inline float rgb_to_srgb(float rgb) {
        return (rgb <= 0.0031308f) ? 12.92f * rgb
            : (1 + 0.055f) * pow(rgb, 1 / 2.4f) - 0.055f;
    }
    inline vec3f srgb_to_rgb(const vec3f& srgb) {
        return { srgb_to_rgb(srgb.x), srgb_to_rgb(srgb.y), srgb_to_rgb(srgb.z) };
    }
    inline vec4f srgb_to_rgb(const vec4f& srgb) {
        return {
            srgb_to_rgb(srgb.x), srgb_to_rgb(srgb.y), srgb_to_rgb(srgb.z), srgb.w };
    }
    inline vec3f rgb_to_srgb(const vec3f& rgb) {
        return { rgb_to_srgb(rgb.x), rgb_to_srgb(rgb.y), rgb_to_srgb(rgb.z) };
    }
    inline vec4f rgb_to_srgb(const vec4f& rgb) {
        return { rgb_to_srgb(rgb.x), rgb_to_srgb(rgb.y), rgb_to_srgb(rgb.z), rgb.w };
    }

    // Conversion between number of channels.
    inline vec4f rgb_to_rgba(const vec3f& rgb) { return { rgb.x, rgb.y, rgb.z, 1 }; }
    inline vec3f rgba_to_rgb(const vec4f& rgba) { return xyz(rgba); }


    // Convert between CIE XYZ and RGB
    inline vec3f rgb_to_xyz(const vec3f& rgb) {
        // https://en.wikipedia.org/wiki/SRGB
        static const auto mat = mat3f{
            {0.4124f, 0.2126f, 0.0193f},
            {0.3576f, 0.7152f, 0.1192f},
            {0.1805f, 0.0722f, 0.9504f},
        };
        return mat * rgb;
    }
    inline vec3f xyz_to_rgb(const vec3f& xyz) {
        // https://en.wikipedia.org/wiki/SRGB
        static const auto mat = mat3f{
            {+3.2406f, -0.9689f, +0.0557f},
            {-1.5372f, +1.8758f, -0.2040f},
            {-0.4986f, +0.0415f, +1.0570f},
        };
        return mat * xyz;
    }

    // Convert between CIE XYZ and xyY
    inline vec3f xyz_to_xyY(const vec3f& xyz) {
        if (xyz == vec3f{ 0, 0, 0 }) return { 0, 0, 0 };
        return {
            xyz.x / (xyz.x + xyz.y + xyz.z), xyz.y / (xyz.x + xyz.y + xyz.z), xyz.y };
    }
    inline vec3f xyY_to_xyz(const vec3f& xyY) {
        if (xyY.y == 0) return { 0, 0, 0 };
        return { xyY.x * xyY.z / xyY.y, xyY.z, (1 - xyY.x - xyY.y) * xyY.z / xyY.y };
    }

    // Convert HSV to RGB
    inline vec3f hsv_to_rgb(const vec3f& hsv) {
        // from Imgui.cpp
        auto h = hsv.x, s = hsv.y, v = hsv.z;
        if (hsv.y == 0) return { v, v, v };

        h = fmod(h, 1.0f) / (60.0f / 360.0f);
        int   i = (int)h;
        float f = h - (float)i;
        float p = v * (1 - s);
        float q = v * (1 - s * f);
        float t = v * (1 - s * (1 - f));

        switch (i) {
        case 0: return { v, t, p };
        case 1: return { q, v, p };
        case 2: return { p, v, t };
        case 3: return { p, q, v };
        case 4: return { t, p, v };
        case 5: return { v, p, q };
        default: return { v, p, q };
        }
    }

    inline vec3f rgb_to_hsv(const vec3f& rgb) {
        // from Imgui.cpp
        auto r = rgb.x, g = rgb.y, b = rgb.z;
        auto K = 0.f;
        if (g < b) {
            std::swap(g, b);
            K = -1;
        }
        if (r < g) {
            std::swap(r, g);
            K = -2 / 6.0f - K;
        }

        auto chroma = r - (g < b ? g : b);
        return { abs(K + (g - b) / (6 * chroma + 1e-20f)), chroma / (r + 1e-20f), r };
    }

    // Approximate color of blackbody radiation from wavelength in nm.
    inline vec3f blackbody_to_rgb(float temperature) {
        // clamp to valid range
        auto t = clamp(temperature, 1667.0f, 25000.0f) / 1000.0f;
        // compute x
        auto x = 0.0f;
        if (temperature < 4000.0f) {
            x = -0.2661239f * 1 / (t * t * t) - 0.2343589f * 1 / (t * t) +
                0.8776956f * (1 / t) + 0.179910f;
        }
        else {
            x = -3.0258469f * 1 / (t * t * t) + 2.1070379f * 1 / (t * t) +
                0.2226347f * (1 / t) + 0.240390f;
        }
        // compute y
        auto y = 0.0f;
        if (temperature < 2222.0f) {
            y = -1.1063814f * (x * x * x) - 1.34811020f * (x * x) + 2.18555832f * x -
                0.20219683f;
        }
        else if (temperature < 4000.0f) {
            y = -0.9549476f * (x * x * x) - 1.37418593f * (x * x) + 2.09137015f * x -
                0.16748867f;
        }
        else {
            y = +3.0817580f * (x * x * x) - 5.87338670f * (x * x) + 3.75112997f * x -
                0.37001483f;
        }
        return xyz_to_rgb(xyY_to_xyz({ x, y, 1 }));
    }


}


namespace maths
{
    // Composite colors
    // alpha blend two colors, returning a pre-multiplied version
    inline vec4f composite(const vec4f& a, const vec4f& b) {
        if (a.w == 0 && b.w == 0) return { 0, 0, 0, 0 };
        auto cc = xyz(a) * a.w + xyz(b) * b.w * (1 - a.w);
        auto ca = a.w + b.w * (1 - a.w);
        return { cc.x / ca, cc.y / ca, cc.z / ca, ca };
    }
}


/*
    References:

    https://www.fourmilab.ch/documents/specrend/
*/

namespace ndt {
    // RGBA_Doubles
    // human visual range is between
    // 380 nm and 780 nm
    //
    // Perhaps this can be done as linear interpolation
    // based on finding the nearest bracket values, using
    // gradient stops
    //
    // 380, 440, r = -1.0f * ((float)wl - 440.0f) / (440.0f - 380.0f); g= 0, b = 1.0;
    // 440, 490, r = 0.0, g = ((float)wl - 440.0f) / (490.0f - 440.0f), b = 1.0
    // 490, 510, r = 0.0, g = 1.0, b = -1.0f * ((float)wl - 510.0f) / (510.0f - 490.0f)
    // 510, 580, r = (float)((wl - 510.0) / (580.0 - 510.0)), g = 1.0, b = 0.0
    // 580, 645, r = 1.0,  g = (float)(-1.0 * (wl - 645.0) / (645.0 - 580.0)), b = 0.0
    // 645, 780, r = 1.0, g = 0.0, b = 0.0
    maths::vec4f  ColorRGBAFromWavelength(double wl, double gamma = 1.0)
    {
        maths::vec4f t = { 0,0,0,1 };

        if (wl >= 380.0 && wl <= 440.0) {
            t.r = -1.0f * ((float)wl - 440.0f) / (440.0f - 380.0f);
            t.b = 1.0;
        }
        else if (wl >= 440.0 && wl <= 490.0) {
            t.g = ((float)wl - 440.0f) / (490.0f - 440.0f);
            t.b = 1.0;
        }
        else if (wl >= 490.0 && wl <= 510.0) {
            t.g = 1.0;
            t.b = -1.0f * ((float)wl - 510.0f) / (510.0f - 490.0f);
        }
        else if ((wl >= 510.0) && (wl <= 580.0)) {
            t.r = (float)((wl - 510.0) / (580.0 - 510.0));
            t.g = 1.0;
        }
        else if (wl >= 580.0 && wl <= 645.0) {
            t.r = 1.0;
            t.g = (float)(-1.0 * (wl - 645.0) / (645.0 - 580.0));
        }
        else if (wl >= 645.0 && wl <= 780.0) {
            t.r = 1.0;
        }

        auto s = 1.0;
        if (wl > 700.0) {
            s = 0.3 + 0.7 * (780.0 - wl) / (780.0 - 700.0);
        }
        else if (wl < 420.0) {
            s = 0.3 + 0.7 * (wl - 380.0) / (420.0 - 380.0);
        }

        t.r = (float)pow(t.r * s, gamma);
        t.g = (float)pow(t.g * s, gamma);
        t.b = (float)pow(t.b * s, gamma);

        return t;
    }



}
