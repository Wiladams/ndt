#pragma once

#include "graphic.hpp"
#include "calendar.hpp"

class PerpetualCalendar : public Graphic
{
	Calendar::USACalendar fPreviousCalendar;
	Calendar::USACalendar fCalendar;
	Calendar::USACalendar fNextCalendar;

	int fMonth;
	int fNextMonth;
	int fPreviousMonth;




	int fDayCells[6][13]= {
			{ 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7},
			{ 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14},
			{ 9,10,11,12,13,14,15,16,17,18,19,20,21},
			{16,17,18,19,20,21,22,23,24,25,26,27,28},
			{23,24,25,26,27,28,29,30,31, 0, 0, 0, 0},
			{30,31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
	};

	float cellWidth;
	float cellHeight;
	float cellGap = 0.5;

public:
	PerpetualCalendar(int year, int month, float w, float h)
		: Graphic(0,0,w,h),
		fCalendar(year),
		fPreviousCalendar(year),
		fNextCalendar(year)
	{
		cellWidth = w / 13;
		cellHeight = h / 6;

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

	void drawBackground(std::shared_ptr<IGraphics> ctx)
	{
		// draw a black background rectangle to start
		ctx->fill(0);
		ctx->noStroke();
		ctx->rect(0, 0, fFrame.w, fFrame.h);
	}

	void drawSelf(std::shared_ptr<IGraphics> ctx)
	{
		// draw numeric squares
		ctx->textSize(12);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
		ctx->noStroke();

		int monthDayNumber = fCalendar.dayCodeForMonth(fMonth);
		int daysInMonth = fCalendar.getDaysInMonth(fMonth);
		int previousDaysInMonth = fPreviousCalendar.getDaysInMonth(fPreviousMonth);
		int nextDaysInMonth = fNextCalendar.getDaysInMonth(fNextMonth);

		for (int row = 0; row < 6; row++)
		{
			for (int col = 0; col < 13; col++)
			{
				int date = fDayCells[row][col];
				
				// deal with previous month
				if (date == 0) {
					date = previousDaysInMonth - monthDayNumber ;
				}

				// Draw the date
				auto cellX = (col * cellWidth);
				auto cellY = (row * cellHeight);

				auto toffsetx = cellGap + ((cellWidth / 2) + (col * cellWidth));
				auto toffsety = cellGap + ((cellHeight / 2) + (row * cellHeight));

				// Put number in cell
				ctx->fill(0xFF, 120);
				ctx->rect(cellX, cellY, cellWidth - (cellGap * 2), cellHeight - (cellGap * 2));
				ctx->fill(0xff, 0xff, 0xc0);
				ctx->text(std::to_string(date).c_str(), toffsetx, toffsety);

			}
		}

		/*
		for (int row = 0; row < 6; row++) 
		{
			for (int col = 0; col < 13; col++) 
			{
				auto cellX = (col * cellWidth);
				auto cellY = (row * cellHeight);

				auto toffsetx = cellGap + ((cellWidth/2) + (col* cellWidth));
				auto toffsety = cellGap + ((cellHeight / 2) + (row * cellHeight));

				auto cellDay = fDayCells[row][col];
				if ((cellDay > 0) && (cellDay <= daysInMonth)) {
					// Put number in cell
					ctx->fill(0xFF,120);
					ctx->rect(cellX, cellY, cellWidth- (cellGap * 2), cellHeight-(cellGap*2));
					ctx->fill(0xff, 0xff,0xc0);
					ctx->text(std::to_string(fDayCells[row][col]).c_str(), toffsetx, toffsety);
				} else {
					// For empty ones, make them gray
					ctx->fill(0xC0);
					ctx->rect(cellX, cellY, cellWidth- (cellGap * 2), cellHeight- (cellGap * 2));
				}
			}
		}
*/
	}

	void drawForeground(std::shared_ptr<IGraphics> ctx)
	{
		// Draw the rectangle that represents 
		// the current month
		double dayOffset = (6.0 - fCalendar.dayCodeForMonth(fMonth) * cellWidth);

		ctx->push();
		ctx->noFill();
		ctx->stroke(250,0,0);
		ctx->rect(dayOffset, 0, cellWidth * 7, cellHeight * 7);
		ctx->pop();
	}
};
