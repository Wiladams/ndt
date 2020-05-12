#pragma once

#include "Surface.h"

struct IDrawable {
	virtual void draw(IGraphics *ctx) = 0;
};