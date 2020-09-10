#pragma once

#include "graphic.hpp"
#include "calendar.hpp"

/*
	Draw a view of a month with the previous and next month's
	numbers filling in the grid where available.
*/
class SlidingMonth : public Graphic
{
	Calendar::USACalendar fPreviousCalendar;
	Calendar::USACalendar fCalendar;
	Calendar::USACalendar fNextCalendar;

	int fMonth;
	int fNextMonth;
	int fPreviousMonth;




	int fDayCells[6][13] = {
			{ 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7},
			{ 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14},
			{ 9,10,11,12,13,14,15,16,17,18,19,20,21},
			{16,17,18,19,20,21,22,23,24,25,26,27,28},
			{23,24,25,26,27,28,29,30,31, 0, 0, 0, 0},
			{30,31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};

	double cellWidth;
	double cellHeight;
	double cellGap = 0.5;

public:
	SlidingMonth(int year, int month, double w, double h)
		: Graphic(0, 0, w, h),
		fCalendar(year),
		fPreviousCalendar(year),
		fNextCalendar(year)
	{
		cellWidth = w / 7;
		cellHeight = h / 5;

		setDate(year, month);
	}

	// set month [1-12]
	void setDate(int year, int month)
	{
		fMonth = month;
		fNextMonth = month + 1;
		fPreviousMonth = month - 1;

		if (fMonth == 1) {
			fPreviousCalendar.setYear(year - 1);
			fPreviousMonth = 12;
		}

		if (fMonth == 12) {
			fNextCalendar.setYear(year + 1);
			fNextMonth = 1;
		}

	}

	void drawBackground(IGraphics* ctx)
	{
		// draw a black background rectangle to start
		ctx->fill(0);
		ctx->noStroke();
		ctx->rect(0, 0, fFrame.w, fFrame.h);
	}

	void drawDate(IGraphics* ctx, int row, int col, int date, const Pixel &c)
	{
		// draw the date
		auto cellX = (col * cellWidth);
		auto cellY = (row * cellHeight);

		auto toffsetx = cellGap + ((cellWidth / 2) + (col * cellWidth));
		auto toffsety = cellGap + ((cellHeight / 2) + (row * cellHeight));

		// Put number in cell
		ctx->fill(0xFF, 120);
		ctx->rect(cellX, cellY, cellWidth - (cellGap * 2), cellHeight - (cellGap * 2));
		ctx->fill(c);
		ctx->text(std::to_string(date).c_str(), toffsetx, toffsety);
	}


	void drawSelf(IGraphics* ctx)
	{
		// draw numeric squares
		ctx->textSize(12);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
		ctx->noStroke();

		int monthDayNumber = fCalendar.dayCodeForMonth(fMonth);
		int daysInMonth = fCalendar.getDaysInMonth(fMonth);
		int previousDaysInMonth = fPreviousCalendar.getDaysInMonth(fPreviousMonth);
		int nextDaysInMonth = fNextCalendar.getDaysInMonth(fNextMonth);


		int row = 0;
		int col = 0;

		// draw pre-month
		for (col = 0; col < monthDayNumber;)
		{
			auto adate = previousDaysInMonth - monthDayNumber + col + 1;
			drawDate(ctx, row, col, adate, color(255));

			col = col + 1;
			if (col >= 7)
			{
				col = 0;
				row = row + 1;
			}
		}

		// draw core date numbers
		for (int date = 1; date <= daysInMonth; date++)
		{
			drawDate(ctx, row, col, date, color(0xff, 0xff, 0xc0));

			// increment row and column
			col = col + 1;
			if (col >= 7)
			{
				col = 0;
				row = row + 1;
			}
		}


		// draw post-month
		if (col > 0) {
			int date = 1;
			while (col < 7) {
				drawDate(ctx, row, col, date, color(255));
				col = col + 1;
				date = date + 1;
			}
		}
	}

};
