#include "p5.hpp"

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