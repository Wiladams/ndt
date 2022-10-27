#include "p5.hpp"
#include <vector>

#include "analogclock.hpp"

using namespace p5;

std::vector<AnalogClock> clocks;

constexpr float spring = 0.03f;        //0.05
constexpr float damper = 0.997f;

Pixel  randomColor()
{
    uint8_t r = (uint8_t)random(30, 255);
    uint8_t g = (uint8_t)random(30, 255);
    uint8_t b = (uint8_t)random(30, 255);
    uint8_t a = (uint8_t)random(0x7f, 255);

    return { r,g,b, a};
}

void addClock()
{
    clocks.push_back(AnalogClock(random(120, canvasWidth - 120), random(120, canvasHeight - 120), random(AnalogClock::SmallestRadius, AnalogClock::LargestRadius)));
    clocks[clocks.size() - 1].setColor(randomColor());
}

void removeClock()
{
    if (clocks.size() < 2)
        return;

    clocks.pop_back();
}

void keyReleased(const KeyboardEvent& event)
{
    switch (keyCode) {
        case VK_ESCAPE: {
            halt();
        }
        break;
        case VK_UP: {
            addClock();
        }
        break;
        case VK_DOWN: {
            removeClock();
        }
        break;
    }
}



void drawConnections()
{
    //printf("COLISSION\n");
    int numBalls = clocks.size();

    for (int id = 0; id < numBalls - 1; id++)
    {
        for (int i = id + 1; i < numBalls; i++)
        {
            //printf("ID: %d  i: %d\n", id, i);

            // Minimal distance between balls before they are 'touching'
            auto minDist = (clocks[id].fRadius) + (clocks[i].fRadius);

            auto dxy = clocks[id].fCenter - clocks[i].fCenter;
            auto dist = maths::distance(clocks[id].fCenter, clocks[i].fCenter);

            
            //printf("distance: %f (%f)\n",dist, minDist);

            if (dist < minDist) {
                //printf("distance: %d-%d;  %f (%f)\n",id, i, dist, minDist);
                stroke(255, 255, 0);
                strokeWeight(5);
                line(clocks[id].fCenter.x, clocks[id].fCenter.y, clocks[i].fCenter.x, clocks[i].fCenter.y);

                // Do the following to introduce some drag
                // once the balls touch each other
                // Calculate an angle of incident, so we can reverse it
                //auto ang = maths::angle(clocks[id].fCenter, clocks[i].fCenter);
                auto ang = maths::atan2(dxy.y, dxy.x);
                auto targetX = clocks[id].fCenter.x + maths::cos(ang) * minDist;
                auto targetY = clocks[id].fCenter.y + maths::sin(ang) * minDist;
                maths::vec2f axy = maths::vec2f{ (targetX - clocks[id].fCenter.x)* spring, (targetY - clocks[id].fCenter.y)* spring };

                clocks[i].setDirection(-axy);
                clocks[id].setDirection(axy);

                clocks[i].adjustSpeed(damper);
                clocks[id].adjustSpeed(damper);
            }
        }
    }

}

void draw()
{
    if (!isLayered())
        background(0xc0);
    else
        clear();

    drawConnections();

    for (size_t i = 0; i < clocks.size(); i++) {
        clocks[i].draw(*gAppSurface);
    }

}



void setup()
{   

    // Create a transparent canvas the size of the screen
    //createCanvas(1920, 1080);
    fullscreen();

    clocks.push_back(AnalogClock(canvasWidth / 2, canvasHeight / 2, 90));


    //frameRate(30);
}
