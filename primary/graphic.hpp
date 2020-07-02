#pragma once

#include "gview.h"

#include <memory>
#include <vector>

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
	virtual BLRect getFrame() const = 0;
};

class Graphic : public IGraphic
{
	BLRect fBounds{};
	BLRect fFrame{};
	std::vector<std::shared_ptr<IGraphic> > fChildren;

protected:
	void setFrame(const BLRect& frame) { fFrame = frame; }


public:
	Graphic()
	{}

	Graphic(const BLRect& frame) 
		:fFrame(frame),
		fBounds(0,0,frame.w, frame.h)
	{
	}

	BLRect getBounds() const { return fBounds; }
	BLRect getFrame() const { return fFrame; }
	
	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
	}

	void moveTo(double x, double y)
	{
		fFrame.x = x;
		fFrame.y = y;
	}

	virtual void drawBackground(IGraphics* ctx)
	{
		// here for sub-classes
	}

	virtual void drawChildren(IGraphics* ctx)
	{
		for (size_t i = 0; i < fChildren.size(); i++)
		{
			fChildren[i]->draw(ctx);
		}
	}

	virtual void drawSelf(IGraphics* ctx)
	{
		// this one sub-classes should implement
		// if they like
	}

	virtual void drawForeground(IGraphics* ctx)
	{
		// do whatever for the foreground
	}

	virtual void draw(IGraphics* ctx)
	{
		drawBackground(ctx);
		drawChildren(ctx);
		drawSelf(ctx);
		drawForeground(ctx);
	}

};