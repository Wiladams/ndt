#pragma once

#include "perlin.h"
#include "blend2d.h"
#include "canvas.h"

#include <iostream>

using std::shared_ptr;
using std::make_shared;


class Texture {
public:
    virtual vec3 value(double u, double v, const vec3& p) const = 0;
};


class SolidColorTexture : public Texture {
private:
    vec3 fColor; 

public:
    SolidColorTexture() {}
    SolidColorTexture(vec3 c) : fColor(c) {}

    SolidColorTexture(double red, double green, double blue)
        : SolidColorTexture(vec3(red, green, blue)) {}

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

    ImageTexture(std::shared_ptr<GCanvas> canvas)
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

        const auto color_scale = 1.0 / 255.0;
        auto pixel = pixelData + j * fStride + i * bytes_per_pixel;

        // We need to turn pixel values [0..255] into
        // rtcolor components [0..1]
        return rtcolor(color_scale * pixel[2], color_scale * pixel[1], color_scale * pixel[0]);
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
        fWidth = canvas->targetWidth();
        fHeight = canvas->targetHeight();
    }


    virtual rtcolor value(double u, double v, const vec3& p) const
    {
        // If we have no texture data, then return solid cyan as a debugging aid.
        if (nullptr == fCanvas)
            return rtcolor(0, 1, 1);

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = clamp(u, 0.0, 1.0);
        v = 1.0 - clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * fWidth);
        auto j = static_cast<int>(v * fCanvas->targetHeight());

        // Clamp integer mapping, since actual coordinates should be less than 1.0
        if (i >= fWidth)  i = fWidth - 1;
        if (j >= fHeight) j = fHeight - 1;

        constexpr auto color_scale = 1.0f / 255;
        auto pixel = fCanvas->get(i, j);

        // We need to turn pixel values [0..255] into
        // rtcolor components [0..1]
        return rtcolor(color_scale * pixel.r, color_scale * pixel.g, color_scale * pixel.b);
    }


};



