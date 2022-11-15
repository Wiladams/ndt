#pragma once

#include "apphost.h"
#include "p5.hpp"
#include "gwindow.h"

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
	std::shared_ptr<IGraphic> fHoverGraphic{};

public:
	WindowManager(float w, float h)
		:Graphic(0,0,w,h)
	{
		setLayout(std::make_shared<IdentityLayout>());

	}

	void setActiveGraphic(std::shared_ptr<IGraphic> g) override
	{
		// if the activegraphic is already the same as the
		// one we're trying to set, don't do anything
		if (fActiveGraphic == g)
			return;


		if (fActiveGraphic != nullptr)
		{
			MouseEvent e{};
			fActiveGraphic->mouseExited(e);
		}

		fActiveGraphic = g;

		if (fActiveGraphic != nullptr)
		{
			MouseEvent e{};
			fActiveGraphic->mouseEntered(e);
		}
	}

	// Handling Keyboard Events
	// If there's anything special that a window manager
	// wants to do with keyboard events, here is where 
	// to do it.
	// In general, we'll just pass the event to the currently
	// active graphic, if there is one.
	// If there is no active graphic, then the event is just ignored
	void keyEvent(const KeyboardEvent& e) override
	{
		if (fActiveGraphic != nullptr)
			fActiveGraphic->keyEvent(e);
	}


	// Handling mouse events
	// There's a lot to be done here
	// 1) Move focus between windows
	// 2) Set active window
	// 3) Move window forward if necessary
	// 4) Pass events along to the active window
	void mouseEvent(const MouseEvent& e) override
	{
		//printf("WindowManager.mouseEvent: (%d,%d) - %d\n", e.x, e.y, e.activity);
		
		// First
		// Figure out which child the mouse pointer 
		// is currently over
		fHoverGraphic = graphicAt(e.x, e.y);

		// If the graphic is not null
		// then we need to figure out whether
		// it's the same as the currently active graphic
		// or a new graphic
		// and take appropriate actions
		if (fHoverGraphic != nullptr)
		{
			// First item of business is to adjust the location
			// of the event to take account of the graphic's frame
			MouseEvent newEvent(e);
			newEvent.x = (e.x - fHoverGraphic->frameX());
			newEvent.y = (e.y - fHoverGraphic->frameY());

			// If the graphic is the same as the currently active graphic
			// just forward the event, without changing anyting
			if (fHoverGraphic == fActiveGraphic)
			{
				fHoverGraphic->mouseEvent(newEvent);
			}
			else {
				// The graphic is not the same as the active graphic
				// tell the existing active graphic the mouse has exited
				
				// If the mouse event is a mousePressed(), then 
				// bring the graphic forward and make it the active graphic
				// and pass event along
				if (e.activity == MOUSEPRESSED)
				{
					moveToFront(fHoverGraphic);
					setActiveGraphic(fHoverGraphic);

					// Send it the mouse event?
					fHoverGraphic->mouseEvent(newEvent);
				}
			}
		}
		else {
			// In this case, things are happening in the open window space
			// so, deactivate active graphic if there event is a mousePressed
			// otherwise, just do nothing
			if (e.activity == MOUSEPRESSED)
				setActiveGraphic(nullptr);
		}

	}

	/*
	void mousePressed(const MouseEvent& e) override
	{
		// Figure out which window is being 
		// clicked
		auto win = graphicAt(e.x, e.y);
		//auto oldGraphic = fActiveGraphic;

		// if not clicked on a view, then simply return
		if (nullptr == win) {
			// BUGBUG - before moving on, the currently active
			// graphic should be told it is no longer the
			// focus
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
	*/

	void fileDrop(const FileDropEvent& e) override
	{
		auto win = graphicAt(e.x, e.y);
		if (win != nullptr)
			win->fileDrop(e);
	}
};