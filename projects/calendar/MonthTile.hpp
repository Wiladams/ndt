#pragma once

#include "graphic.hpp"
#include "elements/calendar.hpp"

using namespace p5;

//
// This MonthTile can show a monthly calendar at varying levels of detail
// The level of detail is on a scale from 
// 0.0 - least detail, to 
// 1.0 - most detail
// The amount of detail just changes the rendering, not the size
// it is up to the user to display in the appropriate size

class CalendarMonthTile : public GraphicElement
{
	static const int colWidth = 28;
	static const int colHeight = 28;

	Calendar::USACalendar fCalendar;
	int fMonth;
	float fLevelOfDetail;
	Pixel fBackgroundColor;

	char  fTitle[64];

public:
	static maths::vec2f preferredSize() { return { 200,200 }; }


	CalendarMonthTile(int year, int month, float x = 0, float y = 0, const Pixel& bkg = { 0xf0, 0xf0, 0xf0, 127 })
		: GraphicElement({ x,y,CalendarMonthTile::preferredSize().x,CalendarMonthTile::preferredSize().y})
		,fCalendar(year)
		,fMonth(month)
		,fBackgroundColor(bkg)
	{
		auto size = preferredSize();
		setBounds({ 0,0,size.x,size.y });
		//setFrame({ x, y, size.x, size.y });
		sprintf_s(fTitle, 64, "%s  %d", Calendar::MonthsLong[fMonth].c_str(), year);
	}

	void drawDayNumbers(IGraphics & ctx)
	{
		SYSTEMTIME sysTime;
		GetLocalTime(&sysTime);

		char buff[8];
		int dayCode = fCalendar.dayCodeForMonth(fMonth);
		int days = fCalendar.getDaysInMonth(fMonth);


		int xoffset = 4 + (dayCode)*CalendarMonthTile::colWidth;
		int yoffset = 48;

		ctx.fill(0);
		ctx.textSize(12);
		ctx.textAlign(ALIGNMENT::BASELINE, ALIGNMENT::RIGHT);

		for (int day = 1; day <= days; day++) {
			sprintf_s(buff, 8, "%2d", day);
		
			if ((sysTime.wDay == day) && (sysTime.wMonth == fMonth))
				ctx.fill(255);
			else
				ctx.fill(0);

			ctx.text(buff, xoffset, yoffset);

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

	void drawDayHeadings(IGraphics & ctx)
	{
		int yoffset = 16;
		int xoffset = 4;

		ctx.noStroke();
		ctx.fill(0x5e);
		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);

		for (int i = 0; i < 7; i++) {
			ctx.text(Calendar::WeekDays2Letter[i].c_str(), xoffset, yoffset);
			xoffset += CalendarMonthTile::colWidth;
		}
	}

	void draw(IGraphics & ctx) override
	{
		//printf("MonthDay: (%d) %d\n", fMonth, fCalendar.dayCodeForMonth(fMonth));
		ctx.push();
		ctx.translate(frame().min.x, frame().min.y);

		// Draw Calendar rectangle
		ctx.noStroke();


		ctx.fill(fBackgroundColor);
		ctx.rect(0, 0, boundsWidth(), boundsHeight());

		// Draw shadow rect for month title

		ctx.fill(0xa0);
		ctx.rect(0, 0, boundsWidth(), 16);

		// Draw the month long name, and year
		ctx.fill(0);
		ctx.textSize(12);
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
		ctx.textFont("Consolas");
		ctx.text(fTitle, boundsWidth() / 2, 6);

		drawDayHeadings(ctx);
		drawDayNumbers(ctx);

		ctx.pop();
	}
};

