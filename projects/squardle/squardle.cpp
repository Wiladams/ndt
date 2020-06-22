#include "p5.hpp"

using namespace p5;


Pixel appTabColor(255, 253, 205);
//Pixel appTabColor(0);
static const int maxShadowLength = 6;

Pixel randomColor(int alpha)
{
	return Pixel(random(0, 255), random(0, 255), random(0, 255), alpha);
}

void setup()
{
	createCanvas(800, 800);
}


void drawDiamonds(double radius, double size, int level, bool shadow = false)
{


	// Diamond path
	BLPath diamond;
	diamond.moveTo(-size / 2.0, 0);
	diamond.lineTo(0, size / 2.0);
	diamond.lineTo(size / 2.0, 0);
	diamond.lineTo(0, -size / 2.0);
	diamond.close();


	double segmentRads = radians(360 / 12);
	double angle = 0;

	push();
	int alpha = map(level, 9, 0, 60, 200);
	fill(randomColor(alpha));
	noStroke();

	if (shadow) {
		// Draw the drop-shadow diamond
		push();
		//int shadowLength = 4;
		int shadowLength = map(level, 9, 0, maxShadowLength, 0);

		for (int offset = 1; offset <= shadowLength; offset++) {
			//fill(0xc0, lerp(220, 10, (double)offset / shadowLength));
			fill(60, lerp(220, 10, (double)offset / shadowLength));
			for (int idx = 1; idx <= 12; idx++)
			{
				double x = radius * cos(angle) + offset;
				double y = radius * sin(angle) + offset;

				push();
				translate(x, y);
				rotate(angle);
				path(diamond);
				pop();

				angle += segmentRads;
			}
		}
		pop();
	}

	// Draw the regular diamond
	angle = 0;
	for (int idx = 1; idx <= 12; idx++)
	{
		double x = radius * cos(angle);
		double y = radius * sin(angle);

		push();
		translate(x, y);
		rotate(angle);
		path(diamond);
		pop();

		angle += segmentRads;
	}
	pop();

	if (level > 0) {
		//bool doShadow = level > 5;
		bool doShadow = true;

		rotate(segmentRads / 2.0);
		drawDiamonds(radius*0.705, size*0.70, level - 1, doShadow);
	}
}



void draw()
{
	double segmentRads = radians(360.0 / 12.0);
	double segAngle = segmentRads/2.0;

	background(appTabColor);

	int nFlowers = 5;
	noStroke();
	for (int i = 1; i <= nFlowers; i++) {
		push();
		//translate(width / 2.0, height / 2.0);
		translate(random(0, width), random(0, height));
		scale(random(0.125, 0.5));
		drawDiamonds(186, 100, 9, true);
		pop();
	}

	noLoop();
}