#include "p5.hpp"
#include "geometry.h"

using namespace p5;

void preload()
{
    Vec3 v1 = { 10,20,30 };
    Vec3 v2 = .5 * v1;
    Vec3 v3 = v1 * 2;

}

void draw()
{
    triangle(10,10, 200,200, 100,200);
    
    noLoop();
}

void setup()
{
    createCanvas(800,600);

    background(0x80);
    fill(0,0,255);
    stroke(255,255,255);
}