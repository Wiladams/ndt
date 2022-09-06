#include "p5.hpp"

using namespace p5;

//
// demonstrating how to us setPpiUnits()
// to setup user determined drawing units
//
// Draw some lines that should be specific
// physical lengths on the screen
//
void drawHundreds()
{
	stroke(127);

	//
	// print lines of different lengths
	// increasing width as we go
	strokeWeight(2);
	line(0, 50, 50, 50);	// half inch
	line(0, 75, 100, 75);	// one inch
	line(0, 100, 150, 100);	// one and a half

	// Use fractins of an inch
	//strokeWeight(1.0/96);
	//line(0, .5, 0.5, 0.5);	// half inch
	//line(0, .75, 1, 0.75);	// one inch
	//line(0, 1, 1.5, 1);		// one and a half

}

void draw()
{
	//background(255, 255, 0);

	stroke(Pixel(255,255,255));
	fill(Pixel(255,255,255));
	textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
	textSize(24);
	text("User Units 100/inch", canvasWidth/2,24);

	drawHundreds();

	strokeWeight(4);
	stroke(127);

	noLoop();
}

void setup()
{
	createCanvas(1280, displayHeight/2, "calibration");

	// Set user space units to be 96/inch
	setUnitsPerInch(100);
	//setUnitsPerInch(1);
}