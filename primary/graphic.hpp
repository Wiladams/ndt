#pragma once

#include "drawable.h"

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
	virtual void moveTo(const double x, const double y) = 0;
};

class Graphic : public IGraphic
{


	std::vector<std::shared_ptr<IGraphic> > fChildren;

protected:
	BLMatrix2D fTransform;  // Internal transformation matrix
	BLRect fBounds{};
	BLRect fFrame{};




public:
	Graphic()
	{
		fTransform = BLMatrix2D::makeIdentity();
	}

	Graphic(const BLRect& frame) 
		:fFrame(frame),
		fBounds(0,0,frame.w, frame.h)
	{
		fTransform = BLMatrix2D::makeIdentity();
	}

	BLRect getBounds() const { return fBounds; }
	
	void setFrame(const BLRect& frame) { fFrame = frame; }
	virtual BLRect getFrame() const { return fFrame; }

	void translateBy(double x, double y)
	{
		fTransform.translate(x, y);
	}

	void setTransform(BLMatrix2D& m) { fTransform = m; }
	BLMatrix2D& getTransform() { return fTransform; }

	bool contains(int x, int y)
	{
		return ((x >= fFrame.x) && (y >= fFrame.y) &&
			(x - fFrame.x <= fFrame.w) &&
			(y - fFrame.y <= fFrame.h));
	}

	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
	}

	void moveBy(double dx, double dy)
	{
		fFrame.x += dx;
		fFrame.y += dy;
	}

	void moveTo(double x, double y)
	{
		fFrame.x = x;
		fFrame.y = y;
	}

	virtual void drawBackground(IGraphics* ctx)
	{
		//ctx->flush();
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
		//ctx->flush();
	}

	virtual void draw(IGraphics* ctx)
	{
		ctx->push();
		ctx->clip(fFrame.x, fFrame.y, fFrame.w, fFrame.h);


		// BUGBUG - maybe perform arbitrary transform?
		auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
		ctx->translate(pt.x, pt.y);

		drawBackground(ctx);
		drawChildren(ctx);
		drawSelf(ctx);
		drawForeground(ctx);

		ctx->noClip();
		ctx->pop();
	}

	// Mouse Handling
	virtual void mouseMoved(const MouseEvent& e)
	{
		// translate according to the transformation
		auto pt = fTransform.mapPoint(e.x, e.y);
		auto newEvent(e);
		newEvent.x = (int)(pt.x + fFrame.x);
		newEvent.y = (int)(pt.y + fFrame.y);
	}

	virtual void mouseDragged(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mousePressed(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mouseReleased(const MouseEvent& e)
	{
		// do nothing
	}
};