#pragma once

#include "p5.hpp"
#include "calendar.hpp"
#include "graphic.hpp"

class DayTile : public Graphic
{
	static const int hourLabelGap = 36;
	static const int hourLineHeight = 48;

	Calendar::USACalendar fCalendar;
	double fJulianDayNumber;

	int fMonth;
	int fYear;
	int fDay;
	int fDayOfWeek;	// which day of the week
	
	Pixel fBackgroundColor{ 0xf0, 0xf0, 0xf0, 127 };
	float fLevelOfDetail;

public:
	// Return the size we prefer to be
	static BLSize getPreferredSize() { return { 300,400 }; }

public:
	DayTile()
		: Graphic({ 0,0,300,400 })
	{
		// Set Date to today's date
		SYSTEMTIME sysTime;
		::GetLocalTime(&sysTime);
		setDate(sysTime.wYear, sysTime.wMonth, sysTime.wDay);
	}

	DayTile(int year, int month, int day)
		: Graphic({0,0,300,400})
	{
		setDate(year, month, day);
	}

	bool setDate(int year, int month, int day)
	{
		fYear = year;
		fMonth = month;
		fDay = day;
		fJulianDayNumber = Calendar::julianDayNumber(year, month, day);
		fDayOfWeek = (int)(fJulianDayNumber + 1) % 7;

		return true;
	}

	void moveTo(double x, double y)
	{

	}

	void drawHourGrid(std::shared_ptr<IGraphics> ctx)
	{
		auto bounds = getBounds();
		char buff[32];
		int buffLen = 32;
		Pixel hourColor(140, 213, 240);
		Pixel halfHourColor(155, 223, 250);

		// Draw background for hourly text
//ctx->fill(230, 230, 220);	// color used when hours not available
		ctx->fill(250);
		//ctx->rect(hourLabelGap, 0, bounds.w - hourLabelGap, bounds.h - 72);
		ctx->rect(0, 0, bounds.w, bounds.h - 72);

		// Draw a line top to bottom separating the hour 
		// numbers from the event grid to the right
		ctx->strokeWeight(0.5);
		ctx->stroke(hourColor);
		ctx->line(hourLabelGap, 0, hourLabelGap, 24 * hourLineHeight);


		// Draw the hourly separators
		ctx->strokeWeight(0.5);
		ctx->textAlign(ALIGNMENT::RIGHT, ALIGNMENT::BOTTOM);
		for (int hour = 0; hour < 23; hour++) {
			ctx->stroke(hourColor);
			// draw full hour line
			//ctx->line(0, (hour * hourLineHeight)+0.5, bounds.w, (hour * hourLineHeight)+0.5);
			ctx->line(0, (hour * hourLineHeight), bounds.w, (hour * hourLineHeight));


			// draw half hour line
			// should be dashed line
			ctx->stroke(halfHourColor);
			ctx->line(hourLabelGap, (hour * hourLineHeight) + hourLineHeight / 2, bounds.w, (hour * hourLineHeight) + hourLineHeight / 2);

			// 12 hour clock numbers
			ctx->noStroke();
			ctx->fill(20);
			int displayHour = hour % 12;
			sprintf_s(buff, buffLen, "%d", (displayHour == 0) ? 12 : displayHour);
			ctx->text(buff, hourLabelGap / 2, (hour * hourLineHeight) + hourLineHeight / 2);
		}
	}

	/*
// draw month day number, year
char buff[32];
sprintf_s(buff, 32, "%s %d, %d",
	Calendar::MonthsLong[fMonth],fDay,fYear);

ctx->noStroke();
ctx->fill(82);
ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
ctx->textSize(24);
ctx->text(buff, 32, 24);
*/

	void drawMonthDay(std::shared_ptr<IGraphics> ctx)
	{
		// Draw nice blue background rectangle for black text
		auto bounds = getBounds();
		ctx->noStroke();
		ctx->fill(0, 170, 220);
		ctx->rect(hourLabelGap, 0, bounds.w- hourLabelGap, 30);

		// Weekday long name
		char buff[32];
		sprintf_s(buff, 32, "%s", Calendar::WeekDaysLong[fDayOfWeek].c_str());
		ctx->noStroke();
		ctx->fill(82);
		ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::CENTER);
		ctx->textSize(18);
		ctx->text(buff, hourLabelGap, 10);

		// Put the number of the day in the section between day name
		// and the list of hours
		sprintf_s(buff, 32, "%d", fDay);
		ctx->text(buff, hourLabelGap, 36);

		// Clip for the hourly grid
		ctx->push();
		ctx->clip(0, 72, bounds.w, bounds.h - 72);
		ctx->translate(0, 72);

		drawHourGrid(ctx);

		ctx->noClip();
		ctx->pop();
	}

	void drawBackground(std::shared_ptr<IGraphics> ctx)
	{
		// draw background
		ctx->noStroke();
		ctx->fill(fBackgroundColor);
		auto bounds = getBounds();
		ctx->rect(0, 0, bounds.w, bounds.h);
	}

	void draw(std::shared_ptr<IGraphics> ctx) 
	{
		drawBackground(ctx);
		drawMonthDay(gAppSurface);
	}
};