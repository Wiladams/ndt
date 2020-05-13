#include "p5.hpp"
#include "MonthTile.hpp"

#include <cstdio>
#include <vector>

using namespace p5;


std::vector<CalendarMonthTile> months;
static const int numColumns = 3;
static const int numRows = 4;
static const int edgeMargin = 4;
static const int lineGap = 8;


void keyReleased(const KeyEvent &e)
{
	if (e.keyCode == VK_ESCAPE) {
		halt();
	}
}

void draw()
{
	if (isLayered()) {
		clear();
		//background(255, 10);
	} else
		background(0xff);

	for (size_t i = 0; i < months.size();i++) {
		months[i].draw(gAppSurface);
	}

}



void setup()
{
	auto cellSize = CalendarMonthTile::getClassPreferredSize();

	createCanvas(cellSize.w*numColumns+(edgeMargin *numColumns-1), cellSize.h*numRows+(edgeMargin *numRows-1));

	int xoffset = edgeMargin;
	int yoffset = lineGap;

	// Setup 12 monthly tiles for this calendar
	for (int month = 1; month <= 12; month++) {
		months.push_back(CalendarMonthTile(2020, month, xoffset, yoffset));
		if (month % 3 > 0) {
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
	setWindowPosition(100, 48);
	noLoop();
}

