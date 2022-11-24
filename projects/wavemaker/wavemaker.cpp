#include "p5.hpp"


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
    for (int x = 0; x <= canvasWidth - 1; x += 30) {
        for (int y = 0; y <= canvasHeight; y += 30) {
            //starting point of each circle depends on mouse position
            double xAngle = map(mouseX, 0, canvasWidth-1, (-4.0 * maths::pi), (4.0 * maths::pi), true);
            double yAngle = map(mouseY, 0, canvasHeight-1, (-4.0 * maths::pi), (4.0 * maths::pi), true);
            
            //printf("xAngle: %f  yAngle: %f\n", xAngle, yAngle);

            //and also varies based on the particle's location
            double angle = (xAngle * ((double)x / (double)canvasWidth)) + (yAngle * ((double)y / (double)canvasHeight));

            //each particle moves in a circle
            double myX = ((double)x + 20.0 * cos((2.0 * maths::pi * t) + angle));
            double myY = ((double)y + 20.0 * sin((2.0 * maths::pi * t) + angle));

            circle(myX, myY, gRadius); //draw particle
        }
    }

    t = t + 0.01; //update time

    //stats.draw();
}

void keyReleased(const KeyboardEvent& event)
{
    if (keyCode == VK_ESCAPE) {
        halt();
    }
}

void setup()
{
    frameRate(30);
    createCanvas(600, 600,"wavemaker",4);
    
    // Do the following if you want full screen
    //createCanvas(displayWidth, displayHeight);    
    //layered();
    //setCanvasPosition(0, 0);

    noStroke();
    fill(40, 200, 40);
    
    frameRate(30);
}