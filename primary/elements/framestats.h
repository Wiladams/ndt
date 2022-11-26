#pragma once

#include "apphost.h"
#include "graphic.hpp"
#include "secondtime.h"

class FrameStats : public GraphicElement
{
	SecondTime fTime;

public:
	FrameStats()
		:GraphicElement(0,0,200, 140)
	{}

	void drawBackground(IGraphics & ctx)override
	{
		ctx.push();
		ctx.fill(160,220);
		ctx.noStroke();

		auto fsz = maths::size(frame());
		ctx.rect(0, 0, fsz.x, fsz.y);
		ctx.pop();
	}

	void drawSelf(IGraphics & ctx) override
	{
		fTime.setSeconds(seconds());

		ctx.push();

		ctx.noStroke();
		ctx.fill(0, 220, 220);

		ctx.textSize(18);
		ctx.textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);

		char buff[256];
		sprintf_s(buff, "Seconds: %3.2f", fTime.fBaseTime);
		ctx.text(buff, 10, 18);

		sprintf_s(buff, "%3zd : %02zd : %02zd.%2zd", fTime.fHours, fTime.fMinutes, fTime.fSeconds, fTime.fTenths);
		ctx.text(buff, 10, 44);
		
		// Frame rate
		sprintf_s(buff, "frame count: %zd", frameCount());
		ctx.text(buff, 10, 68);

		sprintf_s(buff, "expected fps: %f", getFrameRate());
		ctx.text(buff, 10, 92);
		
		sprintf_s(buff, "actual fps: %3.2f", frameCount() / seconds());
		ctx.text(buff, 10, 116);

		
		ctx.pop();
	}
};