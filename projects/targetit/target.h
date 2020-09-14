#pragma once

#include "graphic.hpp"
#include "Graphics.h"

struct GTarget : public virtual IDrawable
{
	int fCenterX;
	int fCenterY;
	int fRadius;

	GTarget(int cx, int cy, int r)
		: fCenterX(cx),
		fCenterY(cy),
		fRadius(r)
	{
	}

	void draw(std::shared_ptr<IGraphics> ctx)
	{
		ctx->noStroke();
		ctx->fill(255, 0, 0);
		ctx->circle(fCenterX, fCenterY, fRadius);

		ctx->fill(255);
		ctx->circle(fCenterX, fCenterY, fRadius * 0.618);

		ctx->fill(255, 0, 0);
		ctx->circle(fCenterX, fCenterY, fRadius * 0.618 / 2.);
	}
};
