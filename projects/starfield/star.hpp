#pragma once

#include "p5.hpp"

using namespace p5;

struct Star {
	double x;
	double y;
	double z;
	double pz;
	double width;
	double height;

	Star()
	{
		x = 0;
		y = 0;
		z = 0;
		pz = 0;
		width = 0;
		height = 0;
	}

	Star(const double w, const double h)
	{
		init(w, h);
	}

	void init(const double w, const double h)
	{
		width = w;
		height = h;
		x = random(-width, width);
		y = random(-height, height);
		z = random(width);
		pz = z;
	}

	void update(const double speed)
	{
		z = z - speed;
		if (z < 1) {
			z = width;
			x = random(-width, width);
			y = random(-height, height);
			pz = z;
		}
	}

	void draw(Surface& g)
	{
		g.fill(color(255));
		g.noStroke();

		auto sx = map(x / z, 0, 1, 0, width);
		auto sy = map(y / z, 0, 1, 0, height);

		auto r = map(z, 0, width, 8, 0);

		g.ellipse(sx, sy, r, r);

		auto px = map(x / pz, 0, 1, 0, width);
		auto py = map(y / pz, 0, 1, 0, height);

		pz = z;

		g.stroke(color(255));
		line(px, py, sx, sy);
	}
};