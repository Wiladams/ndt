#include "p5.hpp"
#include "MonthTile.hpp"
#include "DayTile.hpp"
#include "yearofmonths.h"
#include "slidingmonth.h"

#include <cstdio>
#include <vector>

using namespace p5;


void draw()
{
	if (isLayered())
		// Start with a clear screen
		clear();
	else
		background(0xcd);
}


void setup()
{	
	//createCanvas(1280, 1024);
	fullscreen();

	std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	windowLayout(layout);
	
	// Setup day tile
	auto ps = DayTile::preferredSize();
	auto dtWindow = window(0, 0, ps.x, ps.y);
	auto dt = std::make_shared<DayTile>();
	//dt->setDate(2022, 9, 5);
	dtWindow->addChild(dt);
	

	// Setup year of months window
	auto yom = std::make_shared<YearOfMonths>();
	auto & fr = yom->frame();
	auto yomWin = window(0, 0, fr.w, fr.h);
	yomWin->addChild(yom);
	
	
	// Setup perpetual calendar
	auto pc = std::make_shared<SlidingMonth>(2022, 9, 240, 240);
	auto & pcfr = pc->frame();
	auto pcWin = window(0, 0, pcfr.w, pcfr.h);
	pcWin->addChild(pc);
	
}


void keyReleased(const KeyboardEvent& e)
{
	// quit the whole app with Esc key
	if (e.keyCode == VK_ESCAPE) {
		halt();
	}
}
