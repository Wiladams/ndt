#pragma once

#include "uievent.h"
#include "Graphics.h"
#include "geometry.h"

#include <functional>

// IDrawable
// Base interface to things that can draw.
// There is no concept of boundaries or movement
// or container for other things.
// This interface is meant for those things that 
// are not UI elements, but typically static display elements
// Capture the signature of a draw routine so we can 
// get clever with that later.
using DrawRoutine = std::function<void (IGraphics& ctx)>;

struct IDrawable
{
	DrawRoutine fDrawingRoutine{};


	virtual ~IDrawable() {}



	virtual void draw(IGraphics & ctx) = 0;

	void setDrawingRoutine(DrawRoutine func) { fDrawingRoutine = func; }
};


//
// GraphicElement
// 
// A graphic that has a frame specified in the parent's
// coordinate space, and has an internal bounds.
// It can be moved.
struct GraphicElement : public IDrawable
{
protected:
	bool fIsMoveable=false;

	maths::vec2f fTranslation{};
	maths::vec2f fScale{ 1,1 };
	maths::rectf fBounds{};
	maths::rectf fFrame{};
	
	std::string fName{};

	KeyboardEventDispatch fKeyboardDispatch;
	std::function<void(GraphicElement & g, const MouseEvent& e)> fMouseDispatch{};

public:

	GraphicElement()
	{
	}

	GraphicElement(const maths::rectf& f)
		:fFrame(f)
	{
		fBounds = { 0,0,f.w, f.h };
	}

	GraphicElement(float x, float y, float w, float h)
		:GraphicElement(maths::rectf{ x,y,w,h} )
	{}

	virtual ~GraphicElement() {}

	const std::string& name() const { return fName; }
	void setName(const std::string &name) { fName = name; }

	const bool isMoveable() const { return fIsMoveable; }
	void setMoveable(bool canMove) { fIsMoveable = canMove; }

	KeyboardEventDispatch& keyboardDispatch() { return fKeyboardDispatch; }
	
	// Handling mouse dispatch
	auto & mouseDispatch() { return fMouseDispatch; }
	void setMouseDispatch(std::function<void(GraphicElement &g, const MouseEvent& e)> dispatcher) { fMouseDispatch = dispatcher; }



	virtual bool contains(float x, float y)
	{
		return maths::contains(frame(), maths::vec2f{ x,y });
	}

	// Dealing with our internal boundary
	void setBounds(const maths::rectf& b) { fBounds = b; }
	const maths::rectf& bounds() const { return fBounds; }
	float boundsX() const { return bounds().x; }
	float boundsY() const { return bounds().y; }
	float boundsWidth() const { return bounds().w; }
	float boundsHeight() const { return bounds().h; }

	// Dealing with our external frame
	void setFrame(const maths::rectf& frame) { fFrame = frame; }
	const maths::rectf& frame() const { return fFrame; }
	float frameX() const { return fFrame.x; }
	float frameY() const { return fFrame.y; }
	float frameWidth() const { return fFrame.w; }
	float frameHeight() const { return fFrame.h; }


	// Moves the frame
	virtual void moveTo(const maths::vec2f& xy)
	{
		maths::moveTo(fFrame, xy.x, xy.y);
	}
	void moveTo(const float nx, const float ny) { moveTo({ nx,ny }); }
	void moveBy(const maths::vec2f& dxy) { moveTo(frame().x + dxy.x, frame().y+dxy.y); }
	void moveBy(const float dx, const float dy) { return moveBy({ dx,dy }); }

	void scaleBoundsTo(float x, float y)
	{
		fScale.x = x;
		fScale.y = y;
	}

	void scaleBoundsBy(float dx, float dy)
	{
		fScale.x *= dx;
		fScale.y *= dy;
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
		if (fDrawingRoutine != nullptr)
			fDrawingRoutine(ctx);
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

	virtual void mouseEvent(const MouseEvent& e)
	{
		//printf("GraphicElement:mouseEvent: %d\n", e.activity);

		if (fMouseDispatch != nullptr)
			fMouseDispatch(*this, e);
	}

	virtual void mouseEntered(const MouseEvent& e)
	{
		//printf("GraphicElement::mouseEntered()\n");
	}

	virtual void mouseExited(const MouseEvent& e)
	{
		//printf("GraphicElement::mouseExited()\n");
	}

	//
	// Keyboard Event handling
	//
	virtual void keyEvent(const KeyboardEvent& e)
	{
		//printf("GraphicElement:keyEvent\n");

		fKeyboardDispatch(e);
	}

	// fileDrop
	// Handling the case where a file was dropped atop
	// the graphic.
	virtual void fileDrop(const FileDropEvent& e)
	{
		//printf("Graphic.fileDrop\n");
		// do nothing
	}

};
