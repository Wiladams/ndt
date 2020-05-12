#pragma once

#include "p5.hpp"
#include "calendar.hpp"

using namespace p5;

class CalendarMonthTile {
	static const int colWidth = 28;
	static const int colHeight = 28;

	Calendar::USACalendar fCalendar;
	int fMonth;
	int fOriginX;
	int fOriginY;
	BLRectI fBounds;
	BLRectI fFrame;
	Color fBackgroundColor;

	char  fTitle[64];

public:
	static BLSizeI getPreferredSize() { return { 200,200 }; }

	CalendarMonthTile(int year, int month, int x = 0, int y = 0, const Color& bkg = { 0xf0, 0xf0, 0xf0, 127 })
		: fCalendar(year),
		fMonth(month),
		fBackgroundColor(bkg)
	{
		auto size = CalendarMonthTile::getPreferredSize();
		fBounds = { 0,0,size.w,size.h };
		fFrame = { x, y, size.w, size.h };
		sprintf_s(fTitle, 64, "%s  %d", Calendar::MonthsLong[fMonth].c_str(), year);
	}


	void drawDayNumbers(Surface& ctx)
	{
		char buff[8];
		int dayCode = fCalendar.dayCodeForMonth(fMonth);
		int days = fCalendar.getDaysInMonth(fMonth);


		int xoffset = 4 + (dayCode)*CalendarMonthTile::colWidth;
		int yoffset = 48;

		ctx.fill(color(0));
		ctx.textSize(12);
		ctx.textAlign(ALIGNMENT::BASELINE, ALIGNMENT::RIGHT);

		for (size_t day = 1; day <= days; day++) {
			sprintf_s(buff, 8, "%2d", day);
			text(buff, xoffset, yoffset);

			if ((day + dayCode) % 7 > 0) {
				xoffset += CalendarMonthTile::colWidth;
			}
			else {
				// Start sunday on next row
				xoffset = 4;
				yoffset += CalendarMonthTile::colHeight;
			}
		}
	}

	void drawDayHeadings(Surface& ctx)
	{
		int yoffset = 16;
		int xoffset = 4;

		ctx.noStroke();
		ctx.fill(color(0xbe));
		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);

		for (int i = 0; i < 7; i++) {
			text(Calendar::WeekDays2Letter[i].c_str(), xoffset, yoffset);
			xoffset += CalendarMonthTile::colWidth;
		}
	}

	void draw(Surface& ctx)
	{
		//printf("MonthDay: (%d) %d\n", fMonth, fCalendar.dayCodeForMonth(fMonth));

		ctx.push();
		ctx.translate(fFrame.x, fFrame.y);

		// Draw Calendar rectangle
		ctx.noStroke();


		ctx.fill(fBackgroundColor);
		rect(0, 0, fBounds.w, fBounds.h);

		// Draw shadow rect for month title

		ctx.fill(color(0xa0));
		rect(0, 0, fBounds.w, 16);

		// Draw the month long name, and year
		ctx.fill(color(0));
		ctx.textSize(12);
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
		ctx.text(fTitle, fBounds.w / 2, 6);

		drawDayHeadings(ctx);
		drawDayNumbers(ctx);

		ctx.pop();
	}
};

