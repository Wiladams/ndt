#include "p5.hpp"

#include <vector>
#include <array>

using namespace p5;

constexpr int numBalls = 133;
float spring = 0.03f;        //0.05
float gravity = 2.03f;       // 0.03
float friction = -0.9f;      // -0.9

Pixel  randomColor(uint32_t alpha=255)
{
    uint8_t r = (uint8_t)random(30, 255);
    uint8_t g = (uint8_t)random(30, 255);
    uint8_t b = (uint8_t)random(30, 255);
    uint8_t a = alpha;

    return { r,g,b, a };
}

struct Ball {
    maths::vec2f fCenter{};      // position
    float fRadius = 0;       // serves as 'mass'

    maths::vec2f vxy{};     // velocity
    
    Pixel fColor;

    
    
    Ball() = default;

    // Construct a ball with northing more
    // than center and radius
    Ball(const maths::vec2f &posin, float radius)
    {

        fCenter = posin;

        fRadius = radius;

        vxy = { 0,0 };  // initial velocity vector

        fColor = randomColor(220);  // Some color
    }
    
    void move()
    {
        vxy.y += gravity;
        fCenter += vxy;

        // calculate new x position
        if ((fCenter.x + fRadius) > canvasWidth) {
            fCenter.x = canvasWidth - fRadius;
            vxy.x *= friction;  // bounced off wall, slow down
        }
        else if (fCenter.x - fRadius < 0) {
            fCenter.x = fRadius;
            vxy.x *= friction;
        }

        // Calculate new y position
        if ((fCenter.y + fRadius) > canvasHeight) {
            fCenter.y = canvasHeight - fRadius;
            vxy.y *= friction;
        }
        else if ((fCenter.y - fRadius) < 0) {
            fCenter.y = fRadius;
            vxy.y *= friction;
        }

    }

    void display() {
        noStroke();

        fill(fColor);
        ellipseMode(ELLIPSEMODE::RADIUS);
        ellipse(fCenter.x, fCenter.y, fRadius, fRadius);
        //rect(xy.x, xy.y, radius, radius);

        // draw a center
        //fill(0,127);
        //rect(fCenter.x, fCenter.y, 2, 2);
    }
};

struct Ball* balls[numBalls];

void collision()
{
    //printf("COLISSION\n");

    for (int id = 0; id < numBalls-1; id++) 
    {
        for (int i = id + 1; i < numBalls; i++)
        {
            //printf("ID: %d  i: %d\n", id, i);

            // console.log(others[i]);
            auto dxy = balls[id]->fCenter - balls[i]->fCenter;
            auto dist = maths::distance(balls[id]->fCenter, balls[i]->fCenter);

            // Minimal distance between balls before they are 'touching'
            auto minDist = (balls[id]->fRadius) + (balls[i]->fRadius);
            //printf("distance: %f (%f)\n",dist, minDist);

            if (dist < minDist) {
                //printf("distance: %d-%d;  %f (%f)\n",id, i, dist, minDist);
                stroke(255,0,0);
                strokeWeight(2);
                line(balls[id]->fCenter.x, balls[id]->fCenter.y, balls[i]->fCenter.x, balls[i]->fCenter.y);

                //console.log("2");
                //auto ang = maths::angle(balls[id]->fCenter, balls[i]->fCenter);
                auto ang = maths::atan2(dxy.y, dxy.x);
                auto targetX = balls[id]->fCenter.x + maths::cos(ang) * minDist;
                auto targetY = balls[id]->fCenter.y + maths::sin(ang) * minDist;
                maths::vec2f axy{ (targetX - balls[id]->fCenter.x) * spring , (targetY - balls[id]->fCenter.y) * spring };

                balls[i]->vxy -= axy;
                balls[id]->vxy += axy;
            }
        }
    }

}

void setup() 
{
    createCanvas(1920, 400);
    //layered();
    //fullscreen();

        //frameRate(10);

    for (int i = 0; i < numBalls; i++) 
    {
        balls[i] = new Ball({ random((float)canvasWidth),random((float)canvasHeight) },
            displayWidth/(numBalls+1)/2.0f);        // random(30, 70)
    }
    noStroke();
    fill(255, 204);
}

void draw() 
{
    if (isLayered)
        clear();
    else
        background(92,92,92);
    
    collision();

    for (int i = 0; i < numBalls; i++)
    {
        balls[i]->move();
        balls[i]->display();
    }
}


