#pragma once

#include "apphost.h"
#include "uievent.h"
#include "drawable.h"
#include "gwindow.h"
#include "windowlayout.h"

#include <deque>




/*
	Window manager

	Controls where events are sent

	Brings windows to the forefront and back depending on clicks
*/

class WindowManager : IDrawable
{
protected:
	std::deque<std::shared_ptr<GWindow> > windows;
	std::shared_ptr<GWindow> fActiveWindow;
	std::shared_ptr<IArrangeWindows> fLayout;

public:
	WindowManager(int w, int h)
	{
		fLayout = std::make_shared<CascadeWindowLayout>(w,h);
	}

	void setArrangement(std::shared_ptr<IArrangeWindows> layout)
	{
		fLayout = layout;
	}

	void draw(IGraphics* ctx)
	{
		for (std::shared_ptr<GWindow> win : windows)
		{
			win->draw(ctx);
			ctx->flush();
		}
	}

	virtual void addWindow(std::shared_ptr<GWindow> win)
	{
		windows.push_back(win);
		if (nullptr != fLayout)
			fLayout->addWindow(win);
		
		onWindowAdded(win);
	}

	virtual void onWindowAdded(std::shared_ptr<GWindow> win)
	{
		// perhaps do some layout
	}

	// Find the topmost window at a given position
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

	void setActiveWindow(std::shared_ptr<GWindow> win)
	{
		fActiveWindow = win;
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

	// Handling mouse events
	/*
	void mouseClicked(const MouseEvent& e);
	void mouseDragged(const MouseEvent& e);
	void mouseMoved(const MouseEvent& e);
	void mousePressed(const MouseEvent& e);
	void mouseReleased(const MouseEvent& e);
	void mouseWheel(const MouseEvent& e);
	*/
	void mouseEvent(const MouseEvent& e)
	{
		// Figure out which window the mouse pointer 
		// is currently over
		auto win = windowAt(e.x, e.y);

		// Call an appropriate event
		// handler depending on activity
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
		setActiveWindow(win);

		// bring it to the front
		moveToFront(win);
		win->mousePressed(e);

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

	/*
		Handling Keyboard Events
		void keyboardEvent(const KeyEvent& e);

		void keyPressed(const KeyEvent& e);
		void keyReleased(const KeyEvent& e);
		void keyTyped(const KeyEvent& e);
	*/

};