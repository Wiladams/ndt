#include "p5.hpp"

#include "gview.h"

using namespace p5;

PageView mainView(BLRect(100, 100, 300, 300));

void setup()
{
	createCanvas(600, 600);
}

void draw()
{
	background(245,246,247);

	mainView.draw(gAppSurface);
}