#pragma once

#include "drawable.h"
#include "layout.h"

#include <memory>
#include <vector>
#include <deque>


//
// GraphicElement
// 
// A graphic that has a frame specified in the parent's
// coordinate space, and has an internal bounds.
// It can be moved.
class GraphicElement : public IGraphic
{
protected:
	BLMatrix2D fTransform{};  // Internal transformation matrix
	maths::vec2f fTranslation{};

	BLRect fFrame{};
	BLRect fBounds{};

public:
	GraphicElement()
	{
		fTransform = BLMatrix2D::makeIdentity();
	}

	GraphicElement(float x, float y, float w, float h)
		:fFrame(x,y,w,h)
		,fBounds(0,0,w,h)
	{
		fTransform = BLMatrix2D::makeIdentity();
	}


	const BLRect & bounds() const { return fBounds; }
	void setBounds(const BLRect& b) { fBounds = b; }

	const BLRect & frame() const { return fFrame; }
	void setFrame(const BLRect& frame)
	{
		fFrame = frame;
		//fBounds = BLRect(0, 0, frame.w, frame.h);
	}

	// Moves the frame
	virtual void moveTo(const float x, const float y)
	{
		fFrame.x = x;
		fFrame.y = y;
	}

	// Changes the coordinate system of the bounds
	void translateBoundsTo(float x, float y)
	{
		fTranslation = { x,y };
	}

	// Changes the coordinate system of the bounds
	void translateBoundsBy(float x, float y)
	{
		fTranslation += {x, y};
	}

	void setTransform(BLMatrix2D& m) { fTransform = m; }
	BLMatrix2D& getTransform() { return fTransform; }

	virtual void drawBackground(IGraphics& ctx)
	{
		//ctx->flush();
	}

	virtual void drawForeground(IGraphics& ctx)
	{
		//ctx->flush();
	}

	virtual void drawSelf(IGraphics& ctx)
	{
		// this one sub-classes should implement
		// if they like
	}

	virtual void draw(IGraphics& ctx)
	{
		// Start by saving the context state
		// so we're free to mess around with it
		// while we're drawing ourself.
		ctx.push();

		// Before we do anything else, and while we're still
		// in the coordinate system of our parent, we want to setup 
		// a clip for our frame.
		// Once the clip is set, we want to transform our
		// coordinate sytem to have 0,0 be at the upper left corner.

		ctx.clip(fFrame.x, fFrame.y, fFrame.w, fFrame.h);


		// BUGBUG - maybe perform arbitrary transform?
		//auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
		//ctx.translate(pt.x, pt.y);
		ctx.translate(fFrame.x, fFrame.y);

		// Apply user specified transform
		ctx.translate(fTranslation.x, fTranslation.y);

		drawBackground(ctx);
		drawSelf(ctx);
		drawForeground(ctx);

		ctx.noClip();
		ctx.pop();

	}

	// Handling mouse events
	virtual void mouseEvent(const MouseEvent& e)
	{
		printf("GraphicElement.mouseEvent: %d\n", e.activity);

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		// 
		int tx = e.x + fTranslation.x;
		int ty = e.y - fTranslation.y;

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		//auto newEvent(e);
		//newEvent.x = (int)(pt.x + fFrame.x);
		//newEvent.y = (int)(pt.y + fFrame.y);


		// If the mouse event didn't land on a child
		// then do our own processing
		switch (e.activity) 
		{
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

			case MOUSEHWHEEL:
				mouseHWheel(e);
				break;
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
		printf("mouseReleased()\n");
		// do nothing
	}

	virtual void mouseWheel(const MouseEvent& e)
	{
		//printf("Graphic.mouseWheel\n");
		// do nothing
	}

	virtual void mouseHWheel(const MouseEvent& e)
	{
		//printf("Graphic.mouseHWheel\n");
		// do nothing
	}

	virtual void fileDrop(const FileDropEvent& e)
	{
		//printf("Graphic.fileDrop\n");
		// do nothing
	}

};


class Graphic : public GraphicElement
{
protected:
	std::deque<std::shared_ptr<IGraphic> > fChildren{};
	std::shared_ptr<ILayoutGraphics> fLayout{};
	std::shared_ptr<IGraphic> fActiveGraphic{};
	
public:
	Graphic()
		:Graphic(0.0, 0, 0, 0)
	{}


	Graphic(const float x, const float y, const float w, const float h)
		:GraphicElement(x, y, w, h)
	{
	}

	double width() const { return fBounds.w; }
	double height() const { return fBounds.h; }


	void setLayout(std::shared_ptr<ILayoutGraphics> layout)
	{
		fLayout = layout;
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
			if ((*rit) != nullptr)
			{
				if ((*rit)->contains(x, y))
					return *rit;
			}
			else {
				//printf("GraphicAt() - contains null reference\n");
			}
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

	virtual void drawChildren(IGraphics & ctx)
	{
		for (auto & g : fChildren)
		{
			// BUGBUG - really there shouldn't be any nullptr graphics
			if (nullptr == g)
				continue;

			g->draw(ctx);
		}
	}

	void drawSelf(IGraphics& ctx) override
	{
		drawChildren(ctx);
	}

	// Handling mouse events
	void mouseEvent(const MouseEvent& e) override
	{
		//printf("Graphic.mouseEvent: %d\n", e.activity);

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		// 
		int tx = e.x + fTranslation.x;
		int ty = e.y - fTranslation.y;

		//std::cout << "graphic.mouseEvent original: " << e.x << ", " << e.y << " modified: " << pt.x << ", " << pt.y << std::endl;
		//std::cout << "graphic.mouseEvent original: " << e.x << ", " << e.y << std::endl;

		// Figure out which child the mouse pointer 
		// is currently over
		auto g = graphicAt(tx, ty);

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		//auto newEvent(e);
		//newEvent.x = (int)(pt.x + fFrame.x);
		//newEvent.y = (int)(pt.y + fFrame.y);

		if (g != nullptr) {
			// If it's a sub-graphic, then continue down the chain
			MouseEvent newEvent = e;
			//newEvent.x = (int)(e.x - g->getFrame().x);
			//newEvent.y = (int)(e.y - g->getFrame().y);
			newEvent.x = (int)(tx - g->frame().x);
			newEvent.y = (int)(ty - g->frame().y);

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

			case MOUSEHWHEEL:
				mouseHWheel(e);
				break;
			}
		}

	}

};