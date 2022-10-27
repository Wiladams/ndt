#pragma once

#include "uievent.h"
#include "Graphics.h"
#include "geometry.h"

struct IDrawable
{

	virtual ~IDrawable() {}

	virtual void draw(IGraphics & ctx) = 0;
};

/*
	A Graphic is something that has a preferred
	size, a boundary, and a frame.

	PreferredSize - How big the graphic would like to be
	bounds - How big the graphic actually is
	frame - The location, within the bounds of the parent frame

*/
struct IGraphic : public IDrawable
{
	virtual ~IGraphic() {};

	virtual bool contains(float x, float y)
	{
		return (x >= frame().min.x) && (y >= frame().min.y) &&
			(x < frame().max.x) && (y < frame().max.y);
	}

	// Dealing with  boundary coordinates
	virtual const maths::bbox2f& bounds() const = 0;
	constexpr float boundsX() { return bounds().min.x; }
	constexpr float boundsY() { return bounds().min.y; }
	float boundsWidth() { return bounds().max.x - bounds().min.x; }
	float boundsHeight() { return bounds().max.y - bounds().min.y; }

	//  Dealing with frame coordinates
	virtual const maths::bbox2f & frame() const = 0;
	constexpr float frameX() { return frame().min.x; }
	constexpr float frameY() { return frame().min.y; }
	float frameWidth() { return frame().max.x - frame().min.x; }
	float frameHeight() { return frame().max.y - frame().min.y; }

	/*
	// Convenience methods for the size of the frame
	// if you need to know the size of the contents
	// use the bounds()
	maths::vec2f size() {
		return maths::size(frame());
	}

	float width() {
		auto sz = maths::size(frame());
		return sz.x;
	}

	float height() {
		auto sz = maths::size(frame());
		return sz.y;
	}
	*/



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

	virtual void mouseEvent(const MouseEvent& e) = 0;
	virtual void mouseMoved(const MouseEvent& e) = 0;
	virtual void mouseDragged(const MouseEvent& e) = 0;
	virtual void mousePressed(const MouseEvent& e) = 0;
	virtual void mouseReleased(const MouseEvent& e) = 0;
	virtual void mouseWheel(const MouseEvent& e) = 0;
	virtual void mouseHWheel(const MouseEvent& e) = 0;

	// Keyboard events
	//virtual void keyPressed(const KeyboardEvent& e) =0;
	//virtual void keyReleased(const KeyboardEvent& e)=0;
	//virtual void keyTyped(const KeyboardEvent& e)=0;

	// File dropping
	virtual void fileDrop(const FileDropEvent& e) = 0;
};
