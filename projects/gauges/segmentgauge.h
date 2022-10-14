#pragma once

#include "graphic.hpp"
#include "colorsampler.h"

struct SegmentTick : public IDrawable
{
	float fRadius = 0.0;
	float fStartAngle = 0.0;
	float fSweep = 0.0;

	float fWeight = 20;

	Pixel fIdleColor{};
	Pixel fProgressColor{};


	SegmentTick(float radius, float startAngle, float sweep)
	{
		fRadius = radius;
		fStartAngle = startAngle;
		fSweep = sweep;
	}

	void draw(IGraphics& ctx)
	{
		ctx.push();

		//ctx.rotate(fStartAngle);

		ctx.noFill();
		ctx.stroke(fProgressColor);
		ctx.strokeWeight(fWeight);
		ctx.arc(0, 0, fRadius, fStartAngle, fSweep);

		ctx.pop();
	}
};


struct SegmentGauge : public Graphic
{
	static void getPreferredSize(int& x, int& y) { x = 300; y = 300; }

	float fCx = 0;
	float fCy = 0;
	float fRadius = 0;

	int fNumTicks = 20;
	float fStartAngle = 0;
	float fSweep = 0;
	float fThickness = 20;
	Pixel fStartColor{};
	Pixel fEndColor{};

	float fProgress = 0.0;
	std::vector<SegmentTick> fTicks;


	SegmentGauge(const BLRect &f, float thickness)
		:Graphic(f.x, f.y, f.w, f.h)
	{
		fThickness = thickness;
		fCx = float(f.w / 2.0f);
		fCy = float(f.h/2.0f);

		fRadius = (fCx > fCy) ? fCy/2.0f : fCx/2.0f;

		float sweepDegrees = 200;
		float startDegrees = 90 + ((360 - sweepDegrees) / 2);

		fSweep = maths::radians(sweepDegrees);
		fStartAngle = maths::radians(startDegrees);

		fStartColor = Pixel(255, 125, 0);
		fEndColor = Pixel(0, 255, 0);

		createTicks();
	}

	/*
	void setSegmentThickness(float thickness)
	{
		for (auto& tick : fTicks)
		{
			tick.fWeight = thickness;
		}
	}
	*/

	// We only need to create the tick marks once.
 // Each tick is reponsible for its own drawing
 // but we must tell it what angle it's at, the
 // radius
	void createTicks()
	{
		float segSweep = (fSweep / fNumTicks);
		float tickSweep = (fSweep / fNumTicks) * 0.9f;

		for (int i = 0; i < fNumTicks; i++)
		{
			float startAt = fStartAngle + (i*segSweep);

			SegmentTick tick(fRadius, startAt, tickSweep);
			tick.fWeight = fThickness;
			auto c = ndt::lerpColor(fStartColor, fEndColor, (float)i / fNumTicks);

			tick.fProgressColor = c;
			tick.fWeight = fThickness;

			fTicks.push_back(tick);
		}
	}

	void setProgress(float prog)
	{
		fProgress = maths::clamp((float)prog, 0.0, 1.0);
	}

	void drawBackground(IGraphics & ctx) override
	{
		ctx.stroke(0xff, 0, 0);
		ctx.noFill();
		ctx.rect(0, 0, fFrame.w, fFrame.h);
	}

	void drawForeground(IGraphics &ctx)
	{
		ctx.push();

		ctx.translate(fCx, fCy);
		ctx.rotate(maths::radians(0.0));

		size_t howMany = maths::floor((fProgress * (float)fNumTicks));
		//printf("%3.2f  how many: %zd\n", fProgress, howMany);

		for (size_t i = 0; i < howMany; i++)
		{
			fTicks[i].draw(ctx);
		}


		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx.noStroke();
		ctx.fill(Pixel(0, 220, 150));
		ctx.textSize(36);
		ctx.text("FUEL", 0, 0);

		ctx.pop();
	}
};