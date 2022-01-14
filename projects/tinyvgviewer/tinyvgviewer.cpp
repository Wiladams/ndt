// 
//  tinyvgviewer
//  Read and display Tiny Vector Graphic files
//
#include "p5.hpp"
#include "tinyvggraphic.h"

using namespace p5;
using namespace tinyvg;

// At the application level
// quit the app if "ESC" is pressed
void keyReleased(const KeyboardEvent &e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

// At the application level
// draw the background of the entire app
void draw() 
{
	// Clear to background, whatever it is
	if (!isLayered())
		background(255, 253, 205);	// (245 , 246, 247);
	else
		clear();
}


void setup()
{
	//fullscreen();
	createCanvas(800, 800);

	gAppSurface->setPpiUnits(systemDpi, 96);

	// create some graphics from tiny vg files
	auto g1 = TinyVGGraphic::createFromFilename("comic.tvg");
	auto g2 = TinyVGGraphic::createFromFilename("tiger.tvg");
	auto g3 = TinyVGGraphic::createFromFilename("shield.tvg");
	auto g4 = TinyVGGraphic::createFromFilename("chart.tvg");
	auto g5 = TinyVGGraphic::createFromFilename("everything.tvg");
	auto g6 = TinyVGGraphic::createFromFilename("logo.tvg");
	auto g7 = TinyVGGraphic::createFromFilename("app-icon.tvg");

	// create some windows to hold the graphics
	//window(0, 0, g1->width(), g1->height())->addChild(g1);
	window(0, 0, g2->width(), g2->height())->addChild(g2);
	//window(0, 0, g3->width(), g3->height())->addChild(g3);
	//window(0, 0, g4->width(), g4->height())->addChild(g4);
	//window(0, 0, g5->width(), g5->height())->addChild(g5);
	window(0, 0, 400, 400)->addChild(g6);
	//window(0, 0, g6->width(), g6->height())->addChild(g6);
	//window(0, 0, g7->width(), g7->height())->addChild(g7);
}
