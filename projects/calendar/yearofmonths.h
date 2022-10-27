#pragma once

#include "MonthTile.hpp"
#include <memory>

class YearOfMonths : public Graphic
{
	static const int numColumns = 3;
	static const int numRows = 4;
	static constexpr float edgeMargin = 4;
	static constexpr float lineGap = 8;

	int fBaseYear = 2022;

public:
	YearOfMonths(int baseYear = 2022)
		:fBaseYear(baseYear)
	{
		auto cellSize = CalendarMonthTile::preferredSize();

		float xoffset = edgeMargin;
		float yoffset = edgeMargin;

		// Setup 12 monthly tiles for this calendar
		for (int month = 1; month <= 12; month++) {
			auto mon = std::make_shared<CalendarMonthTile>(2022, month, xoffset, yoffset);
			addChild(mon);

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

		setFrame({ {0,0},{cellSize.x * 3 + (edgeMargin * 4), cellSize.y * 4 + (edgeMargin * 2) + (lineGap * 3)} });
		//setBounds(frame());
	}
};
