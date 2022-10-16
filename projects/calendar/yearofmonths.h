#pragma once

#include "MonthTile.hpp"
#include <memory>

class YearOfMonths : public Graphic
{
	static const int numColumns = 3;
	static const int numRows = 4;
	static const int edgeMargin = 4;
	static const int lineGap = 8;

public:
	YearOfMonths()
	{
		auto cellSize = CalendarMonthTile::preferredSize();

		int xoffset = edgeMargin;
		int yoffset = edgeMargin;

		// Setup 12 monthly tiles for this calendar
		for (int month = 1; month <= 12; month++) {
			addChild(std::make_shared<CalendarMonthTile>(2022, month, xoffset, yoffset));

			if (month % 3 > 0) {
				// continue advancing
				xoffset += cellSize.x + edgeMargin;
			}
			else {
				// end of row, move to next row
				xoffset = edgeMargin;
				yoffset += cellSize.y + lineGap;
			}
		}

		setFrame({ 0,0,(double)cellSize.x * 3 + (edgeMargin * 4), (double)cellSize.y * 4 + (edgeMargin * 2) + (lineGap * 3) });
	}
};
