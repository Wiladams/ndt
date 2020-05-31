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

#include "perlin.h"
#include "blend2d.h"

#include <iostream>


class Texture {
public:
    virtual rtcolor value(double u, double v, const vec3& p) const = 0;
};


class solid_color : public Texture {
public:
    solid_color() {}
    solid_color(rtcolor c) : color_value(c) {}

    solid_color(double red, double green, double blue)
        : solid_color(rtcolor(red, green, blue)) {}

    virtual rtcolor value(double u, double v, const vec3& p) const {
        return color_value;
    }

private:
    rtcolor color_value;
};


class checker_texture : public Texture {
public:
    checker_texture() {}
    checker_texture(shared_ptr<Texture> t0, shared_ptr<Texture> t1) : even(t0), odd(t1) {}

    virtual rtcolor value(double u, double v, const vec3& p) const {
        auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

public:
    shared_ptr<Texture> odd;
    shared_ptr<Texture> even;
};


class noise_texture : public Texture {
public:
    noise_texture() {}
    noise_texture(double sc) : scale(sc) {}

    virtual rtcolor value(double u, double v, const vec3& p) const {
        // return rtcolor(1,1,1)*0.5*(1 + noise.turb(scale * p));
        // return rtcolor(1,1,1)*noise.turb(scale * p);
        return rtcolor(1, 1, 1) * 0.5 * (1 + sin(scale * p.z + 10 * noise.turb(p)));
    }

public:
    perlin noise;
    double scale;
};


class ImageTexture : public Texture {
private:
    BLImage fImage;
    uint8_t * pixelData;
    int fWidth, fHeight;
    intptr_t fStride;

public:
    const static int bytes_per_pixel = 4;

    ImageTexture()
        : pixelData(nullptr), 
        fWidth(0), 
        fHeight(0), 
        fStride(0) {}
    
    ImageTexture(void* data, int width, int height, intptr_t stride)
        :pixelData((uint8_t *)data),
        fStride(stride),
        fWidth(width),
        fHeight(height)
    {
    }

    ImageTexture(BLImage& img)
    {
        fImage = img;
        BLImageData info;
        fImage.getData(&info);
        pixelData = (uint8_t*)info.pixelData;
        fStride = info.stride;
        fWidth = info.size.w;
        fHeight = info.size.h;

    }

    ImageTexture(const char* filename) {

        auto err = fImage.readFromFile(filename);

        if (err) {
            printf("could not load pot of gold (%d)\n", err);
            return;
        }

        BLImageData info;
        fImage.getData(&info);
        pixelData = (uint8_t*)info.pixelData;
        fStride = info.stride;
        fWidth = info.size.w;
        fHeight = info.size.h;
    }

    ~ImageTexture() {
        //delete pixelData;
    }

    virtual rtcolor value(double u, double v, const vec3& p) const 
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (nullptr == pixelData)
            return rtcolor(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * fWidth);
        auto j = static_cast<int>(v * fHeight);

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= fWidth)  i = fWidth - 1;
        if (j >= fHeight) j = fHeight - 1;

        const auto rtcolor_scale = 1.0 / 255.0;
        auto pixel = pixelData + j * fStride + i * bytes_per_pixel;

        // We need to turn pixel values [0..255] into
        // rtcolor components [0..1]
        return rtcolor(rtcolor_scale * pixel[2], rtcolor_scale * pixel[1], rtcolor_scale * pixel[0]);
    }


};



