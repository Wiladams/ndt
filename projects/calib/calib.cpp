#include "p5.hpp"

using namespace p5;

//
// demonstrating how to us setPpiUnits()
// to setup user determined drawing units
//
// Draw some lines that should be specific
// physical lengths on the screen
//
void draw()
{
	//background(255, 255, 0);

	stroke(Pixel(255,255,255));
	fill(Pixel(255,255,255));
	textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
	textSize(24);
	text("User Units 96/inch", getWidth()/2,24);


	//
	// print lines of different lengths
	// increasing width as we go
	strokeWeight(4);
	stroke(127);
	line(48, 48, 96, 48);
	line(48, 72, 144, 72);
	line(48, 96, 192, 96);
	//
	noLoop();
}

void setup()
{
	//fullscreen();
	createCanvas(1280, displayHeight/2);

	// Set user space units to be 96/inch
	//gAppSurface->setPpiUnits(systemPpi, 96);
	gAppSurface->setPpiUnits(systemPpi, 260*0.45);
}