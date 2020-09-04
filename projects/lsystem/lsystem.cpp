#include "p5.hpp"
#include <vector>
#include <string>

// http://web.mit.edu/~eric_r/Public/lsystems/lsys3D12.pde

using namespace p5;


struct Rule {
    char fPrimary;
    std::string fSub;
};


double x;
double y;
double currentAngle = 0;
double step = 20;
double angle = 90;

// Lindemeyer stuff


std::string theString = "A";
int numLoops = 8;
std::vector<Rule> theRules;
int whereInString = 0;

// interpret an L-system
std::string lindenmayer(const std::string s) 
{
    std::string outputstring; // start a blank output string

    // iterate through 'therules' looking for symbol matches:
    for (size_t i = 0; i < s.size(); i++) {
        bool ismatch = false; // by default, no match
        for (size_t j = 0; j < theRules.size(); j++) {
            if (s[i] == theRules[j].fPrimary) {
                outputstring += theRules[j].fSub; // write substitution
                ismatch = true; // we have a match, so don't copy over symbol
                break; // get outta this for() loop
            }
        }
        // if nothing matches, just copy the symbol over.
        if (!ismatch)
            outputstring += s[i];
    }

    return outputstring; // send out the modified string
}

Pixel randomColor()
{
    // give me some random color values:
    uint32_t r = (uint32_t)random(128, 255);
    uint32_t g = (uint32_t)random(0, 192);
    uint32_t b = (uint32_t)random(0, 50);
    uint32_t a = (uint32_t)random(50, 100);

    return Pixel(r,g,b,a);
}
// this is a custom function that draws turtle commands
void drawIt(const char k) 
{

    if (k == 'F') { // draw forward
      // polar to cartesian based on step and currentangle:
        double x1 = x + step * cos(radians(currentAngle));
        double y1 = y + step * sin(radians(currentAngle));
        line(x, y, x1, y1); // connect the old and the new

        // update the turtle's position:
        x = x1;
        y = y1;
    }
    else if (k == '+') {
        currentAngle += angle; // turn left
    }
    else if (k == '-') {
        currentAngle -= angle; // turn right
    }



    // pick a gaussian (D&D) distribution for the radius:
    double radius = 0;
    radius += random(0, 15);
    radius += random(0, 15);
    radius += random(0, 15);
    radius = radius / 3;

    // draw the stuff:
    fill(randomColor());
    ellipse(x, y, radius, radius);
}

void draw() 
{
    stroke(0);

    // draw the current character in the string:
    drawIt(theString[whereInString]);

    // increment the point for where we're reading the string.
    // wrap around at the end.
    whereInString++;

    if (whereInString > theString.size() - 1)
    {
        whereInString = 0;
        clear();
    }
}

void setup()
{
    theRules.push_back({ 'A', "-BF+AFA+FB-" });
    theRules.push_back({ 'B', "+AF-BFB-FA+" });

	createCanvas(displayWidth, displayHeight);
    frameRate(30);
	//background(255);


	x = 0;
	y = height - 1;
    //y = 0;
	// Compute the L-System
	for (int i = 0; i < numLoops; i++) {
		theString = lindenmayer(theString);
	}

    fullscreen();
}

// halt the entire application once a user
// presses the 'escape' key
void keyReleased(const KeyboardEvent& e)
{
    if (e.keyCode == VK_ESCAPE)
        halt();
}
