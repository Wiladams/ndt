#pragma once

#include "p5.hpp"
#include "calendar.hpp"
#include "graphic.hpp"

class DayTile : Graphic
{
	Calendar::USACalendar fCalendar;
	double fJulianDayNumber;
	float fLevelOfDetail;
	Color fBackgroundColor;
	SYSTEMTIME fSysTime;

public:
	DayTile(double julianDay, const Color& bkg = { 0xf0, 0xf0, 0xf0, 127 })
		: Graphic({0,0,200,300}),
		fJulianDayNumber(julianDay),
		fBackgroundColor(bkg)
	{
		::GetLocalTime(&fSysTime);
	}

	void drawMonthDay(IGraphics* ctx)
	{
		// draw month day number, year
		char buff[32];
		sprintf_s(buff, 32, "%s %d, %d", 
			Calendar::MonthsLong[fSysTime.wMonth],
			fSysTime.wDay,
			fSysTime.wYear);

		ctx->noStroke();
		ctx->fill(82);
		ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
		ctx->textSize(24);
		ctx->text(buff, 32, 24);
	}

	void drawBackground(IGraphics* ctx)
	{
		// draw background
		ctx->noStroke();
		ctx->fill(fBackgroundColor);
		auto bounds = getBounds();
		ctx->rect(0, 0, bounds.w, bounds.h);
	}

	void draw(IGraphics* ctx) 
	{
		drawBackground(ctx);
		drawMonthDay(gAppSurface);
	}
};