#include "p5.hpp"
#include "coloring.h"
#include "canvas.h"
#include <memory>

using namespace p5;

std::shared_ptr<GCanvas> canvas;

void setup()
{
	createCanvas(displayWidth, 300);
	//createCanvas(100, 10);

	setWindowPosition(0, 0);

	canvas = std::make_shared<GCanvas>(width, height);
}

void draw()
{
	background(0xff);

	for (int y = 0; y < 200; y+=1) {
		double gamma = map(y, 0, 200, 0.025, 1.25);
		for (int x = 0; x < width - 1; x++) {
			double wl = map(x, 0, width - 1, 380, 780);
			auto c = coloring::ColorRGBAFromWavelength(wl, gamma);
			auto p = BLRgba32(c.r * 255, c.g * 255, c.b * 255, 255);
			canvas->set(x, y, p);

		}
	}
	image(canvas->getImage(), 0, 0);
	noLoop();
}