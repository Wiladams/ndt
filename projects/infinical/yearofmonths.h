#pragma once

#include "CalendarMonth.hpp"


#include <memory>

class YearOfMonths : public Graphic
{
	static const int numColumns = 3;
	static const int numRows = 4;
	static const int edgeMargin = 4;
	static const int lineGap = 8;
	int fBaseYear;

public:
	static BLSizeI getPreferredSize() 
	{ 
		auto cellSize = CalendarMonth::getClassPreferredSize();

		return { 
			int((double)cellSize.w * 3 + (edgeMargin * 4)),
			int((double)cellSize.h * 4 + (edgeMargin * 2) + (lineGap * 3)) 
		};
	}


	YearOfMonths(int baseYear)
		:fBaseYear(baseYear)
	{
		auto cellSize = CalendarMonth::getClassPreferredSize();

		int xoffset = edgeMargin;
		int yoffset = edgeMargin;

		// Setup text label of the year number
		//yoffset += cellSize.h + lineGap;
		yoffset += lineGap*3;


		// Setup 12 monthly tiles for this calendar
		for (int month = 1; month <= 12; month++) {
			addChild(std::make_shared<CalendarMonth>(baseYear, month, xoffset, yoffset));

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

		setFrame({ 0,0,(double)cellSize.w * 3 + (edgeMargin * 4), (double)cellSize.h * 4 + (edgeMargin * 2) + (lineGap * 3) });
	}

	virtual void drawForeground(std::shared_ptr<IGraphics> ctx)
	{
		// draw the year
		auto f = getFrame();
		char yearText[64];
		sprintf_s(yearText, "%d", fBaseYear);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx->textSize(18);
		ctx->fill(0);
		ctx->noStroke();
		ctx->text(yearText, f.w / 2,18);

		//ctx->flush();
	}
};
