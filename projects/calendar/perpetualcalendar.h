#pragma once

#include "graphic.hpp"
#include "calendar.hpp"

class PerpetualCalendar : public Graphic
{
	Calendar::USACalendar fCalendar;
	int fMonth;
	int fDayCode;	// 0 - x

	int fDayCells[6][13]= {
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
	PerpetualCalendar(int year, int month, double w, double h)
		: Graphic(0,0,w,h),
		fCalendar(year)
	{
		cellWidth = w / 13;
		cellHeight = h / 6;

		setMonth(month);
	}

	// set month [1-12]
	void setMonth(int month)
	{
		fMonth = month;
		fDayCode = fCalendar.dayCodeForMonth(month);

		std::cout << "day Code: " << fDayCode << std::endl;
	}

	void drawBackground(IGraphics* ctx)
	{
		// draw a black background rectangle to start

		ctx->fill(0);
		ctx->noStroke();
		ctx->rect(0, 0, fFrame.w, fFrame.h);
	}

	void drawSelf(IGraphics* ctx)
	{
		// draw numeric squares
		ctx->textSize(12);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
		ctx->noStroke();

		for (int row = 0; row < 6; row++) {
			for (int col = 0; col < 13; col++) {
				auto cellX = (col * cellWidth);
				auto cellY = (row * cellHeight);

				auto toffsetx = cellGap + ((cellWidth/2) + (col* cellWidth));
				auto toffsety = cellGap + ((cellHeight / 2) + (row * cellHeight));

				if (fDayCells[row][col] > 0) {
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
	}

	void drawForeground(IGraphics* ctx)
	{
		double dayOffset = (6 - fDayCode) * cellWidth;

		ctx->push();
		ctx->noFill();
		ctx->stroke(250,0,0);
		ctx->rect(dayOffset, 0, cellWidth * 7, cellHeight * 7);
		ctx->pop();
	}
};
