/*
	Play with two things here
	1) using a canvas and setting pixels directly
	2) Using wavelength to create rgb values
		a) use varying gamma
*/

#include "p5.hpp"
#include "coloring.h"
#include "canvas.h"

#include <memory>
#include <iostream>

using namespace p5;

std::shared_ptr<GCanvas> canvas;

void setup()
{
	createCanvas(displayWidth, 300);

	setCanvasPosition(0, 0);

	canvas = std::make_shared<GCanvas>(canvasWidth, canvasHeight);

	noLoop();
}

void draw()
{
	//std::cout << "colorama.draw()" << std::endl;

	background(0xff);

	for (int y = 0; y < canvasHeight; y+=1) {
		double gamma = map(y, 0, (double)canvasHeight -1, 0.025, 2.5);
		for (int x = 0; x < canvasWidth - 1; x++) {
			double wl = map(x, 0, (double)canvasWidth - 1, 380, 780);
			auto c = ndt::ColorRGBAFromWavelength(wl, gamma);
			auto p = BLRgba32(c.r * 255, c.g * 255, c.b * 255, 255);
			canvas->set(x, y, p);
		}
	}

	image(canvas->getImage(), 0, 0);
}
