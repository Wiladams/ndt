#include "p5.hpp"
#include <vector>

#include "analogclock.hpp"

using namespace p5;

std::vector<AnalogClock> clocks;


/*
void keyTyped(const KeyEvent& event)
{
    // if [Space] toggle layering
    if (keyCode == ' ') {
        if (gIsLayered)
            noLayered();
        else {
            layered();
        }
    }
}
*/

Color  randomColor()
{
    uint8_t r = (uint8_t)random(30, 255);
    uint8_t g = (uint8_t)random(30, 255);
    uint8_t b = (uint8_t)random(30, 255);
    uint8_t a = (uint8_t)random(0x7f, 255);

    return { r,g,b, a};
}

void addClock()
{
    //printf("addingClock\n");
    clocks.push_back(AnalogClock(random(120, displayWidth - 120), random(120, displayHeight - 120), random(90, 300)));
    clocks[clocks.size() - 1].setColor(randomColor());
}

void removeClock()
{
    //printf("removingClock\n");
    if (clocks.size() < 2)
        return;

    clocks.pop_back();
}

void keyReleased(const KeyEvent& event)
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

void draw()
{
    if (!gIsLayered)
        background(0xc0);
    else
        clear();

    for (size_t i = 0; i < clocks.size(); i++) {
        clocks[i].draw(*gAppSurface);
    }

}



void setup()
{

    
    clocks.push_back(AnalogClock(displayWidth/2, displayHeight/2, 90));

    // Create a transparent canvas the size of the screen
    //createCanvas((long)size.w, (long)size.h);
    createCanvas(displayWidth, displayHeight);
    layered();
    setWindowPosition(0, 0);

    //frameRate(3);
}
