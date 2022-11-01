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
	createCanvas(1024, 768);

	std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	windowLayout(layout);

	gAppSurface->setDpiUnits(systemDpi, 96);

	// create some graphics from tiny vg files
	auto g1 = TinyVGGraphic::createFromFilename("comic.tvg");
	auto g2 = TinyVGGraphic::createFromFilename("tiger.tvg");
	auto g3 = TinyVGGraphic::createFromFilename("shield.tvg");
	auto g4 = TinyVGGraphic::createFromFilename("chart.tvg");
	auto g5 = TinyVGGraphic::createFromFilename("everything.tvg");
	auto g6 = TinyVGGraphic::createFromFilename("logo.tvg");
	auto g7 = TinyVGGraphic::createFromFilename("app-icon.tvg");

	// create some windows to hold the graphics
	window(0, 0, g1->frameWidth(), g1->frameHeight())->addChild(g1);
	window(0, 0, g2->frameWidth(), g2->frameHeight())->addChild(g2);
	window(0, 0, g3->frameWidth(), g3->frameHeight())->addChild(g3);
	window(0, 0, g4->frameWidth(), g4->frameHeight())->addChild(g4);
	window(0, 0, g5->frameWidth(), g5->frameHeight())->addChild(g5);
	window(0, 0, g6->frameWidth(), g6->frameHeight())->addChild(g6);
	window(0, 0, g7->frameWidth(), g7->frameHeight())->addChild(g7);
}
