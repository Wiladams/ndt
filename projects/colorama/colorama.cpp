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
	//createCanvas(100, 10);

	setWindowPosition(0, 0);

	canvas = std::make_shared<GCanvas>(width, height);

	noLoop();
}

void draw()
{
	//std::cout << "colorama.draw()" << std::endl;

	background(0xff);

	for (int y = 0; y < height; y+=1) {
		double gamma = map(y, 0, (double)height-1, 0.025, 2.5);
		for (int x = 0; x < width - 1; x++) {
			double wl = map(x, 0, (double)width - 1, 380, 780);
			auto c = ndt::ColorRGBAFromWavelength(wl, gamma);
			auto p = BLRgba32(c.r * 255, c.g * 255, c.b * 255, 255);
			canvas->set(x, y, p);
		}
	}

	image(canvas->getImage(), 0, 0);
}

/*
// For fiddler.blend2d.com
int width;
int height;

inline double map(double value, double start1, double stop1, double start2, double stop2, bool withBounds = false)
{
  return start2 + (value - start1) * ((stop2 - start2) / (stop1 - start1));
}

void set(BLImageData &data, int x, int y, BLRgba32 c)
{
  ((BLRgba32 *)(data.pixelData))[(int)y * data.size.w + (int)x] = c;
}

BLImage render(const BLContextCreateInfo& cci)
{
  width = 640;
  height = 480;

  BLImage img(width, height, BL_FORMAT_PRGB32);
  BLImageData imageData;
  img.getData(&imageData);

  BLContext ctx(img, cci);

	//background(0xff);

	for (int y = 0; y < height; y+=1) {
		double gamma = map(y, 0, (double)height-1, 0.025, 2.5);
		for (int x = 0; x < width - 1; x++) {
			double wl = map(x, 0, (double)width - 1, 380, 780);
			auto c = ColorRGBAFromWavelength(wl, gamma);
			auto p = BLRgba32(c.r * 255, c.g * 255, c.b * 255, 255);
			set(imageData, x, y, p);
		}
	}


  return img;
}
*/