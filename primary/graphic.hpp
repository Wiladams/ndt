#pragma once

#include "drawable.h"

#include <memory>
#include <vector>
#include <deque>

//
// IGraphic
//	A Graphic is something that has a preferred
//	size, a boundary, and a frame.
//
//  A Graphic can also choose to handle user input from
//  various devices.
// 
//	PreferredSize - How big the graphic would like to be
//	bounds - How big the graphic actually is
//	frame - The location, within the bounds of the parent frame
//

struct IGraphic : public IDrawable
{
	virtual ~IGraphic() {};

	virtual bool contains(float x, float y)
	{
		bool res = (x >= frame().min.x) && (y >= frame().min.y) &&
			(x < frame().max.x) && (y < frame().max.y);

		return res;
	}

	// Dealing with  boundary coordinates
	virtual const maths::bbox2f& bounds() const = 0;
	float boundsX() const { return bounds().min.x; }
	float boundsY() const { return bounds().min.y; }
	float boundsWidth() const { return bounds().max.x - bounds().min.x; }
	float boundsHeight() const { return bounds().max.y - bounds().min.y; }

	//  Dealing with frame coordinates
	virtual const maths::bbox2f& frame() const = 0;
	float frameX() const { return frame().min.x; }
	float frameY() const { return frame().min.y; }
	float frameWidth() const { return frame().max.x - frame().min.x; }
	float frameHeight() const { return frame().max.y - frame().min.y; }

	virtual void moveTo(const maths::vec2f&) = 0;

	virtual void moveTo(const float nx, const float ny)
	{
		moveTo({ nx,ny });
	}

	void moveBy(const maths::vec2f& dxy)
	{
		moveTo(frame().min + dxy);
	}

	void moveBy(const float dx, const float dy)
	{
		return moveBy({ dx,dy });
	}

	// Mouse events
	virtual void mouseEntered(const MouseEvent& e) = 0;
	virtual void mouseExited(const MouseEvent& e) = 0;

	virtual void mouseEvent(const MouseEvent& e) = 0;
	//virtual bool mouseMoved(const MouseEvent& e) = 0;
	//virtual bool mouseDragged(const MouseEvent& e) = 0;
	//virtual bool mousePressed(const MouseEvent& e) = 0;
	//virtual bool mouseReleased(const MouseEvent& e) = 0;
	//virtual bool mouseWheel(const MouseEvent& e) = 0;
	//virtual bool mouseHWheel(const MouseEvent& e) = 0;

	// Keyboard events
	virtual void keyEvent(const KeyboardEvent& e) = 0;
	//virtual bool keyPressed(const KeyboardEvent& e) = 0;
	//virtual bool keyReleased(const KeyboardEvent& e) = 0;
	//virtual bool keyTyped(const KeyboardEvent& e) = 0;

	// File dropping
	virtual void fileDrop(const FileDropEvent& e) = 0;
};



//
// GraphicElement
// 
// A graphic that has a frame specified in the parent's
// coordinate space, and has an internal bounds.
// It can be moved.
struct GraphicElement : public IGraphic
{
protected:
	maths::vec2f fTranslation{};
	maths::bbox2f fBounds{};
	maths::bbox2f fFrame{};

	KeyboardEventDispatch fKeyboardDispatch;
	MouseEventDispatch fMouseDispatch;

public:

	GraphicElement()
	{
	}

	GraphicElement(const maths::bbox2f& f)
		:fFrame(f)
	{
		auto sz = maths::size(f);
		fBounds = { {0,0},{sz.x,sz.y} };
	}

	GraphicElement(float x, float y, float w, float h)
		:GraphicElement(maths::bbox2f{ {x,y},{x + w,y + h} })
	{}

	virtual ~GraphicElement() {}

	KeyboardEventDispatch & keyboardDispatch() { return fKeyboardDispatch; }
	MouseEventDispatch& mouseDispatch() { return fMouseDispatch; }

	const maths::bbox2f& bounds() const override {return fBounds;}
	void setBounds(const maths::bbox2f& b) { fBounds = b; }

	const maths::bbox2f& frame() const override { return fFrame; }
	void setFrame(const maths::bbox2f& frame) {fFrame = frame;}

	// Moves the frame
	void moveTo(const maths::vec2f &xy) override
	{
		auto dxy = xy - frame().min;
		fFrame.min += dxy;
		fFrame.max += dxy;
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

	virtual void drawBackground(IGraphics& ctx)
	{
	}

	virtual void drawForeground(IGraphics& ctx)
	{
	}

	virtual void drawSelf(IGraphics& ctx)
	{
		// this one sub-classes should implement
		// if they like
	}

	void draw(IGraphics& ctx) override
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
		ctx.clip(frame());


		// BUGBUG - maybe perform arbitrary transform?
		//auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
		//ctx.translate(pt.x, pt.y);
		ctx.translate(frameX(), frameY());

		// Apply user specified transform
		ctx.translate(fTranslation.x, fTranslation.y);

		drawBackground(ctx);
		drawSelf(ctx);
		drawForeground(ctx);

		ctx.noClip();
		ctx.pop();

	}

