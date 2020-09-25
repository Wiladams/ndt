#pragma once

#include "p5.hpp"

using namespace p5;

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
		x = random(-w, w);
		y = random(-h, h);
		z = random(w);
		pz = z;
		std::cout << "star::init " << x << " " << y << " " << z << std::endl;
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

	void draw(std::shared_ptr<Surface> ctx)
	{
		//printf("draw\n");
		//ctx->fill(color(255));
		//ctx->noStroke();

		auto sx = map(x / z, 0, 1, 0, width);
		auto sy = map(y / z, 0, 1, 0, height);

		auto r = map(z, 0, width, 8, 0);
		//std::cout << "radius: " << r << std::endl;
		//ctx->ellipse(sx, sy, r, r);

		auto px = map(x / pz, 0, 1, 0, width);
		auto py = map(y / pz, 0, 1, 0, height);

		pz = z;


		ctx->stroke(ctx->color(255));
		ctx->fill(ctx->color(255));
		ctx->line(px, py, sx, sy);
		//printf("star line: %3.2f,%3.2f <=> %3.2f,%3.2f\n", px, py, sx, sy);
	}
};