#pragma once
#include "p5.hpp"
#include "secondtime.h"


class FrameStats : public Graphic 
{
	SecondTime fTime;

public:
	FrameStats()
		:Graphic(0,0,200, 140)
	{

	}

	void drawBackground(std::shared_ptr<IGraphics> ctx)
	{
		ctx->push();
		ctx->fill(160);
		ctx->noStroke();

		ctx->rect(0, 0, fFrame.w, fFrame.h);
		ctx->pop();
	}

	void drawSelf(std::shared_ptr<IGraphics> ctx)
	{
		fTime.setSeconds(p5::seconds());

		ctx->push();

		ctx->noStroke();
		ctx->fill(0, 220, 220);

		ctx->textSize(18);
		ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);

		char buff[256];
		sprintf_s(buff, "Seconds: %3.2f", fTime.fBaseTime);
		ctx->text(buff, 10, 18);

		sprintf_s(buff, "%3zd : %02zd : %02zd.%2zd", fTime.fHours, fTime.fMinutes, fTime.fSeconds, fTime.fTenths);
		ctx->text(buff, 10, 44);
		
		// Frame rate
		sprintf_s(buff, "frame count: %d", p5::frameCount);
		ctx->text(buff, 10, 68);

		sprintf_s(buff, "expected fps: %d", p5::getFrameRate());
		ctx->text(buff, 10, 92);
		
		sprintf_s(buff, "actual fps: %3.2f", p5::frameCount / p5::seconds());
		ctx->text(buff, 10, 116);

		
		ctx->pop();
	}
};