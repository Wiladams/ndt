#pragma once

#include "p5.hpp"
#include "gwindow.h"
#include <memory>
#include <deque>

class IArrangeWindows {
public:
	virtual void addWindow(std::shared_ptr<GWindow> win)
	{
		// By default do nothing 
	}

	virtual void layout(std::deque<std::shared_ptr<GWindow> > wins)
	{
		// by default, do nothing
	}
};

class CascadeWindowLayout : public IArrangeWindows
{
	int wX = 10;
	int wY = 49;
	int fWidth;
	int fHeight;
	int fVerticalOffset = 10;
	int fHorizontalOffset = -10;
	int fVerticalGap = 8;

public:
	CascadeWindowLayout(int w, int h)
	{
		fWidth = w;
		fHeight = h;
	}

	virtual void addWindow(std::shared_ptr<GWindow> win)
	{
		int x = wX;
		int y = wY;

		wX += (int)(win->getFrame().w + fHorizontalOffset);
		wY += (int)fVerticalOffset;

		if (wX > fWidth - 256) 
		{
			wX = 10;
			wY += 256 + fVerticalGap;
		}

		win->moveTo(x, y);
	}

	// Perform layout starting from scratch
	void layout(std::deque<std::shared_ptr<GWindow> > wins)
	{
		int x = 10;
		int y = 49;

		for (std::shared_ptr<GWindow> win : wins)
		{
			int offsetx = x;
			int offsety = y;

			x += (int)(win->getFrame().w + fHorizontalOffset);
			y += fVerticalOffset;

			if (x > fWidth - 256) {
				x = 10;
				y += 256 + fVerticalGap;
			}

			win->moveTo(offsetx, offsety);
		}
	}
};
