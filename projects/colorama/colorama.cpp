/*
	Play with two things here
	1) using a canvas and setting pixels directly
	2) Using wavelength to create rgb values
		a) use varying gamma
*/

#include "apphost.h"
#include "coloring.h"
#include "maths.hpp"

#include <memory>






void draw()
{
	//background(0xff);


	for (int y = 0; y < canvasHeight; y+=1) 
	{
		double gamma = maths::map(y, 0, (double)canvasHeight -1, 0.025, 2.5);
		for (int x = 0; x < canvasWidth - 1; x++) {
			double wl = maths::map(x, 0, (double)canvasWidth - 1, 380, 780);
			auto c = ndt::ColorRGBAFromWavelength(wl, gamma);
			maths::vec4b p = { c.b * 255, c.g * 255, c.r * 255,   255 };

			appFrameBuffer().setPixel(x, y, p);
		}
	}
}

void onLoad()
{
	createAppWindow(displayWidth, 300, "colorama");

	setCanvasPosition(0, 0);

	draw();
	screenRefresh();
}
