#pragma once

#include "drawable.h"
#include "layout.h"

#include <memory>
#include <vector>
#include <deque>



class Graphic : public IGraphic
{
protected:
	std::deque<std::shared_ptr<IGraphic> > fChildren;
	std::shared_ptr<ILayoutGraphics> fLayout;
	std::shared_ptr<IGraphic> fActiveGraphic;
	


	BLMatrix2D fTransform;  // Internal transformation matrix

	BLRect fFrame{};
	BLRect fBounds{};

public:
	Graphic()
		:Graphic(0.0, 0, 0, 0)
	{}


	Graphic(const double x, const double y, const double w, const double h)
		:fFrame(x, y, w, h)
		,fBounds(0, 0, w, h)
	{
		fTransform = BLMatrix2D::makeIdentity();
	}

	Graphic(const BLRect& frame) 
		:fFrame(frame)
		,fBounds(0,0,frame.w, frame.h)
		,fActiveGraphic(nullptr)
	{
		fTransform = BLMatrix2D::makeIdentity();
	}

	//BLRect& bounds() { return fBounds; }
	BLRect getBounds() const { return fBounds; }
	void setBounds(const BLRect& b) { fBounds = b; }
	double width() const { return fBounds.w; }
	double height() const { return fBounds.h; }

	void setFrame(const BLRect& frame) { fFrame = frame; }
	virtual BLRect getFrame() const { return fFrame; }

	void setLayout(std::shared_ptr<ILayoutGraphics> layout)
	{
		fLayout = layout;
	}

	void translateBy(double x, double y)
	{
		fTransform.translate(x, y);
	}

	void setTransform(BLMatrix2D& m) { fTransform = m; }
	BLMatrix2D& getTransform() { return fTransform; }

	virtual bool contains(int x, int y)
	{
		return ((x >= fFrame.x) && (y >= fFrame.y) &&
			(x - fFrame.x <= fFrame.w) &&
			(y - fFrame.y <= fFrame.h));
	}

	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
		if (nullptr != fLayout) {
			fLayout->layout(fChildren);
		}
	}
	
	// Find the topmost window at a given position
	std::shared_ptr<IGraphic> graphicAt(int x, int y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<std::shared_ptr<IGraphic> >::reverse_iterator rit = fChildren.rbegin();
		for (rit = fChildren.rbegin(); rit != fChildren.rend(); ++rit)
		{
			if ((*rit)->contains(x, y))
				return *rit;
		}

		return nullptr;
	}

	virtual void setActiveGraphic(std::shared_ptr<IGraphic> g)
	{
		fActiveGraphic = g;
	}

	void moveToFront(std::shared_ptr<IGraphic> g)
	{
		std::deque<std::shared_ptr<IGraphic> >::iterator it = fChildren.begin();
		for (it = fChildren.begin(); it != fChildren.end(); ++it)
		{
			if (*it == g) {
				fChildren.erase(it);
				fChildren.push_back(g);
				break;
			}
		}
	}

	void moveTo(double x, double y)
	{
		fFrame.x = x;
		fFrame.y = y;
	}

	virtual void drawBackground(IGraphics & ctx)
	{
		//ctx->flush();
	}

	virtual void drawChildren(IGraphics & ctx)
	{
		for (std::shared_ptr<IGraphic> g : fChildren)
		{
			// BUGBUG - really there shouldn't be any nullptr graphics
			if (nullptr == g)
				continue;

			g->draw(ctx);
		}
	}

	virtual void drawSelf(IGraphics & ctx)
	{
		// this one sub-classes should implement
		// if they like
	}

	virtual void drawForeground(IGraphics & ctx)
	{
		//ctx->flush();
	}



	virtual void draw(IGraphics & ctx)
	{
		ctx.push();
		//ctx->clip(fFrame.x, fFrame.y, fFrame.w, fFrame.h);


		// BUGBUG - maybe perform arbitrary transform?
		auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
		ctx.translate(pt.x, pt.y);

		drawBackground(ctx);
		drawChildren(ctx);
		drawSelf(ctx);
		drawForeground(ctx);

		//ctx->noClip();
		ctx.pop();
		
	}


	// Handling mouse events
	virtual void mouseEvent(const MouseEvent& e)
	{
		//printf("Graphic.mouseEvent: %d\n", e.activity);

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		//std::cout << "graphic.mouseEvent original: " << e.x << ", " << e.y << " modified: " << pt.x << ", " << pt.y << std::endl;
		//std::cout << "graphic.mouseEvent original: " << e.x << ", " << e.y << std::endl;

		// Figure out which child the mouse pointer 
		// is currently over
		auto g = graphicAt(e.x, e.y);

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		//auto newEvent(e);
		//newEvent.x = (int)(pt.x + fFrame.x);
		//newEvent.y = (int)(pt.y + fFrame.y);

		if (g != nullptr) {
			// If it's a sub-graphic, then continue down the chain
			MouseEvent newEvent = e;
			newEvent.x = (int)(e.x - g->getFrame().x);
			newEvent.y = (int)(e.y - g->getFrame().y);

			g->mouseEvent(newEvent);
		}
		else {
			// If the mouse event didn't land on a child
			// then do our own processing
			switch (e.activity) {
			case MOUSEDRAGGED:
				mouseDragged(e);
				break;

			case MOUSEPRESSED:
				mousePressed(e);
				break;

			case MOUSEMOVED:
				mouseMoved(e);
				break;

			case MOUSERELEASED:
				mouseReleased(e);
				break;

			case MOUSEWHEEL:
				mouseWheel(e);
				break;
			}
		}

	}

	virtual void mouseMoved(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mouseDragged(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mousePressed(const MouseEvent& e)
	{
		// do nothing
		//printf("Graphic::mousePressed\n");
	}

	virtual void mouseReleased(const MouseEvent& e)
	{
		// do nothing
	}

	virtual void mouseWheel(const MouseEvent& e)
	{
		printf("Graphic.mouseWheel\n");
		// do nothing
	}

	virtual void fileDrop(const FileDropEvent& e)
	{
		// do nothing
	}

};