#pragma once

#include "p5.hpp"

struct Particle
{
	double fMass;
	p5::P5Vector fLocation;
	p5::P5Vector fVelocity;

	Particle(p5::P5Vector location, p5::P5Vector velocity, double mass)
		:fLocation(location),
		fVelocity(velocity),
		fMass(mass)
	{}

	virtual void draw(IGraphics& ctx)
	{
		ctx.fill(220, 127);
		ctx.noStroke();

		// Create a raindrop
		BLPath path;
		path.moveTo(fLocation.x, fLocation.y - 5);
		path.cubicTo(fLocation.x + 3, fLocation.y - 2, fLocation.x + 1, fLocation.y - 2, fLocation.x+3, fLocation.y);
		path.cubicTo(fLocation.x + 3, fLocation.y + 5, fLocation.x - 3, fLocation.y + 5, fLocation.x - 3, fLocation.y);
		path.close();
		ctx.path(path);

	}

	virtual void update()
	{
		fLocation.add(fVelocity);
	}
};