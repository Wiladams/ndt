#include "p5.hpp"
//#include "geometry.h"

using namespace p5;



void draw()
{
    scale(1, -1);
    translate(0, -height - 1);

    triangle(10,10, 200,200, 100,200);

    fill(255, 0, 0);
    circle(width/2, height/2, 400);

    noLoop();
}


void setup()
{
    createCanvas(800,600);

    background(0x80);
    fill(0,0,255);
    stroke(255,255,255);
}
