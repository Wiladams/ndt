#include "p5.hpp"
#include <cstdio>

using namespace p5;


//local stats = require("P5status")()

double t = 0; // time variable
double gRadius = 10;

void draw()
{

    background(10, 10); // translucent background(creates trails)

    //printf("%d , %d\n", mouseX, mouseY);

    // make a x and y grid of ellipses
    fill(40, 200, 40);
    for (int x = 0; x <= width - 1; x += 30) {
        for (int y = 0; y <= height; y += 30) {
            //starting point of each circle depends on mouse position
            double xAngle = map(mouseX, 0, width-1, (-4.0 * PI), (4.0 * PI), true);
            double yAngle = map(mouseY, 0, height-1, (-4.0 * PI), (4.0 * PI), true);
            
            //printf("xAngle: %f  yAngle: %f\n", xAngle, yAngle);

            //and also varies based on the particle's location
            double angle = (xAngle * ((double)x / (double)width)) + (yAngle * ((double)y / (double)height));

            //each particle moves in a circle
            double myX = ((double)x + 20.0 * cos((2.0 * PI * t) + angle));
            double myY = ((double)y + 20.0 * sin((2.0 * PI * t) + angle));

            //printf("myX: %f  myY: %f\n", myX, myY);

            circle(myX, myY, gRadius); //draw particle
        }
    }

    t = t + 0.01; //update time

    //stats.draw();
}

void keyReleased(const KeyEvent& event)
{
    if (keyCode == VK_ESCAPE) {
        halt();
    }
}

void setup()
{
    createCanvas(600, 600);
    
    // Do the following if you want full screen
    //createCanvas(displayWidth, displayHeight);    
    //layered();
    //setWindowPosition(0, 0);

    noStroke();
    fill(40, 200, 40);
    
    frameRate(30);
}