#include "p5.hpp"

#include "analogclock.hpp"

using namespace p5;

AnalogClock clock1(0, 0);

void keyTyped(const KeyEvent& event)
{
    // if [Space] toggle layering
    if (keyCode == ' ') {
        if (gIsLayered)
            noLayered();
        else
            layered();
    }
}

void keyReleased(const KeyEvent& event)
{
    if (event.keyCode == VK_ESCAPE)
        halt();
}

void draw()
{
    if (!gIsLayered)
        background(0xc0);
    else
        clear();

    clock1.draw(*gAppSurface);
}

void setup()
{
    auto size = AnalogClock::getPreferredSize();
    createCanvas((long)size.w, (long)size.h);
}
