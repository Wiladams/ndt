#include "p5.hpp"
#include "MonthTile.hpp"
#include "DayTile.hpp"

#include <cstdio>
#include <vector>

using namespace p5;



void keyReleased(const KeyEvent &e)
{
	if (e.keyCode == VK_ESCAPE) {
		halt();
	}
}

void drawYearOfMonths(IGraphics *ctx)
{
	static const int numColumns = 3;
	static const int numRows = 4;
	static const int edgeMargin = 4;
	static const int lineGap = 8;

	std::vector<CalendarMonthTile> months;

	auto cellSize = CalendarMonthTile::getClassPreferredSize();

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

	
	for (size_t i = 0; i < months.size(); i++) {
		months[i].draw(ctx);
	}
}

void drawDayTiles(IGraphics* ctx)
{
	DayTile dt;
	
	dt.setDate(2020, 5, 14);
	
	ctx->push();
	ctx->translate(500, 72);

	dt.draw(ctx);
	ctx->pop();
}

void draw()
{
	if (isLayered()) {
		clear();
		//background(255, 10);
	} else
		background(0xc0);

	push();
	scale(.618, 0.618);
	drawYearOfMonths(gAppSurface);
	pop();


	drawDayTiles(gAppSurface);
}




void setup()
{

	//createCanvas(cellSize.w*numColumns+(edgeMargin *numColumns-1), cellSize.h*numRows+(edgeMargin *numRows-1));
	createCanvas(1024, 768);

	//layered();
	//setWindowPosition(100, 48);
	noLoop();
}

