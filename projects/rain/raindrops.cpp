#include "p5.hpp"
#include "particle.hpp"

#include <queue>

using namespace p5;

struct RainDrop : Particle
{
    bool fIsObstructed;

    RainDrop(p5::P5Vector location, p5::P5Vector velocity, double mass)
        :Particle(location, velocity, mass),
        fIsObstructed(false)
    {

    }

    virtual void update()
    {
        if (fIsObstructed) {
            fLocation.add(fVelocity * 0.25);
        } else {
            fLocation.add(fVelocity);
        }
    }
};

std::queue<RainDrop> rainDrops;
static int maxDrops = 200;
static p5::P5Vector windForce(0, 0);

BLBoxI Obstruction;


void keyPressed(const KeyEvent & event)
{
    switch (keyCode) {

        // direction of the wind
        case VK_RIGHT:
            windForce.x += 0.25;
        break;

        case VK_LEFT:
            windForce.x -= 0.25;
        break;

        // Number of rain drops +, -
        case VK_OEM_PLUS:
        case VK_ADD:            // The one on the numpad
            maxDrops += 50;
            //printf("maxDrops: %d\n", maxDrops);
            break;

        case VK_OEM_MINUS:
        case VK_SUBTRACT:       // The one on the numpad
            maxDrops -= 50;
            if (maxDrops < 200)
                maxDrops = 200;

            //printf("maxDrops: %d\n", maxDrops);
            break;
    }
}

void keyReleased(const KeyEvent& event)
{
    switch (keyCode) {
        case VK_ESCAPE: {
            halt();
        }
        break;
    }
}

void addDrop(int numDrops = 1)
{
    for (int i = 1; i <= numDrops; i++)
    {
        p5::P5Vector loc(random(0, width - 1), 0);
        p5::P5Vector vel(windForce.x+random(-2, 2), windForce.y+random(20, 35));

        rainDrops.push(RainDrop(loc, vel, 1.0));
    }
}

BOOL __stdcall enumwins(HWND hWnd, LPARAM lParam)
{

    if (::IsWindowVisible(hWnd) != 0)
    {
        if (hWnd != gAppWindow->getHandle())
        {
            RECT aRect;
            GetWindowRect(hWnd, &aRect);
            //printf("== enumwins %d %d %d %d ==\n", aRect.left, aRect.top, aRect.right, aRect.bottom);

            if (aRect.left >= 0 && aRect.right > 0) {
                BLBoxI aBox(aRect.left, aRect.top, aRect.right, aRect.bottom);

                if (::GetForegroundWindow() == hWnd) {
                    //stroke(255, 0, 0);
                    //strokeWeight(2);
                    //noFill();
                    //rect(aBox.x0, aBox.y0, aBox.x1 - aBox.x0, aBox.y1 - aBox.y0);
                    Obstruction = aBox;
                    return TRUE;
                }
                else {
                    //stroke(0, 0, 255);
                    //strokeWeight(2);
                    //noFill();
                    //rect(aBox.x0, aBox.y0, aBox.x1 - aBox.x0, aBox.y1 - aBox.y0);
                }
            }
        }
    }
 

    return TRUE;
}

void applyObstructions()
{
    ::EnumWindows(enumwins, (LPARAM)nullptr);

    size_t dropiSize = rainDrops.size();

    // go through all the drops, testing against obstructions
    for (size_t dropi = 0; dropi < dropiSize; dropi++) {
        RainDrop drop = rainDrops.front();
        rainDrops.pop();
        drop.fIsObstructed = false;
        if (Obstruction.contains(drop.fLocation.x, drop.fLocation.y))
        {
            drop.fIsObstructed = true;
        }
        rainDrops.push(drop);
    }
}

void draw()
{
    clear();

 
    // First get all the obstructions before we do 
    // our updating of positions.
    applyObstructions();

    int numDrops = rainDrops.size();

    for (int i = 1; i <= numDrops; i++)
    {
        RainDrop aDrop = rainDrops.front();
        rainDrops.pop();
        aDrop.update();
        aDrop.draw(*gAppSurface);

        if (gAppSurface->contains(aDrop.fLocation.x, aDrop.fLocation.y)) {
            rainDrops.push(aDrop);
        }
    }

    // Makeup for lost drops
    if (rainDrops.size() < maxDrops) {
        addDrop(maxDrops - rainDrops.size());
    }
}

void setup()
{
	createCanvas(displayWidth, displayHeight-40);
	layered();
	setWindowPosition(0, 0);
    //frameRate(4);
    // Seed the sky with drops
    for (int i = 1; i <= maxDrops; i++)
        addDrop();
}