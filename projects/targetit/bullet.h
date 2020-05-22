#pragma once

#include "p5.hpp"
#include "geometry.h"
#include "graphic.hpp"

class Bullet : public virtual IDrawable
{
	Vec2f fOrigin;

	float fDuration;
	double fStartTime;
	double fEndingTime;
	bool fFinished;

public:
	Vec2f fDestination;

	//
	// Default to a quarter second of animation
	//
	Bullet(const Vec2f& origin, const Vec2f& destination, float duration=0.25f)
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

	
	void draw(IGraphics* ctx)
	{
		if (fFinished)
			return;

		// Figure out location based on current time in animation
		double u = map(p5::seconds(), fStartTime, fEndingTime, 0, 1, true);
		double cx = map(u, 0, 1, fOrigin.x, fDestination.x);
		double cy = map(u, 0, 1, fOrigin.y, fDestination.y);

		ctx->stroke(0);
		ctx->fill(255);
		ctx->circle(cx, cy, 10);
		
		if (u >= 1.0)
			fFinished = true;
	}
};