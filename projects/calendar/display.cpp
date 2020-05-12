#include "p5.hpp"
#include "MonthTile.hpp"

#include <cstdio>
#include <vector>

using namespace p5;


std::vector<CalendarMonthTile> months;

void keyReleased(const KeyEvent &e)
{
	if (e.keyCode == VK_ESCAPE) {
		halt();
	}
}

void draw()
{
	if (gIsLayered) {
		clear();
		//background(255, 10);
	}
	else
		background(0xff);

	for (size_t i = 0; i < months.size();i++) {
		months[i].draw(*gAppSurface);
	}

}

void setup()
{
	auto cellSize = CalendarMonthTile::getPreferredSize();

	createCanvas(cellSize.w*4+(4*3), 800);
	//scale(0.75, 0.75);

	int edgeMargin = 4;
	int lineGap = 4;

	int xoffset = edgeMargin;
	int yoffset = lineGap;

	// Setup 12 monthly tiles for this calendar
	for (int month = 1; month <= 12; month++) {
		months.push_back(CalendarMonthTile(2020, month, xoffset, yoffset));
		if (month % 4 > 0) {
			// continue advancing
			xoffset += cellSize.w + edgeMargin;
		}
		else {
			// end of row, move to next row
			xoffset = edgeMargin;
			yoffset += cellSize.h + lineGap;
		}
	}

	layered();
	noLoop();
}

