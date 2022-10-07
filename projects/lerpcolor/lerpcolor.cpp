/*
    This is a good stress test for polygon drawing.
    In particular, it's interesting to watch memory
    consumption, as well as CPU usage.

    Up_Arrorw - Increase the number of iterations per frame by 1,000
    Down_Arrow - Decrease number of iterations per frame by 1,000
    ESC - Exit
*/
#include "p5.hpp"
#include "framestats.h"

using namespace p5;

int ITERATIONS = 1000;
FrameStats _stats;

void draw()
{
    //background(255);
    clear();
    //printf("lerpcolor.draw()\n");

    BLRgba32 from = color(255, 0, 0, (int)(0.2 * 255));
    BLRgba32 to = color(0, 0, 255, (int)(0.2 * 255));
    BLRgba32 c1 = lerpColor(from, to, 0.33f);
    BLRgba32 c2 = lerpColor(from, to, 0.66f);

    noStroke();
    fill(from);


    for (int i = 1; i <= ITERATIONS; i++) {
        fill(from);
        
        //rect(10, 10, 200, 200);

        quad(
            random(-40, 220), random(canvasHeight),
            random(-40, 220), random(canvasHeight),
            random(-40, 220), random(canvasHeight),
            random(-40, 220), random(canvasHeight)
        );


        fill(c1);
        quad(
            random(140, 380), random(canvasHeight),
            random(140, 380), random(canvasHeight),
            random(140, 380), random(canvasHeight),
            random(140, 380), random(canvasHeight)
        );

        fill(c2);
        quad(
            random(320, 580), random(canvasHeight),
            random(320, 580), random(canvasHeight),
            random(320, 580), random(canvasHeight),
            random(320, 580), random(canvasHeight)
        );

        fill(to);
        quad(
            random(500, 760), random(canvasHeight),
            random(500, 760), random(canvasHeight),
            random(500, 760), random(canvasHeight),
            random(500, 760), random(canvasHeight)
        );

        // the command queue can get too big and 
        // eat up lots of memory.  A flush will clear
        // the commands, but slow things down a lot
        gAppSurface->flush();
    }

    _stats.draw(*gAppSurface);
}

void keyReleased(const KeyboardEvent& event)
{
    // Up arrow, add more
    // Down arrow, remove one
    //print("keyReleased: ", event, keyCode)
    if (keyCode == VK_UP) {
        ITERATIONS += 1000;

    }
    else if (keyCode == VK_DOWN) {
        ITERATIONS -= 1000;
        if (ITERATIONS < 1)
            ITERATIONS = 1000;
    }
    else if (keyCode == VK_ESCAPE) {
        halt();
    }

    printf("ITERATIONS: %d\n", ITERATIONS);
}

void setup()
{
    background(255);
    noStroke();

    createCanvas(720, 400);
    //fullscreen();

    frameRate(5);
}
  