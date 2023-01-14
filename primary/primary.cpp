#include "p5.hpp"

#include "elements/framestats.h"
#include "canvas.h"

using namespace p5;

FrameStats _stats;

void setup()
{
	createCanvas(400, 400);
	frameRate(1);

	auto win = window(0, 0, 200, 200);
	win->setTitle("Primary Window");
}

void draw()
{
	clear();
	//printf("draw\n");
	_stats.draw(*gAppSurface);
}
