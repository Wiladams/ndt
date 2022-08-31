#pragma once

#include "apphost.h"

#include "calendar.hpp"
#include "graphic.hpp"

//
// This MonthTile can show a monthly calendar
//

class CalendarMonth : public Graphic
{
	static const int colWidth = 28;
	static const int colHeight = 28;

	Calendar::USACalendar fCalendar;
	int fMonth;
	int fOriginX;
	int fOriginY;
	float fLevelOfDetail;
	BLRectI fBounds;
	BLRectI fFrame;
	Pixel fBackgroundColor;

	char  fTitle[64];

public:
	static BLSizeI getClassPreferredSize() { return { 200,200 }; }


	CalendarMonth(int year, int month, int x = 0, int y = 0, const Pixel& bkg = { 0xf0, 0xf0, 0xf0, 127 })
		: Graphic(0,0,200,200),
		fCalendar(year),
		fMonth(month),
		fBackgroundColor(bkg)
	{
		auto size = getClassPreferredSize();
		fBounds = { 0,0,size.w,size.h };
		fFrame = { x, y, size.w, size.h };
		//sprintf_s(fTitle, 64, "%s  %d", Calendar::MonthsLong[fMonth].c_str(), year);
		sprintf_s(fTitle, 64, "%s", Calendar::MonthsLong[fMonth].c_str());
	}



	void drawDayNumbers(std::shared_ptr<IGraphics> ctx)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		char buff[8];
		int dayCode = fCalendar.dayCodeForMonth(fMonth);
		int days = fCalendar.getDaysInMonth(fMonth);


		int xoffset = 4 + (dayCode)*CalendarMonth::colWidth;
		int yoffset = 48;

		ctx->fill(0);
		ctx->textSize(12);
		ctx->textAlign(ALIGNMENT::BASELINE, ALIGNMENT::RIGHT);

		for (int day = 1; day <= days; day++) {
			sprintf_s(buff, 8, "%2d", day);
		
			if ((sysTime.wDay == day) && (sysTime.wMonth == fMonth))
				ctx->fill(255,0,0);
			else
				ctx->fill(0);

			ctx->text(buff, xoffset, yoffset);

			if ((day + dayCode) % 7 > 0) {
				xoffset += CalendarMonth::colWidth;
			}
			else {
				// Start sunday on next row
				xoffset = 4;
				yoffset += CalendarMonth::colHeight;
			}
		}
	}

	void drawDayHeadings(std::shared_ptr<IGraphics> ctx)
	{
		int yoffset = 16;
		int xoffset = 4;

		ctx->noStroke();
		ctx->fill(0x5e);
		ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);

		for (int i = 0; i < 7; i++) {
			ctx->text(Calendar::WeekDays2Letter[i].c_str(), xoffset, yoffset);
			xoffset += CalendarMonth::colWidth;
		}
	}

	void draw(std::shared_ptr<IGraphics> ctx)
	{
		//printf("MonthDay: (%d) %d\n", fMonth, fCalendar.dayCodeForMonth(fMonth));
		ctx->push();
		ctx->translate(fFrame.x, fFrame.y);

		// Draw Calendar rectangle
		ctx->noStroke();


		ctx->fill(fBackgroundColor);
		ctx->rect(0, 0, fBounds.w, fBounds.h);

		// Draw shadow rect for month title

		ctx->fill(0xa0);
		ctx->rect(0, 0, fBounds.w, 16);

		// Draw the month long name, and year
		ctx->fill(0);
		ctx->textSize(12);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
		ctx->text(fTitle, fBounds.w / 2, 6);

		drawDayHeadings(ctx);
		drawDayNumbers(ctx);

		ctx->pop();
	}
};

