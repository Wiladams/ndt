#include "p5.hpp"
#include "random.hpp"
#include <cstdio>

/*
    Demonstrate random number generation by displaying
    a bunch of pixels at random locations across
    the window.  Both the location as well as
    the color should be pretty uniformly random.
*/

TausPRNG rng(5);

void draw()
{
    background(0x7f);
    noStroke();

    for (int i = 1; i<=1000; i++)
    {
        // draw some random pixels
        int x = random(0,width-1);
        int y = random(0,height-1);
        BLRgba32 c1 = color(random(255), random(255), random(255));
        BLRgba32 c2 = color(random(255), random(255), random(255), 0x7f);
    
        fill(c1);
        circle(x, y, 4);

        //fill(c2);
        //circle(mouseX, mouseY, 32);
    }
}

void setup()
{
    createCanvas(800, 600);
}