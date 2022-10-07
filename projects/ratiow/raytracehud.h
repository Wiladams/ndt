#pragma once

// Heads Up display for the ray tracing application
// This displays the interface that allows you to render
// different scenes
//

#include "guistyle.hpp"
#include "Surface.h"
#include "raytracer.h"

class RaytraceHUD {
    GUIStyle fStyler;
    Surface fSurface;
    shared_ptr<RayTracer> fTracer;

    void textRaised(const char* txt, int x, int y, int w, int h)
    {
        fSurface.push();
        fSurface.translate(x, y);
        fStyler.drawRaisedRect(&fSurface, 0, 0, w, h);
        fSurface.noStroke();
        ((IGraphics*)&fSurface)->fill(0, 0, 0);
        fSurface.text(txt, 4, (double)h - 6);
        fSurface.pop();
    }

    void textSunken(const char* txt, int x, int y, int w, int h)
    {
        fSurface.push();
        fSurface.translate(x, y);
        fStyler.drawSunkenRect(&fSurface, 0, 0, w, h);
        fSurface.noStroke();
        ((IGraphics*)&fSurface)->fill(0, 0, 0);
        fSurface.text(txt, 4, (double)h - 6);
        fSurface.pop();
    }

public:
    RaytraceHUD(const int w, const int h, shared_ptr<RayTracer> tracer)
        : fSurface(w, h, 1),
        fTracer(tracer)
    {
    }

    void draw()
    {
        fSurface.push();
        fSurface.clear();
        fSurface.noStroke();
        fSurface.fill(Pixel(220, 220, 220, 180));
        fSurface.translate(2, (double)canvasHeight - 62 - 1);
        fSurface.rect(0, 0, (double)canvasWidth - 6, 60, 2, 2);

        fSurface.translate(2, 2);
        fSurface.fill(Pixel(0, 0, 0));

        // First row of buttons
        textRaised("F1 - Random", 0, 2, 116, 18);
        textRaised("F2 - Two Spheres", 120, 2, 116, 18);
        textRaised("F3 - Perlin Spheres", 240, 2, 116, 18);
        textRaised("F4 - Earth", 360, 2, 116, 18);
        textRaised("F5 - Simple Light", 480, 2, 116, 18);

        // Second row of buttons
        textRaised("F6 - Cornell Box", 0, 24, 116, 18);
        textRaised("F7 - Cornell Balls", 120, 24, 116, 18);
        textRaised("F8 - Cornell Smoke", 240, 24, 116, 18);
        textRaised("F9 - Cornell Final", 360, 24, 116, 18);
        textRaised("F10 - Final Scene", 480, 24, 116, 18);


        // Third Row of buttons - commands
        fSurface.text(" S - Save", 0, 54);
        char buff[32];
        int buffLen = 32;
        sprintf_s(buff, buffLen, "+/-  Samples (%d)", fTracer->getSamplesPerPixel());
        fSurface.text(buff, 120, 54);

        fSurface.pop();

        fSurface.flush();
    }

    BLImage& getImage() { return fSurface.getImage(); }
};
