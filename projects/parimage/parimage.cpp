#include "p5.hpp"
#include "imagesampler.h"

using namespace p5;
using namespace ndt;

BLImage img;

class ImageTinter : public virtual ISampler2D<BLRgba32>
{
	const ISampler2D<BLRgba32>& fSource;
	BLRgba fTint;

public:
	ImageTinter(const BLRgba& tint, ISampler2D<BLRgba32>& src)
		:fSource(src),
		fTint(tint)
	{
	}

	BLRgba32 operator()(double u, double v) const
	{
		// get pixel from source
		BLRgba32 srcC = fSource(u, v);

		// add tint
		BLRgba32 c;
		c.r = constrain(srcC.r + fTint.r, 0, 255);
		c.g = constrain(srcC.g + fTint.g, 0, 255);
		c.b = constrain(srcC.b + fTint.b, 0, 255);
		c.a = srcC.a;

		// return that value
		return c;
	}
};

void preload()
{
	// Load the various images
	img = loadImage("breakfast_small.jpg");
}

void setup()
{
	// Simple scaling can occur by varying the 
	// size of the window.
	// the image sampler will make it work
	//createCanvas(displayWidth, displayHeight);
	createCanvas(320, 240);
	noLoop();
}

void draw()
{
	// Chain two samplers together
	ndt::ImageSampler srcSampler(img);
	ImageTinter effect({ 100,65,0 }, srcSampler);

	int xskip = 1;
	int yskip = 1;

	loadPixels();

	for (int y = 0; y < height; y += yskip) {
		double v = (double)y / (height-1);
		for (int x = 0; x < width; x += xskip) {
			double u = (double)x / (width-1);

			set(x, y, effect(u, v));
		}
	}
}