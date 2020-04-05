#include "p5.hpp"
#include "Stack.hpp"

float gRadius = 20;
float gravity = 7.0;    // 9.0
float mass = 1.80;       // 2.0
float gDamping = 0.7;   // 0.7
float gStiffness = 0.3; // 0.2

/*
    A Spring2D is the essential data structure
    that represents the bahavior of a spring.  It is
    modeled as a single point with a radius,
    mass, gravity, stiffness, and damping.

    A force is applied to the spring, and its parameters
    will determine how much it will move in which direction.
*/
struct Spring2D {
    int x;
    int y;
    int radius;
    int vx;
    int vy;
    float mass;
    float gravity;
    float stiffness;
    float damping;
    PixRGBA color;

    Spring2D(int xpos, int ypos, float m, float g, PixRGBA c)
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

    void update(int targetX, int targetY)
    {
        float forceX = (targetX - x) * stiffness;
        float ax = forceX / mass;
        
        vx = damping * (vx + ax);
        x = x + vx;
        float forceY = (targetY - y) * stiffness;
        forceY = forceY + gravity;
        float ay = forceY / mass;
        vy = damping * (vy + ay);
        y = y + vy;
    }

    void display(int nx, int ny)
    {
        noStroke();
        fill(color);
        ellipse(x, y, radius, radius);
        stroke(255);
        line(x, y, nx, ny);
    }

};


Stack<Spring2D *> springs;
Spring2D * headSpring = nullptr;

PixRGBA  randomColor()
{
    int r = random(30,255);
    int g = random(30,255);
    int b = random(30,255);
    
    //return PixRGBA(r,g,b,126);
    return PixRGBA(r,g,b,255);
}

void addSpring()
{
    Spring2D * aspring = new Spring2D(width/2, height / 2, mass, gravity, randomColor());
    
    if (headSpring == nullptr) {
        headSpring = aspring;
    } else {
        springs.push(aspring);
    }
}

// Remove a single spring
void removeSpring()
{
    if (springs.length() > 1) {
        Spring2D *aspring = (Spring2D *)springs.pop();
        //delete aspring;
    }
}

void clearSprings()
{
    Spring2D * spring = (Spring2D *)springs.pop();
    while (spring != nullptr) {
        delete spring;
        spring = (Spring2D *)springs.pop();
    }
}

void reset()
{
    // Inputs: x, y, mass, gravity
    clearSprings();
    if (headSpring != nullptr) {
        delete headSpring;
        headSpring = nullptr;
    }

    addSpring();
    addSpring();
}

int T_SP = ' ';
int VK_UP = 38;
int VK_DOWN = 40;

void keyReleased(const KeyEvent &event)
{
    // Up arrow, add more
    // Down arrow, remove one
    //print("keyReleased: ", event, keyCode)
    if (keyCode == VK_UP) {
        addSpring();
    } else if (keyCode == VK_DOWN) {
        removeSpring();
    }
}


void keyTyped(const KeyEvent &event)
{
    // If the user types a '<sp>' reset
    // the chain to 1 node
    if (keyCode == T_SP) {
        reset();
    }
}

void draw()
{
    background(127);

    if (headSpring == nullptr) {
        return;
    }

    //printf("Mouse: %d %d\n", mouseX, mouseY);

    headSpring->update(mouseX, mouseY);
    headSpring->display(mouseX, mouseY);
  
  
    // iterate through rest of springs
    Spring2D * currentSpring = headSpring;

    Spring2D * spring;
    for (int i=0; i < springs.length(); i++)
    {
        spring = springs[i];
        spring->update(currentSpring->x, currentSpring->y);
        spring->display(currentSpring->x, currentSpring->y);
        currentSpring = spring;
    }


}

void setup()
{
    createCanvas(1280, 1024);
    setFrameRate(30);
    //fill(PixRGBA(0x7fffffff));

    reset();
}
