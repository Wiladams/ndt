#pragma once

#include "Graphics.h"

class IDrawable
{
protected:
	virtual ~IDrawable() {}

public:

	virtual void draw(IGraphics* ctx) = 0;
};
