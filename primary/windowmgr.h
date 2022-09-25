#pragma once

#include "apphost.h"
#include "p5.hpp"
#include "gwindow.h"
#include "guistyle.hpp"

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
		//setLayout(std::make_shared<CascadeLayout>(w,h));
		setLayout(std::make_shared<IdentityLayout>());

	}

	// Maybe we want to do some special drawing
	// around each window
	virtual void drawChildren(IGraphics & ctx)
	{
		auto shadow = ctx.color(0x40);

		// Here we draw each window
		// along the way, we can display whatever
		// chrome we want, or do any rendering effects

		for (std::shared_ptr<IGraphic> g : fChildren)
		{
			auto f = g->getFrame();

			//GUIStyle::drawDropShadow(ctx, f, 8, shadow);
			
			// Tell the window to draw itself into the context
			g->draw(ctx);
		}
	}

	// Handling mouse events
	virtual void mouseEvent(const MouseEvent& e)
	{
		//printf("WindowManager.mouseEvent: %d\n", e.activity);
		// Figure out which child the mouse pointer 
		// is currently over
		auto g = graphicAt(e.x, e.y);

		if (g != nullptr) {
			// If it's a sub-graphic, then continue down the chain
			MouseEvent newEvent = e;
			newEvent.x = (int)(e.x - g->getFrame().x);
			newEvent.y = (int)(e.y - g->getFrame().y);

			switch (e.activity) 
			{
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

			g->mouseEvent(newEvent);
		}

		// If we didn't land on anything, don't do anything

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
		//win->mousePressed(e);
	}

	/*
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
	*/

	//Handling Keyboard Events
	void keyboardEvent(const KeyboardEvent& e)
	{}


	void keyPressed(const KeyboardEvent& e)
	{}

	void keyReleased(const KeyboardEvent& e)
	{}

	void keyTyped(const KeyboardEvent& e)
	{}

	void fileDropped(const FileDropEvent& e)
	{
		auto win = graphicAt(e.x, e.y);
		if (win != nullptr)
			win->fileDrop(e);
	}
};