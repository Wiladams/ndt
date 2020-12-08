#include "p5.hpp"

using namespace p5;

void draw()
{
	background(255, 255, 0);
	stroke(0);

	// print lines of different lengths
	// increasing width as we go
	for (size_t i = 1; i <= 192; i++) {
		auto w = map(i, 1, 192, 0.25, 2);
		strokeWeight(w);

		line(0, i*2, i, i*2);
	}

	// To experiment with line thickness
	strokeWeight(1.0);

	//noLoop();
}

void setup()
{
	//fullscreen();
	createCanvas(1280, displayHeight/2);

	// Set user space units to be 96/inch
	gAppSurface->setPpiUnits(systemPpi, 96);
}