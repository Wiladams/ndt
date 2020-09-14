#pragma once

#include "uievent.h"
#include "Graphics.h"

class IDrawable
{
protected:
	virtual ~IDrawable() {}

public:

	virtual void draw(std::shared_ptr<IGraphics> ctx) = 0;
};

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

	virtual void mouseMoved(const MouseEvent& e) = 0;
	virtual void mouseDragged(const MouseEvent& e) = 0;
	virtual void mousePressed(const MouseEvent& e) = 0;
	virtual void mouseReleased(const MouseEvent& e) = 0;

	// Keyboard events
	//virtual void keyPressed(const KeyboardEvent& e) =0;
	//virtual void keyReleased(const KeyboardEvent& e)=0;
	//virtual void keyTyped(const KeyboardEvent& e)=0;
};
