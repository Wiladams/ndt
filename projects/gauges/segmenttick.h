#pragma once

#include "graphic.hpp"
#include "colorsampler.h"

struct SegmentTick : public IDrawable
{
	double fRadius = 0.0;
	double fStartAngle = 0.0;
	double fSweep = 0.0;

	double fWeight = 20.0;

	Pixel fIdleColor{};
	Pixel fProgressColor{};


	SegmentTick(double radius, double startAngle, double sweep)
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

	double fCx = 0;
	double fCy = 0;
	double fRadius = 0;

	int fNumTicks = 20;
	double fStartAngle = 0;
	double fSweep = 0;
	double fThickness = 20;
	Pixel fStartColor{};
	Pixel fEndColor{};

	double fProgress = 0.0;
	std::vector<SegmentTick> fTicks;


	SegmentGauge(double radius, double cx, double cy)
		:Graphic(0,0,radius*2,radius*2)
	{
		fCx = cx;
		fCy = cy;

		fRadius = radius;

		double sweepDegrees = 200.0;
		double startDegrees = 90.0 + ((360 - sweepDegrees) / 2.0);

		fSweep = maths::Radians(sweepDegrees);
		fStartAngle = maths::Radians(startDegrees);

		fStartColor = Pixel(255, 125, 0);
		fEndColor = Pixel(0, 255, 0);

		createTicks();
	}

	void setSegmentThickness(double thickness)
	{
		for (auto& tick : fTicks)
		{
			tick.fWeight = thickness;
		}
	}


	// We only need to create the tick marks once.
 // Each tick is reponsible for its own drawing
 // but we must tell it what angle it's at, the
 // radius
	void createTicks()
	{
		double segSweep = (fSweep / fNumTicks);
		double tickSweep = (fSweep / fNumTicks) * 0.9;

		for (int i = 0; i < fNumTicks; i++)
		{
			double startAt = fStartAngle + (i*segSweep);

			SegmentTick tick(fRadius, startAt, tickSweep);
			tick.fWeight = fThickness;
			auto c = ndt::lerpColor(fStartColor, fEndColor, (double)i / fNumTicks);

			tick.fProgressColor = c;

			fTicks.push_back(tick);
		}
	}

	void setProgress(double prog)
	{
		fProgress = maths::Clamp(prog, 0.0, 1.0);
	}

	void drawForeground(IGraphics &ctx)
	{
		ctx.push();

		ctx.translate(fCx, fCy);
		ctx.rotate(maths::Radians(0.0));

		size_t howMany = maths::Floor((fProgress * (double)fNumTicks));
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