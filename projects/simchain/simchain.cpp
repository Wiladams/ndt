#include "p5.hpp"

#include <vector>

using namespace p5;

double gRadius = 20.0;
double gravity = 9.0;    // 9.0
double mass = 2.0;       // 2.0
double gDamping = 0.7;   // 0.7
double gStiffness = 0.2; // 0.2

/*
    A Spring2D is the essential data structure
    that represents the bahavior of a spring.  It is
    modeled as a single point with a radius,
    mass, gravity, stiffness, and damping.

    A force is applied to the spring, and its parameters
    will determine how much it will move in which direction.
*/
struct Spring2D {
    double x;
    double y;
    double radius;
    double vx;
    double vy;
    double mass;
    double gravity;
    double stiffness;
    double damping;
    BLRgba32 color;

    static const Spring2D empty;

    Spring2D() {};


    Spring2D(int xpos, int ypos, double m, double g, BLRgba32 c)
    {
        x = xpos;   // The x- and y-coordinates
        y = ypos;
        vx = 0;     // The x- and y-axis velocities
        vy = 0;
        mass = m;
        gravity = g;
        radius = gRadius;
        stiffness = gStiffness;
        damping = gDamping;
        color = c;
    }

    bool operator==(const Spring2D& rhs)
    {
        // Either they are the exact same object
        // or they have the exact same attributes
        return (&rhs == this) ||
            ((x == rhs.x) &&
                (y == rhs.y) &&
                (radius == rhs.radius));
    }

    void update(int targetX, int targetY)
    {
        auto forceX = (targetX - x) * stiffness;
        auto ax = forceX / mass;

        vx = damping * (vx + ax);
        x = x + vx;
        auto forceY = (targetY - y) * stiffness;
        forceY = forceY + gravity;
        auto ay = forceY / mass;
        vy = damping * (vy + ay);
        y = y + vy;
    }

    void display(double nx, double ny)
    {
        noStroke();
        fill(color);
        ellipse(x, y, radius, radius);
        stroke(255);
        line(x, y, nx, ny);
    }

};

const Spring2D Spring2D::empty;

Pixel  randomColor()
{
    uint8_t r = (uint8_t)random(30, 255);
    uint8_t g = (uint8_t)random(30, 255);
    uint8_t b = (uint8_t)random(30, 255);

    return { r,g,b,255 };
}

std::vector<Spring2D> springs;
Spring2D headSpring(canvasWidth / 2, canvasHeight / 2, mass, gravity, randomColor());


void addSpring()
{
    Spring2D aspring(canvasWidth / 2, canvasHeight / 2, mass, gravity, randomColor());


    springs.push_back(aspring);
}

// Remove a single spring
void removeSpring()
{
    if (springs.size() > 1) {
        springs.pop_back();
    }
}

void clearSprings()
{
    springs.clear();
}

void reset()
{
    // Inputs: x, y, mass, gravity
    clearSprings();

    addSpring();
}

void setup()
{
    fullscreen();
    frameRate(30);

    reset();
}

void keyReleased(const KeyboardEvent& event)
{
    // Up arrow, add more
    // Down arrow, remove one
    //print("keyReleased: ", event, keyCode)
    if (keyCode == VK_UP) {
        addSpring();
    }
    else if (keyCode == VK_DOWN) {
        removeSpring();
    }
    else if (keyCode == VK_ESCAPE) {
        halt();
    }
}


void draw()
{
    clear();
    background(color(10, 0, 0, 117));

    // Let the top spring follow the mouse
    headSpring.update(mouseX, mouseY);
    headSpring.display(mouseX, mouseY);


    // iterate through rest of springs
    Spring2D* currentSpring = &headSpring;

    for (int i = 0; i < springs.size(); i++)
    {
        springs[i].update(currentSpring->x, currentSpring->y);
        springs[i].display(currentSpring->x, currentSpring->y);

        currentSpring = &springs[i];
    }

}
