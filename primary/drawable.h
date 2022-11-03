#pragma once

#include "uievent.h"
#include "Graphics.h"
#include "geometry.h"

// IDrawable
// Base interface to things that can draw.
// There is no concept of boundaries or movement
// or container for other things.
// This interface is meant for those things that 
// are not UI elements, but typically static display elements
struct IDrawable
{

	virtual ~IDrawable() {}

	virtual void draw(IGraphics & ctx) = 0;
};

//
// IGraphic
//	A Graphic is something that has a preferred
//	size, a boundary, and a frame.
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
		return (x >= frame().min.x) && (y >= frame().min.y) &&
			(x < frame().max.x) && (y < frame().max.y);
	}

	// Dealing with  boundary coordinates
	virtual const maths::bbox2f& bounds() const = 0;
	float boundsX() const { return bounds().min.x; }
	float boundsY() const { return bounds().min.y; }
	float boundsWidth() const { return bounds().max.x - bounds().min.x; }
	float boundsHeight() const { return bounds().max.y - bounds().min.y; }

	//  Dealing with frame coordinates
	virtual const maths::bbox2f & frame() const = 0;
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
	virtual void mouseEvent(const MouseEvent& e) = 0;
	virtual void mouseMoved(const MouseEvent& e) = 0;
	virtual void mouseDragged(const MouseEvent& e) = 0;
	virtual void mousePressed(const MouseEvent& e) = 0;
	virtual void mouseReleased(const MouseEvent& e) = 0;
	virtual void mouseWheel(const MouseEvent& e) = 0;
	virtual void mouseHWheel(const MouseEvent& e) = 0;

	// Keyboard events
	virtual void keyEvent(const KeyboardEvent& e) = 0;
	virtual void keyPressed(const KeyboardEvent& e) =0;
	virtual void keyReleased(const KeyboardEvent& e)=0;
	virtual void keyTyped(const KeyboardEvent& e)=0;

	// File dropping
	virtual void fileDrop(const FileDropEvent& e) = 0;
};
