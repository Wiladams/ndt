#pragma once

#include "apphost.h"
#include "uievent.h"
#include "drawable.h"
#include "gwindow.h"
//#include "windowlayout.h"
#include "layout.h"

#include <deque>
#include <memory>



/*
	Window manager

	Controls where events are sent

	Brings windows to the forefront and back depending on clicks
*/

class WindowManager : public GWindow
{
protected:

public:
	WindowManager(int w, int h)
		:GWindow(0,0,w,h)
	{
		setLayout(std::make_shared<CascadeLayout>(w,h));
	}

	virtual void drawBackground(IGraphics* ctx)
	{
		// do nothing
		// So, only application drawing occurs
	}

	// Handling mouse events
	void mouseEvent(const MouseEvent& e)
	{
		// Figure out which window the mouse pointer 
		// is currently over
		auto win = graphicAt(e.x, e.y);

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
		auto win = graphicAt(e.x, e.y);

		// if not clicked on a view, then simply return
		if (nullptr == win) {
			fActiveGraphic = nullptr;
			return;
		}

		// deactivate old active window

		// Activate new active window
		setActiveGraphic(win);

		// bring it to the front
		moveToFront(win);
		win->mousePressed(e);
	}

	void mouseMoved(const MouseEvent& e)
	{
		//std::cout << "WindowManager.mouseMoved " << e.x << ", " << e.y << std::endl;

		// return immediately if there's no active window
		if (nullptr == fActiveGraphic) {
			return;
		}

		fActiveGraphic->mouseMoved(e);
	}

	void mouseReleased(const MouseEvent& e)
	{
		// if there's no active window
		// just return immediately
		if (nullptr == fActiveGraphic)
			return;

		fActiveGraphic->mouseReleased(e);
	}


	//Handling Keyboard Events
	void keyboardEvent(const KeyboardEvent& e)
	{}

	void keyboardEvent(const KeyboardEvent& e)
	{}

	void keyPressed(const KeyboardEvent& e)
	{}

	void keyReleased(const KeyboardEvent& e)
	{}

	void keyTyped(const KeyboardEvent& e)
	{}


};