	virtual void mouseEvent(const MouseEvent& e) override
	{
		printf("GraphicElement:mouseEvent: %d\n", e.activity);

		fMouseDispatch(e);
	}

	virtual void mouseEntered(const MouseEvent& e) override
	{
		printf("GraphicElement::mouseEntered()\n"); 
	}

	virtual void mouseExited(const MouseEvent& e) override 
	{
		printf("GraphicElement::mouseExited()\n");
	}

	//
	// Keyboard Event handling
	//
	virtual void keyEvent(const KeyboardEvent& e) override
	{
		printf("GraphicElement:keyEvent\n");
		
		fKeyboardDispatch(e);
	}

	// fileDrop
	// Handling the case where a file was dropped atop
	// the graphic.
	virtual void fileDrop(const FileDropEvent& e) override
	{
		//printf("Graphic.fileDrop\n");
		// do nothing

	}

};

#include "layout.h"

/*

struct DepthFirstGraphicFinder
{
	std::shared_ptr<IGraphic> findAt(std::deque<std::shared_ptr<IGraphic> > gs, float x, float y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<std::shared_ptr<IGraphic> >::reverse_iterator rit = gs.rbegin();
		for (rit = gs.rbegin(); rit != gs.rend(); ++rit)
		{
			if ((*rit) != nullptr)
			{
				if ((*rit)->contains(x, y))
					return *rit;
			}
			else {
				//printf("DepthFirstGraphicFinder::findAt() - contains null reference\n");
			}
		}

		return nullptr;
	}

	std::shared_ptr<IGraphic> operator()(std::deque<std::shared_ptr<IGraphic> > gs, float x, float y)
	{
		return findAt(gs, x, y);
	}

};
*/
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

	Graphic(const maths::bbox2f &gframe)
		:GraphicElement(gframe)
	{}

	Graphic(const float x, const float y, const float w, const float h)
		:GraphicElement(x, y, w, h)
	{
	}

	void setLayout(std::shared_ptr<ILayoutGraphics> layit)
	{
		fLayout = layit;
	}

	virtual void layout()
	{
		if (nullptr != fLayout) {
			auto b = fLayout->layout(fChildren);
			setBounds(b);
		}
	}

	// Add a child to our set of children
	// If we're using layout, automatically set
	// the bounds to include the child's frame
	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
		layout();
	}
	
	// Find the topmost window at a given position
	virtual std::shared_ptr<IGraphic> graphicAt(float x, float y)
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

		//printf("Graphic::graphicAt(NULL)\n");

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

	/*
	virtual bool keyEvent(const KeyboardEvent& e) override
	{
		printf("Graphic:keyEvent\n");
		bool handled = false;

		switch (e.activity)
		{
		case KEYPRESSED:
			handled = keyPressed(e);
			break;

		case KEYRELEASED:
			handled = keyReleased(e);
			break;

		case KEYTYPED:
			handled = keyTyped(e);
			break;
		}

		return handled;
	}

	virtual bool keyPressed(const KeyboardEvent& e) override
	{
		printf("Graphic:keyPressed\n");
		// do nothing
		return false;
	}

	virtual bool keyReleased(const KeyboardEvent& e) override
	{
		printf("Graphic:keyReleased\n");

		// do nothing
		return false;
	}

	virtual bool keyTyped(const KeyboardEvent& e) override
	{
		printf("Graphic:keyTyped\n");

		// do nothing
		return false;
	}
	//*/

	// Handling mouse events
	void mouseEvent(const MouseEvent& e) override
	{
		printf("Graphic.mouseEvent: %d\n", e.activity);


/*
		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		// 
		int tx = e.x + fTranslation.x;
		int ty = e.y - fTranslation.y;

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
			newEvent.x = (tx - g->frame().min.x);
			newEvent.y = (ty - g->frame().min.y);

			handled = g->mouseEvent(newEvent);
		}
		else {
			// If the mouse event didn't land on a child
			// then do our own processing
			switch (e.activity) {
			case MOUSEDRAGGED:
				handled = mouseDragged(e);
				break;

			case MOUSEPRESSED:
				handled = mousePressed(e);
				break;

			case MOUSEMOVED:
				handled = mouseMoved(e);
				break;

			case MOUSERELEASED:
				handled = mouseReleased(e);
				break;

			case MOUSEWHEEL:
				handled = mouseWheel(e);
				break;

			case MOUSEHWHEEL:
				handled = mouseHWheel(e);
				break;
			}
		}
		*/

	}

};