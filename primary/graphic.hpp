#pragma once

#include "p5.hpp"
#include "drawable.hpp"

/*
	A Graphic is something that has a preferred
	size, a boundary, and a frame.  
	
	PreferredSize - How big the graphic would like to be
	Bounds - How big the graphic actually is
	Frame - The location, within the bounds of the parent frame
	
*/
struct IGraphic {
	virtual BLSize getPreferredSize() = 0;
	virtual BLRectI getBounds() = 0;
	virtual BLRectI getFrame() = 0;
};

class Graphic : IDrawable, IGraphic
{
	BLRectI fFrame;
	BLRectI fBounds;

public:
	Graphic(BLRectI bounds) 
		:fBounds(bounds)
	{

	}

	virtual void draw(Surface* surf) = 0;

};