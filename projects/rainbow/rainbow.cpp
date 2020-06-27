#include "p5.hpp"
#include "sampler.hpp"
#include "colorsampler.h"
#include "graphic.hpp"
#include "bezier.hpp"

using namespace p5;


// Colors of the rainbow
uint8_t gAlpha = 255;
Pixel rColors[] = {
	{255,0,0,gAlpha},
	{255,165,0, gAlpha},
	{255, 255,0,gAlpha},
	{0,255,0,gAlpha},
	{0,0,255,gAlpha},
	{93,118,203,gAlpha},
	{143,94,154,gAlpha},
};
int nColors = sizeof(rColors) / sizeof(Pixel);

BLGradient gradient(BLLinearGradientValues(0, 0, 0, 0));
BLImage potOfGold;

class GradientBezier : public virtual IDrawable
{
	GradientSampler1D fSampler;
	BLPoint p1;
	BLPoint p2;
	BLPoint p3;
	BLPoint p4;
	size_t fSegments;

public:
	GradientBezier(const BLGradient& grad, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, size_t segments=60)
		:fSampler(grad),
		p1(x1, y1),
		p2(x2, y2),
		p3(x3, y3),
		p4(x4, y4),
		fSegments(segments)
	{
	}

	virtual void draw(IGraphics* ctx)
	{
		// Get starting point
		BLPoint lp = ndt::bezier_point(0, p1, p2, p3, p4);

		int i = 1;
		while (i <= fSegments) {
			double u = (double)i / fSegments;

			BLPoint p = ndt::bezier_point(u, p1, p2, p3, p4);

			// draw line segment from last point to current point
			// figure out color for segment
			stroke(fSampler(u));
			ctx->line(lp.x, lp.y, p.x, p.y);

			// Assign current to last
			lp = p;

			i = i + 1;
		}
	}
};

void preload()
{
	// Create the gradient to represent the rainbow colors
	for (int i = 0; i < nColors; i++) {
		double offset = map(i, 0, nColors - 1, 0, 1);
		gradient.addStop(offset, rColors[i]);
	}

	// Load the various images
	auto err = potOfGold.readFromFile("potofgold.png");

	if (err)
		printf("could not load pot of gold (%d)\n", err);
}

void setup()
{
	createCanvas(displayWidth, displayHeight);
	layered();
	setWindowPosition(0, 0);
}



void draw()
{
	static const int baseSize = 300;

	//if (isLayered()) {
		clear();
	//} else
	//{
	//	background(255);
	//}

	// draw pot of gold at end of rainbow
	int potX = width - potOfGold.width() - 200;
	int potY = height - potOfGold.height() - 300;

	p5::image(potOfGold, potX, potY);
	gAppSurface->flush();

	// Draw the actual rainbow
	int yoffset = 0;
	GradientSampler1D colorSampler(gradient);
	strokeWeight(2);
	for (int x = 0; x <= baseSize; x++) {
		double offset = map(x, 0, baseSize, 0, 1);

		auto c0 = colorSampler(offset);
		auto c1 = c0;
		c0.a = 10;

		// calculate stops along the curve
		BLGradient grad(BLLinearGradientValues(0, 0, 0, 0));
		grad.addStop(0, c0);
		grad.addStop(0.20, c1);
		grad.addStop(0.80, c1);
		grad.addStop(1.0, c0);

		int finalX = map(x, 0, baseSize, potX+potOfGold.width()-10, potX+30);
		int finalY = potY+36;

		GradientBezier bez(grad, x, height, width * 0.3, yoffset, width * 0.6, yoffset, finalX, finalY, 1200);
		bez.draw(gAppSurface);


		yoffset += 1;
	}

	noLoop();
}


void keyReleased(const KeyEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}
