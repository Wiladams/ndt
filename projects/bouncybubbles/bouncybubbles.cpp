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
    maths::vec2f xy{};      // position
    float fRadius = 0;       // serves as 'mass'

    maths::vec2f vxy{};     // velocity
    
    Pixel fColor;

    
    
    Ball() = default;

    // Construct a ball with northing more
    // than center and radius
    Ball(const maths::vec2f &posin, float radius)
    {

        xy = posin;

        fRadius = radius;

        vxy = { 0,0 };  // initial velocity vector

        fColor = randomColor(220);  // Some color
    }
    
    void move()
    {
        vxy.y += gravity;
        xy += vxy;

        // calculate new x position
        if ((xy.x + fRadius) > canvasWidth) {
            xy.x = canvasWidth - fRadius;
            vxy.x *= friction;  // bounced off wall, slow down
        }
        else if (xy.x - fRadius < 0) {
            xy.x = fRadius;
            vxy.x *= friction;
        }

        // Calculate new y position
        if ((xy.y + fRadius) > canvasHeight) {
            xy.y = canvasHeight - fRadius;
            vxy.y *= friction;
        }
        else if ((xy.y - fRadius) < 0) {
            xy.y = fRadius;
            vxy.y *= friction;
        }

    }

    void display() {
        noStroke();

        fill(fColor);
        ellipse(xy.x, xy.y, fRadius, fRadius);
        //rect(xy.x, xy.y, radius, radius);

        // draw a center
        fill(0,127);
        rect(xy.x, xy.y, 2, 2);
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
            auto dxy = balls[id]->xy - balls[i]->xy;
            auto dist = maths::distance(balls[id]->xy, balls[i]->xy);

            // Minimal distance between balls before they are 'touching'
            auto minDist = (balls[id]->fRadius) + (balls[i]->fRadius);
            //printf("distance: %f (%f)\n",dist, minDist);

            if (dist < minDist) {
                //printf("distance: %d-%d;  %f (%f)\n",id, i, dist, minDist);
                stroke(255,0,0);
                strokeWeight(2);
                line(balls[id]->xy.x, balls[id]->xy.y, balls[i]->xy.x, balls[i]->xy.y);

                //console.log("2");
                //auto ang = maths::angle(balls[id]->xy, balls[i]->xy);
                auto ang = maths::atan2(dxy.y, dxy.x);
                auto targetX = balls[id]->xy.x + maths::cos(ang) * minDist;
                auto targetY = balls[id]->xy.y + maths::sin(ang) * minDist;
                maths::vec2f axy{ (targetX - balls[id]->xy.x) * spring , (targetY - balls[id]->xy.y) * spring };

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


