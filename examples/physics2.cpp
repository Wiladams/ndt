#include "p5.hpp"

const int nShapes = 200;
float shapeCount;
float birthRate = 0.25;
float gGravity = 0.10;
int gDeathRate = 1;

// control width of sparay when shapes are born
float sprayWidth = 5;


struct shape {
    float w;
    float h;
    float x;
    float y;
    float xSpeed;
    float ySpeed;
    float gravity;
    float damping;
    float friction;

    int lives;

    PixRGBA c;

    shape() 
    {
        lives = 9;
        x = 0;
        y = 0;
        w = random(2,17);
        h = w;
        xSpeed = random(-sprayWidth, sprayWidth);
        gravity = gGravity;
        damping = random(0.7, 0.98);
        friction = random(0.65, 0.95);
        c = color(255);
    }

    void reset()
    {
        lives = lives - gDeathRate;
        if (lives > 0) {
            x = width/2;
            y = 0;
            xSpeed = random(-sprayWidth, sprayWidth);
            c = color(c.red-10);
        }
    }

    void update()
    {
        x += xSpeed;
        ySpeed += gravity;
        y += ySpeed;

        // collision detection
        if (y >= height-h) {
            y = height-h;

            // bounce
            ySpeed *= -1.0;
            // slow down vertical motion whenever
            // we hit the ground
            ySpeed *= damping;
            xSpeed *= friction;
        }

        if (x >= width-w) {
            x = width-w;
            xSpeed *= -1;
        }
        if (x<=0) {
            x = 0;
            xSpeed *= -1;
        }

        // If we've stopped moving, begin life anew
        if ((fabs(xSpeed) <= 0.1) && (fabs(ySpeed) <= 0.1)) {
            reset();
        }
    }

    void draw()
    {
        stroke(0);
        //fill(255, 100);
        fill(c);
        rect(x, y, w, h);
    }
};


shape shapes[nShapes];

void setup()
{
    createCanvas(400, 400);
    background(255);

    noStroke();

    for (int i=0; i < nShapes; i++) {
        shapes[i].x = width/2;
    }

    frameRate(60);
}

void draw() {
    // fade background
    fill(255, 100);
    rect(0,0,width, height);
    fill(0);

    // shapeCount births rects over time
    for (int i=0; i<shapeCount; i++) {
        shapes[i].draw();
        shapes[i].update();
    }
    if (shapeCount < nShapes) {
        shapeCount += birthRate;
    }
}