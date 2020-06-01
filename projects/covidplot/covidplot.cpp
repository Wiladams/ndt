#include "p5.hpp"
#include "locations.h"

using namespace p5;

Pixel  randomColor()
{
	int r = (int)random(30, 255);
	int g = (int)random(30, 255);
	int b = (int)random(30, 255);

	Pixel c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = 255;

	return c;
}

void draw()
{
	int cellWidth = width / nLocations;
	
	background(230);
	strokeWeight(1);
	stroke(0);
	fill(0xc0);

	for (int i = 0; i < nLocations; i++) {
		int cellHeight = map(locations[i].population, 0, 2000000000, 20, height);
		int x = map(i, 0, nLocations - 1, 0, width-cellWidth);
		int y = 0;

		fill(randomColor());
		rect(x, y, cellWidth, cellHeight);
	}

	noLoop();
}

void setup()
{
	createCanvas(displayWidth, displayHeight-80);
	translate(0, height);
	scale(1, -1);
}