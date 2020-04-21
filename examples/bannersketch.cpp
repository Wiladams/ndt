
/*
PROCESSINGJS.COM HEADER ANIMATION
MIT License - F1lT3R/Hyper-Metrix
Native Processing Compatible

Reference: http://processingjs.org/sketches/bannerSketch.pde
*/

/*
	Key Commands
	Esc - quit the program
	[Space] - toggle between layered and not layered views
*/
#include "p5.hpp"

// Set number of circles
static const int count = 40;
// Set maximum and minimum circle size
int maxSize = 100;
int minSize = 20;

// Build float array to store circle properties
double e[count][5];


// Set size of dot in circle center
double ds = 2;

// Set drag switch to false
bool dragging = false;

// integers showing which circle (the first index in e) that's locked, and its position in relation to the mouse
int lockedCircle;
int lockedOffsetX;
int lockedOffsetY;

// If user presses mouse...
void mousePressed(const MouseEvent& ev) 
{
	// Look for a circle the mouse is in, then lock that circle to the mouse
	// Loop through all circles to find which one is locked
	for (int j = 0; j< count; j++) {
		// If the circles are close...
		if (sq(e[j][0] - mouseX) + sq(e[j][1] - mouseY) < sq(e[j][2] / 2)) {
			// Store data showing that this circle is locked, and where in relation to the cursor it was
			lockedCircle = j;
			lockedOffsetX = mouseX - (int)e[j][0];
			lockedOffsetY = mouseY - (int)e[j][1];

			// Break out of the loop because we found our circle
			dragging = true;
			break;
		}
	}
}

// If user releases mouse...
void mouseReleased(const MouseEvent& e)
{
	// ..user is no-longer dragging
	dragging = false;
}

void keyTyped(const KeyEvent& event)
{
	// toggle between layered and not layered
	if (keyCode == ' ') {
		if (gIsLayered) {
			noLayered();
		}
		else {
			layered();
		}
	}
}


// Press the Esc key to stop the application
void keyReleased(const KeyEvent& event)
{

	if (keyCode == VK_ESCAPE) {
		halt();
	}
}


void draw() {
	// Fill background black
	if (gIsLayered) {
		clear();
		background(100, 100, 100, 10);
	} else {
		background(0);
	}

	// Begin looping through circle array
	for (int j = 0; j< count; j++) {
		// Disable shape stroke/border
		noStroke();

		// Cache diameter and radius of current circle
		double radi = e[j][2];
		double diam = radi / 2.0;
		if (sq(e[j][0] - mouseX) + sq(e[j][1] - mouseY) < sq(e[j][2] / 2.0))
			fill(64, 187, 128, 100); // green if mouseover
		else
			fill(64, 128, 187, 100); // regular

		if ((lockedCircle == j && dragging)) {
			// Move the particle's coordinates to the mouse's position, minus its original offset
			e[j][0] = (double)mouseX - lockedOffsetX;
			e[j][1] = (double)mouseY - lockedOffsetY;
		}
		
		// Draw circle
		ellipse(e[j][0], e[j][1], radi, radi);

		// Move circle
		e[j][0] += e[j][3];
		e[j][1] += e[j][4];


		// Wrap edges of canvas so circles leave the top
		// and re-enter the bottom, etc...
		if (e[j][0] < -diam) {
			e[j][0] = width + diam;
		}
		if (e[j][0] > width + diam) {
			e[j][0] = -diam;
		}
		if (e[j][1] < 0 - diam) {
			e[j][1] = height + diam;
		}
		if (e[j][1] > height + diam) {
			e[j][1] = -diam;
		}


		// If current circle is selected...
		if ((lockedCircle == j && dragging)) {
			// Set fill color of center dot to white..
			fill(255, 255, 255, 255);
			// ..and set stroke color of line to green.
			stroke(128, 255, 0, 100);
		}
		else {
			// otherwise set center dot color to black.. 
			fill(0, 0, 0, 255);
			// and set line color to turquoise.
			stroke(64, 128, 128, 255);
		}

		// Loop through all circles
		for (int k = 0; k< count; k++) {
			// If the circles are close...
			if (sq(e[j][0] - e[k][0]) + sq(e[j][1] - e[k][1]) < sq(diam)) {
				// Stroke a line from current circle to adjacent circle
				line(e[j][0], e[j][1], e[k][0], e[k][1]);
			}
		}

		// Turn off stroke/border
		noStroke();
		
		// Draw dot in center of circle
		rect(e[j][0] - ds, e[j][1] - ds, ds * 2, ds * 2);
	}
}

// Set up canvas
void setup() {
	// Frame rate
	frameRate(30);

	// Size of canvas (width,height)
	createCanvas(displayWidth, 78);
	setWindowPosition(0, 0);
	layered();

	// Stroke/line/border thickness
	strokeWeight(1);
	rectMode(CENTER);
	ellipseMode(CENTER);

	// Initiate array with random values for circles
	for (int j = 0; j < count; j++) {
		e[j][0] = random((double)width); // X 
		e[j][1] = random((double)height); // Y
		e[j][2] = random(minSize, maxSize); // Radius        
		e[j][3] = random(-.12, .12); // X Speed
		e[j][4] = random(-.12, .12); // Y Speed
	}
}
