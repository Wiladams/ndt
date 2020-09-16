#pragma once

#include "apphost.h"
#include "uievent.h"
#include "drawable.h"
#include "gwindow.h"
#include "layout.h"

#include <deque>
#include <memory>


/*
	Window manager

	Controls layout of new windows
	Controls where events are sent
	Brings windows to the forefront and back depending on clicks
	Sends UI events to correct window
*/

class WindowManager : public Graphic
{
protected:

public:
	WindowManager(int w, int h)
		:Graphic(0,0,w,h)
	{
		setLayout(std::make_shared<CascadeLayout>(w,h));
	}

	virtual void drawForeground(std::shared_ptr<IGraphics> ctx)
	{
		// then composite each windows image into the ctx
	}

/*
	// Handling mouse events
	// called from p5 event topic typically
	void mouseEvent(const MouseEvent& e)
	{
		auto g = graphicAt(e.x, e.y);

		//std::cout << "windowManager.mouseEvent original: " << e.x << ", " << e.y << " window: " << g << std::endl;

		if (g != nullptr) {
			// If it's a sub-graphic, then continue down the chain
			auto newEvent = e;
			newEvent.x = e.x - g->getFrame().x;
			newEvent.y = e.y - g->getFrame().y;
		
			g->mouseEvent(newEvent);
		}
		else {
			// If the mouse event didn't land on a child
			// then do our own processing
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
	}
*/

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


	void keyPressed(const KeyboardEvent& e)
	{}

	void keyReleased(const KeyboardEvent& e)
	{}

	void keyTyped(const KeyboardEvent& e)
	{}


};