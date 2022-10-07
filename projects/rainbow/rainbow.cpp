#include "p5.hpp"

#include "colorsampler.h"
#include "geometry.h"

using namespace p5;

BLImage potOfGold;


class GradientBezier : public IDrawable
{
	GradientSampler1D fSampler;
	maths::vec2f p1;
	maths::vec2f p2;
	maths::vec2f p3;
	maths::vec2f p4;
	size_t fSegments;

public:
	GradientBezier(const BLGradient& grad, float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, size_t segments = 60)
		:p1{ x1, y1 }
		,p2{x2, y2}
		, p3{ x3, y3 }
		,p4{x4, y4}
		,fSegments(segments)
		,fSampler(grad)
	{
	}

	virtual void draw(IGraphics & ctx) override
	{
		// Get starting point
		maths::vec2f lp = maths::bezier_cubic_eval(p1, p2, p3, p4, 0);

		int i = 1;
		while (i <= fSegments) {
			REAL u = (REAL)i / fSegments;

			maths::vec2f p = maths::bezier_cubic_eval(p1, p2, p3, p4, u);

			// draw line segment from last point to current point
			// figure out color for segment
			stroke(fSampler(u));
			ctx.line(lp.x, lp.y, p.x, p.y);

			// Assign current to last
			lp = p;

			i = i + 1;
		}
	}
};


void setup()
{
	// Load the pot of gold image
	auto err = potOfGold.readFromFile("potofgold.png");

	if (err)
		printf("could not load pot of gold (%d)\n", err);

	//createCanvas(displayWidth, displayHeight, "rainbow");
	fullscreen();
}



void draw()
{
	constexpr int baseSize = 200;

	// Clear the background before drawing
	clear();
	//background(255);

	// draw pot of gold at end of rainbow
	int potX = canvasWidth - potOfGold.width() - 200;
	int potY = canvasHeight - potOfGold.height() - 300;

	// Draw the pot of gold first
	p5::image(potOfGold, potX, potY);
	flush();

	
	// Draw the actual rainbow
	int yoffset = 0;
	VisibleLightSampler colorSampler;

	strokeWeight(2);

	// Alpha blending is not right
	///*
	for (int x = 0; x <= baseSize; x++) {
		double offset = map(x, 0, baseSize, 0, 1.0);

		auto c0 = colorSampler(offset);
		auto c1 = c0;
		c0.setA(110);

		// calculate stops along the curve
		BLGradient grad(BLLinearGradientValues(0, 0, 0, 0));
		grad.addStop(0, c0);
		grad.addStop(0.20, c1);
		grad.addStop(0.80, c1);
		grad.addStop(1.0, c0);

		double finalX = map(x, 0, baseSize, (double)potX+potOfGold.width()-10, (double)potX+30);
		double finalY = (int)(potY+36);

		GradientBezier bez(grad, x, canvasHeight, canvasWidth * 0.3, yoffset, canvasWidth * 0.6, yoffset, finalX, finalY,1200);
		bez.draw(*gAppSurface);

		yoffset += 1;
	}
	//*/
	noLoop();
}

// Exit the program if the user presses escape
void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}
