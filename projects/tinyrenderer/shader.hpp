#pragma once


#include "maths.hpp"
#include "pixelaccessor.h"

#include "tgaimage.hpp"
#include "algebra.hpp"

struct IShader {
    // Get a pixel from an image based on uv parametric value
    //static maths::vec4b sample2D(const PixelAccessor<maths::vec4b>& img, vec2f& uvf)
    static maths::vec4b sample2D(const TGAImage& img, vec2f& uvf) 
    {
        return img.getPixel(uvf[0] * img.width(), uvf[1] * img.height());
    }

    // This is the workhorse of the shader
    // Get the color of a fragment
    virtual bool fragment(const vec3f bar, maths::vec4b& color) = 0;
};
