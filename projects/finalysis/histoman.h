#pragma once

#include "p5.hpp"
#include "filehisto.h"
#include "gwindow.h"

#include <memory>
#include <deque>

class HistoManager : public IDrawable
{
	int wX = 10;
	int wY = 10;

	std::deque<std::shared_ptr<GWindow> > windows;
	std::shared_ptr<GWindow> fActiveWindow;

public:
	void draw(IGraphics* ctx)
	{
		for (std::shared_ptr<GWindow> win : windows)
		{
			win->draw(ctx);
			ctx->flush();
		}
	}

	void addWindow(std::shared_ptr<GWindow> win)
	{
		// do rudimentary layout as we add windows
		//layout();

		int x = wX;
		int y = wY;
		wX += 256 - 10;
		wY += 10;

		if (wX > width - 256) {
			wX = 10;
			wY += 256 + 8;
		}

		win->moveTo(x, y);

		windows.push_back(win);
	}

	std::shared_ptr<GWindow> windowAt(int x, int y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<shared_ptr<GWindow> >::reverse_iterator rit = windows.rbegin();
		for (rit = windows.rbegin(); rit != windows.rend(); ++rit)
		{
			if ((*rit)->contains(x, y))
				return *rit;
		}

		return nullptr;
	}

	void moveToFront(std::shared_ptr<GWindow> win)
	{
		std::deque<std::shared_ptr<GWindow> >::iterator it = windows.begin();
		for (it = windows.begin(); it != windows.end(); ++it)
		{
			if (*it == win) {
				windows.erase(it);
				windows.push_back(win);
				break;
			}
		}
	}

	void mouseEvent(const MouseEvent& e)
	{
		switch (e.activity) {
		case MOUSEPRESSED:
			mousePressed(e);
			break;

		case MOUSEMOVED:
			mouseMoved(e);
			break;

		case MOUSERELEASED:
			mouseReleased(e);
			break;
		}
	}

	void mousePressed(const MouseEvent& e)
	{
		// Figure out which window is being 
		// clicked
		auto win = windowAt(e.x, e.y);

		// if not clicked on a view, then simply return
		if (nullptr == win) {
			fActiveWindow = nullptr;
			return;
		}

		// deactivate old active window

		// Activate new active window
		fActiveWindow = win;
		// bring it to the front
		moveToFront(fActiveWindow);
		fActiveWindow->mousePressed(e);

		std::cout << "WindowManager.mousePressed " << e.x << ", " << e.y << std::endl;
	}

	void mouseMoved(const MouseEvent& e)
	{
		//std::cout << "WindowManager.mouseMoved " << e.x << ", " << e.y << std::endl;

		// return immediately if there's no active window
		if (nullptr == fActiveWindow) {
			return;
		}

		fActiveWindow->mouseMoved(e);
	}

	void mouseReleased(const MouseEvent& e)
	{
		// if there's no active window
		// just return immediately
		if (nullptr == fActiveWindow)
			return;

		fActiveWindow->mouseReleased(e);
	}
};
