#pragma once

#include "uievent.h"
#include "Graphics.h"

struct IDrawable
{

	virtual ~IDrawable() {}

	virtual void draw(IGraphics & ctx) = 0;
};

/*
	A Graphic is something that has a preferred
	size, a boundary, and a frame.

	PreferredSize - How big the graphic would like to be
	Bounds - How big the graphic actually is
	Frame - The location, within the bounds of the parent frame

*/
struct IGraphic : public IDrawable
{
	virtual ~IGraphic() {};

	virtual bool contains(int x, int y)
	{
		auto f = getFrame();
		return ((x >= f.x) && (y >= f.y) &&
			(x - f.x <= f.w) &&
			(y - f.y <= f.h));
	}

	virtual BLRect getFrame() const = 0;
	virtual void moveTo(const double x, const double y) = 0;

	virtual void moveBy(double dx, double dy)
	{
		auto f = getFrame();
		moveTo(f.x + dx, f.y + dy);
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
