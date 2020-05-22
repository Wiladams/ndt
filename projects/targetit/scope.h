#pragma once

#include "graphic.hpp"
#include "Graphics.h"

class GScope : public virtual IDrawable
{
	int fCenterX;
	int fCenterY;
	int fRadius;

public:
	GScope()
		: fRadius(20),
		fCenterX(0),
		fCenterY(0)
	{}

	void focusOn(int x, int y)
	{
		fCenterX = x;
		fCenterY = y;
	}

	void draw(IGraphics* ctx)
	{
		// Draw a targeting circle
		// Maybe do a radial gradient so it
		// looks a bit 3D-ish
		ctx->stroke(0);
		ctx->strokeWeight(2);
		ctx->noFill();
		ctx->circle(fCenterX, fCenterY, fRadius);
		ctx->stroke(255);
		ctx->circle(fCenterX, fCenterY, fRadius - 1);

		// draw some radial lines
		ctx->push();

		for (int i = 0; i < 4; i++) {
			double angle = i * HALF_PI;
			ctx->rotate(angle, fCenterX, fCenterY);

			// darker fat line
			ctx->stroke(0);
			ctx->strokeWeight(5);
			ctx->line(fCenterX + 5, fCenterY, fCenterX + fRadius + 5, fCenterY);

			// thinner light line atop
			ctx->stroke(255, 127);
			ctx->strokeWeight(2);
			ctx->line((double)fCenterX + 5, fCenterY, (double)fCenterX + fRadius + 5, fCenterY);
		}
		ctx->pop();
	}
};
