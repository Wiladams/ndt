#include "p5.hpp"

#include "colorsampler.h"
#include "graphic.hpp"
#include "bezier.hpp"

using namespace p5;

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
		:p1(x1, y1),
		p2(x2, y2),
		p3(x3, y3),
		p4(x4, y4),
		fSegments(segments),
		fSampler(grad)
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
	// Load the pot of gold image
	auto err = potOfGold.readFromFile("potofgold.png");

	if (err)
		printf("could not load pot of gold (%d)\n", err);
}

void setup()
{
	// Create a canvas to cover the whole screen
	createCanvas(displayWidth, displayHeight);
	
	// Make the background invisible
	layered();
	setWindowPosition(0, 0);
}



void draw()
{
	static const int baseSize = 300;

	// Clear the background before drawing
	clear();

	// draw pot of gold at end of rainbow
	int potX = width - potOfGold.width() - 200;
	int potY = height - potOfGold.height() - 300;

	// Draw the pot of gold first
	p5::image(potOfGold, potX, potY);
	flush();


	// Draw the actual rainbow
	int yoffset = 0;
	VisibleLightSampler colorSampler;

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

		int finalX = (int)map(x, 0, baseSize, (double)potX+potOfGold.width()-10, (double)potX+30);
		int finalY = (int)(potY+36);

		GradientBezier bez(grad, x, height, width * 0.3, yoffset, width * 0.6, yoffset, finalX, finalY,1200);
		bez.draw(gAppSurface);


		yoffset += 1;
	}

	noLoop();
}

// Exit the program if the user presses escape
void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}
