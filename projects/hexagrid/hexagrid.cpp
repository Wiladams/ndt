#include "p5.hpp"

using namespace p5;

// https://eperezcosano.github.io/hex-grid/


void make_polygon(BLPath &path, size_t numSides, float radius = 1.0f) 
{
    float cx = 0;
    float cy = 0;
    const auto insideAngle = maths::radians(360.0f / (float)numSides);

    path.clear();
    
    for (size_t i = 0; i < numSides; i++) 
    {
        float x = cx + radius * maths::cos(insideAngle * i);
        float y = cy + radius * maths::sin(insideAngle * i);
        if (i == 0)
            path.moveTo(x, y);
        else
            path.lineTo(x, y);
    }
    path.close();
}


int NUMSIDES = 8;


void drawHex()
{
    const auto insideAngle = maths::radians(360.0f / (float)NUMSIDES);

    background(255);

    push();
    translate(canvasWidth / 2.0f, canvasHeight / 2.0f);
    scale(1, -1);

    // draw axis lines
    stroke(255, 0, 0);
    line(0, canvasHeight/2.0f, 0, -canvasHeight/2.0);
    line(-canvasWidth / 2.0f, 0, canvasWidth / 2.0f, 0);

    stroke(0);

    for (size_t i = 0; i < NUMSIDES; i++)
    {
        push();
        rotate(insideAngle * i);
        translate(120, 0);
        rotate(insideAngle /2.0f);
        line(0, 0, 0, 120);
        circle(0, 0, 8);
        pop();
    }

    pop();
}

void draw()
{
    BLPath ppath{};
    make_polygon(ppath, 6, 120);

    background(200, 210, 226);

    push();
    translate(canvasWidth / 2.0f, canvasHeight / 2.0f);
    scale(1, -1);

    // draw axis lines
    stroke(255, 0, 0);
    line(0, canvasHeight / 2.0f, 0, -canvasHeight / 2.0);
    line(-canvasWidth / 2.0f, 0, canvasWidth / 2.0f, 0);

    // draw polygon
    stroke(0);
    path(ppath);
    pop();
}

void setup()
{
    createCanvas(640, 480);
}