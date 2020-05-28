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
class IGraphic : public virtual IDrawable 
{
protected:
	virtual ~IGraphic() {};

public:
	virtual BLRectI getBounds() const = 0;
};

class Graphic : public virtual IGraphic
{
	BLRectI fBounds{};
	BLRectI fFrame{};
	std::vector<IGraphic*> fChildren;

protected:
	void setBounds(const BLRectI& bounds) { fBounds = bounds; }


public:
	Graphic()
	{}

	Graphic(const BLRectI& bounds) 
		:fBounds(bounds) ,
		fFrame(bounds)
	{}


	BLRectI getBounds() const { return fBounds; }
	
	void addChild(IGraphic* child)
	{
		fChildren.push_back(child);
	}

	void moveTo(double x, double y)
	{
		fFrame.x = x;
		fFrame.y = y;
	}

	virtual void draw(IGraphics* surf)
	{
		// Do nothing in particular
	}

};