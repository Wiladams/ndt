#include "p5.hpp"

using namespace p5;

static const int NUMSINES = 20; // how many of these things can we do at once?
double sines[NUMSINES]; // an array to hold all the current angles
double rad; // an initial radius value for the central sine
int i; // a counter variable
    
// play with these to get a sense of what's going on:
double fund = 0.005; // the speed of the central sine
double ratio = 1; // what multiplier for speed is each additional sine?
int galpha = 50; // how opaque is the tracing system
    
bool trace = false; // are we tracing?

    
void draw()
{
    if (isLayered())
    {
        // clear surface
        background(0,0);
    }

    if (!trace) {
        background(204); // clear screen if showing geometry
        stroke(0, 255); // black pen
        noFill(); // don't fill
    }
    
      // MAIN ACTION
      push(); // start a transformation matrix
      translate(width / 2.0, height / 2.0); // move to middle of screen
    
      for (int i = 0; i < NUMSINES; i++) {
        double erad = 0; // radius for small "point" within circle... this is the 'pen' when tracing
        
        // setup for tracing
        if (trace) {
          stroke(0, 0, (int)(255 * (float(i) / NUMSINES)), galpha); // blue
          fill(0, 0, 255, (int)(galpha / 2.0)); // also, um, blue
          erad = 5.0 * (1.0 - float(i) / NUMSINES); // pen width will be related to which sine
        }
        double radius = rad / (i + 1); // radius for circle itself
        rotate(sines[i]); // rotate circle
        if (!trace) ellipse(0, 0, radius * 2, radius * 2); // if we're simulating, draw the sine
        push(); // go up one level
        translate(0, radius); // move to sine edge
        if (!trace) 
            ellipse(0, 0, 5, 5); // draw a little circle
        if (trace) 
            ellipse(0, 0, erad, erad); // draw with erad if tracing
        
        pop(); // go down one level
        translate(0, radius); // move into position for next sine
        sines[i] = fmod((sines[i] + (fund + (fund * i * ratio))) , maths::Pi2); // update angle based on fundamental
      }
    
      pop(); // pop down final transformation
}
   

int T_SP = ' ';

void keyTyped(const KeyEvent& event)
{
    if (keyCode == T_SP) {
        trace = !trace;
        background(255);
    }
}




void setup()
    {
        createCanvas(710, 400);
        frameRate(60);

        rad = height / 4; // compute radius for central circle
        background(204); // clear the screen

        for (int i = 0; i < NUMSINES; i++) {
            sines[i] = maths::Pi; // start EVERYBODY facing NORTH
        }
    }
