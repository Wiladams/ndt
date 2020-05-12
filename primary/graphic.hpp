#pragma once

#include "p5.hpp"


class IDrawable 
{
protected:
	virtual ~IDrawable() {}

public:

	virtual void draw(IGraphics* ctx) = 0;
};

/*
	A Graphic is something that has a preferred
	size, a boundary, and a frame.  
	
	PreferredSize - How big the graphic would like to be
	Bounds - How big the graphic actually is
	Frame - The location, within the bounds of the parent frame
	
*/
class IGraphic : public IDrawable 
{
protected:
	virtual ~IGraphic() {};

public:
	virtual BLRectI getBounds() const = 0;
};

class Graphic : public IGraphic
{
	BLRectI fBounds{};

protected:
	void setBounds(const BLRectI& bounds) { fBounds = bounds; }

public:
	Graphic(const BLRectI& bounds) 
		:fBounds(bounds) 
	{}


	BLRectI getBounds() const { return fBounds; }

	virtual void draw(IGraphics* surf)
	{
		// Do nothing in particular
	}

};