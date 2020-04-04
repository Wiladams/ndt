#include "p5.hpp"

static const int rectSize = 20;

void draw()
{
    noStroke();

    // draw a bunch of solid rectangles
    // as fast as possible.
    for (int i=1; i<=50000; i++){
        fill(color(random(255), random(255), random(255)));
        rect(random(0,width-1-rectSize), random(0,height-1-rectSize), rectSize, rectSize);
    }
}

void setup()
{
    createCanvas(800,600);
    frameRate(15);
}