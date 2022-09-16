#pragma once

#include "p5.hpp"

//using namespace p5;

struct Star {
	double x;
	double y;
	double z;
	double pz;
	double fWidth;
	double fHeight;

	Star()
	{
		x = 0;
		y = 0;
		z = 0;
		pz = 0;
		fWidth = 0;
		fHeight = 0;
	}

	Star(const double w, const double h)
	{
		init(w, h);
	}

	void init(const double w, const double h)
	{
		fWidth = w;
		fHeight = h;
		x = p5::random(-w, w);
		y = p5::random(-h, h);
		z = p5::random(w);
		pz = z;
	}

	void update(const double speed)
	{
		z = z - speed;
		if (z < 1) {
			z = canvasWidth;
			x = p5::random(-canvasWidth, canvasWidth);
			y = p5::random(-canvasHeight, canvasHeight);
			pz = z;
		}
	}

	void draw(IGraphics & ctx)
	{
		//printf("draw\n");
		//ctx->fill(color(255));
		//ctx->noStroke();

		auto sx = p5::map(x / z, 0, 1, 0, canvasWidth);
		auto sy = p5::map(y / z, 0, 1, 0, canvasHeight);

		auto r = p5::map(z, 0, canvasWidth, 8, 0);
		//ctx->ellipse(sx, sy, r, r);

		auto px = p5::map(x / pz, 0, 1, 0, canvasWidth);
		auto py = p5::map(y / pz, 0, 1, 0, canvasHeight);

		pz = z;

		ctx.stroke(ctx.color(255));
		ctx.fill(ctx.color(255));
		ctx.line(px, py, sx, sy);
	}
};