#pragma once

/*
    This file is the encapsulation of a Ray Tracer
*/
#include "rtweekend.h"
#include "box.h"
#include "camera.h"
#include "hittable_list.h"

#include "canvas.h"


class RayTracer {
    GCanvas fCanvas;
    int fCurrentRow;
    int fSamplesPerPixel;
    int fMaxDepth = 50;
    size_t fFrameWidth;
    size_t fFrameHeight;
    Camera fCamera;
    hittable_list fWorld;
    rtcolor fBackground;

public:
    RayTracer()
        :RayTracer(300, 300, 10, 50) {}

    RayTracer(const int w, const int h, const int spp, const int maxDepth)
        :fCanvas(w, h),
        fCurrentRow(- 1),
        fSamplesPerPixel(spp),
        fMaxDepth(maxDepth),
        fFrameWidth(w),
        fFrameHeight(h),
        fWorld(),
        fBackground(0,1,1)
    {
    }

    void setBackground(const rtcolor& c)
    {
        fBackground = c;
    }

    Camera& getCamera() { return fCamera; }
    void setCamera(const Camera& cam) { fCamera = cam; reset(); }

    int getCurrentRow() { return fCurrentRow; }

    int getSamplesPerPixel() const { return fSamplesPerPixel; }
    void setSamplesPerPixel(const int spp)
    {
        fSamplesPerPixel = (int)clamp(spp, 10, 10000);
        printf("setSamplesPerPixel(%d)\n", fSamplesPerPixel);
    }

    void setWorld(hittable_list &world)
    {
        fWorld = world;

        reset();
    }

    void reset()
    {
        fCurrentRow = fCanvas.targetHeight() - 1;
    }

    // Set a single pixel taking into account scaling by the 
    // number of samples per pixel
    void setPixel(int x, int y, const rtcolor& c)
    {
        //printf("setPixel: %3.2f, %3.2f, %3.2f\n", c.r, c.g, c.b);

        auto r = c.r;
        auto g = c.g;
        auto b = c.b;

        // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
        if (r != r) r = 0.0;
        if (g != g) g = 0.0;
        if (b != b) b = 0.0;

        // Divide the rtcolor by the number of samples and gamma-correct for gamma=2.0.
        auto scale = 1.0 / fSamplesPerPixel;
        r = sqrt(scale * r);
        g = sqrt(scale * g);
        b = sqrt(scale * b);

        int ri = static_cast<int>(256 * clamp(r, 0.0, 0.999));
        int gi = static_cast<int>(256 * clamp(g, 0.0, 0.999));
        int bi = static_cast<int>(256 * clamp(b, 0.0, 0.999));

        //printf("setPixel: %d, %d, %d\n", ri, gi, bi);
        auto p = BLRgba32(ri, gi, bi);

        y = fCanvas.targetHeight() - 1 - y;
        fCanvas.set(x, y, p);
    }

    rtcolor ray_color(const Ray& r, const rtcolor& bkgnd, int depth) {
        hit_record rec;

        // If we've exceeded the ray bounce limit, no more light is gathered.
        if (depth <= 0)
            return rtcolor(0, 0, 0);

        // If the ray hits nothing, return the bkgnd rtcolor.
        if (!fWorld.hit(r, 0.001, infinity, rec))
            return bkgnd;

        Ray scattered;
        rtcolor attenuation;
        rtcolor emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return emitted;

        return emitted + (attenuation * ray_color(scattered, bkgnd, depth - 1));
    }

    bool renderRow()
    {
        //printf("renderRow: %d\n", fCurrentRow);

        if (fCurrentRow < 0)
            return false;

        for (int i = 0; i < fFrameWidth; ++i)
        {
            rtcolor pixel_color;
            for (int s = 0; s < fSamplesPerPixel; ++s) {
                auto u = (i + random_double()) / ((double)fFrameWidth - 1);
                auto v = (fCurrentRow + random_double()) / ((double)fFrameHeight - 1);
                Ray r = fCamera.get_ray(u, v);
                pixel_color += ray_color(r, fBackground, fMaxDepth);
            }
            setPixel(i, fCurrentRow, pixel_color);     // Set pixel
        }

        fCurrentRow = fCurrentRow - 1;

        return true;
    }

    BLImage& getImage() { return fCanvas.getImage(); }
};
