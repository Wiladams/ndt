#pragma once

#include "perlin.h"
#include "blend2d.h"
#include "canvas.h"
#include "math3d.h"

#include <iostream>
#include <algorithm>

using std::shared_ptr;
using std::make_shared;


class Texture {
public:
    virtual rtcolor value(const double u, const double v, const vec3& p) const = 0;
    virtual BLRgba32 pixelValue(double u, double v, const vec3& p) const
    {
        auto c = value(u, v, p);
        return { (uint32_t)(c.r * 255),(uint32_t)(c.g * 255) ,(uint32_t)(c.b * 255)};
    }
};


class SolidColorTexture : public Texture {
private:
    rtcolor fColor;

public:
    SolidColorTexture(){}
    SolidColorTexture(rtcolor c) : fColor(c) {}
    SolidColorTexture(double red, double green, double blue)
        : fColor(rtcolor(red, green, blue)) {}


    virtual rtcolor value(double u, double v, const vec3& p) const {
        return fColor;
    }
};


class checker_texture : public Texture {
public:
    checker_texture() {}
    checker_texture(shared_ptr<Texture> t0, shared_ptr<Texture> t1) : even(t0), odd(t1) {}

    virtual rtcolor value(double u, double v, const vec3& p) const {
        //printf("%3.2f, %3.2f\n", u,v);
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
    perlin noise;
    double scale; 

public:
    noise_texture() :scale(0) {}
    noise_texture(double sc) : scale(sc) {}

    virtual rtcolor value(double u, double v, const vec3& p) const {
        // return rtcolor(1,1,1)*0.5*(1 + noise.turb(scale * p));
        // return rtcolor(1,1,1)*noise.turb(scale * p);
        return rtcolor(1, 1, 1) * 0.5 * (1 + sin(scale * p.z + 10 * noise.turb(p)));
    }
};


class ImageTexture : public Texture {
private:
    BLImage     fImage;
    BLImageData fData;


public:
    const static int bytes_per_pixel = 4;

    ImageTexture() {}

    ImageTexture(const char* filename) 
    {
        auto err = fImage.readFromFile(filename);

        if (err) {
            printf("could not load image (%d)\n", err);
            return;
        }

        fImage.getData(&fData);
    }

    ~ImageTexture() {}

    virtual rtcolor value(double u, double v, const vec3& p) const 
    {
        static const double color_scale = 1.0 / 255.0;

        // If we have no texture data, then return solid cyan as a debugging aid.
        if (nullptr == fData.pixelData)
            return rtcolor(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        int i = static_cast<int>(maths::Map(Clamp(u,0.0,1.0), 0, 1, 0, fData.size.w - 1));
        int j = static_cast<int>(maths::Map(Clamp(v,0.0,1.0), 0, 1, fData.size.h - 1, 0));

        uint8_t * pix = (uint8_t *)fData.pixelData + j * fData.stride + i * bytes_per_pixel;

        // We need to turn pixel values [0..255] into
        // rtcolor components [0..1]
        rtcolor c(color_scale * pix[2], color_scale * pix[1], color_scale * pix[0]);

        return c;
    }


};


class CanvasTexture : public Texture {
private:
    std::shared_ptr<GCanvas> fCanvas;
    int fWidth;
    int fHeight;

public:

    CanvasTexture()
        : fCanvas(nullptr),
        fWidth(0),
        fHeight(0)
    {}


    CanvasTexture(std::shared_ptr<GCanvas> canvas)
        : fCanvas(canvas)
    {
        fWidth = (int)canvas->targetWidth();
        fHeight = (int)canvas->targetHeight();
    }


    virtual rtcolor value(double u, double v, const vec3& p) const
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (nullptr == fCanvas)
            return rtcolor(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = math3d::Clamp(u, 0.0, 0.999);
        v = 0.999 - math3d::Clamp(v, 0.0, 0.999);  // Flip V to image coordinates

        auto i = static_cast<int>(u * fWidth);
        auto j = static_cast<int>(v * fCanvas->targetHeight());

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= fWidth)  i = fWidth - 1;
        if (j >= fHeight) j = fHeight - 1;

        constexpr auto color_scale = 1.0f / 255;
        auto pixel = fCanvas->get(i, j);

        // We need to turn pixel values [0..255] into
        // rtcolor components [0..1]
        return rtcolor(color_scale * pixel.r(), color_scale * pixel.g(), color_scale * pixel.b());
    }


};



