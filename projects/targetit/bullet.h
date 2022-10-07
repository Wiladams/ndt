#pragma once

#include "p5.hpp"

#include "graphic.hpp"

using namespace p5;

struct Bullet : public IDrawable
{
	maths::vec2f fOrigin{};
	maths::vec2f fDestination{};

	double fDuration=0;
	double fStartTime=0;
	double fEndingTime=0;
	bool fFinished=false;

	//
	// Default to a quarter second of animation
	//
	Bullet(const maths::vec2f& origin, const maths::vec2f& destination, double duration=0.25f)
		: fOrigin(origin),
		fDestination(destination),
		fDuration(duration),
		fFinished(true)
	{
	}

	bool isFinished() { return p5::seconds() > fEndingTime; }

	// No animation will occur until
	// play() is called.
	void play()
	{
		fStartTime = p5::seconds();
		fEndingTime = fStartTime + fDuration;
		fFinished = false;
	}

	
	void draw(IGraphics & ctx) override
	{
		if (fFinished)
			return;

		// Figure out location based on current time in animation
		float u = map(p5::seconds(), fStartTime, fEndingTime, 0, 1.0, true);
		float cx = map(u, 0, 1.0, fOrigin.x, fDestination.x);
		float cy = map(u, 0, 1.0, fOrigin.y, fDestination.y);

		ctx.stroke(0);
		ctx.fill(255);
		ctx.circle(cx, cy, 10);
		
		if (u >= 1.0f)
			fFinished = true;
	}
};