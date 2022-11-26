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
	std::string fName{};

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

	KeyboardEventDispatch& keyboardDispatch() { return fKeyboardDispatch; }
	MouseEventDispatch& mouseDispatch() { return fMouseDispatch; }

	virtual bool contains(float x, float y)
	{
		bool res = (x >= frame().min.x) && (y >= frame().min.y) &&
			(x < frame().max.x) && (y < frame().max.y);

		return res;
	}

	// Dealing with our internal boundary
	void setBounds(const maths::bbox2f& b) { fBounds = b; }
	const maths::bbox2f& bounds() const { return fBounds; }
	float boundsX() const { return bounds().min.x; }
	float boundsY() const { return bounds().min.y; }
	float boundsWidth() const { return bounds().max.x - bounds().min.x; }
	float boundsHeight() const { return bounds().max.y - bounds().min.y; }

	// Dealing with our external frame
	void setFrame(const maths::bbox2f& frame) { fFrame = frame; }
	const maths::bbox2f& frame() const { return fFrame; }
	float frameX() const { return fFrame.min.x; }
	float frameY() const { return fFrame.min.y; }
	float frameWidth() const { return fFrame.max.x - fFrame.min.x; }
	float frameHeight() const { return fFrame.max.y - fFrame.min.y; }


	// Moves the frame
	virtual void moveTo(const maths::vec2f& xy)
	{
		auto dxy = xy - frame().min;
		fFrame.min += dxy;
		fFrame.max += dxy;
	}
	void moveTo(const float nx, const float ny) { moveTo({ nx,ny }); }
	void moveBy(const maths::vec2f& dxy) { moveTo(frame().min + dxy); }
	void moveBy(const float dx, const float dy) { return moveBy({ dx,dy }); }

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
		printf("GraphicElement:mouseEvent: %d\n", e.activity);

		fMouseDispatch(e);
	}

	virtual void mouseEntered(const MouseEvent& e)
	{
		printf("GraphicElement::mouseEntered()\n");
	}

	virtual void mouseExited(const MouseEvent& e)
	{
		printf("GraphicElement::mouseExited()\n");
	}

	//
	// Keyboard Event handling
	//
	virtual void keyEvent(const KeyboardEvent& e)
	{
		printf("GraphicElement:keyEvent\n");

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
