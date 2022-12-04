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

class WindowManager : public GraphicGroup
{
protected:
	bool fIsDragging{ false };
	std::shared_ptr<GraphicElement> fSelectedGraphic{ nullptr };

public:
	WindowManager(float w, float h)
		:GraphicGroup(0,0,w,h)
	{
		setLayout(std::make_shared<IdentityLayout>());

		// Setup for mousedispatcher
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
		{
			//printf("WindoManager:keyEvent\n");
			return fActiveGraphic->keyEvent(e);
		}

	}

	// Handling mouse events
	// There's a lot to be done here
	// 1) Move focus between windows
	// 2) Set active window
	// 3) Move window forward if necessary
	// 4) Pass events along to the active window
	void mouseEvent(const MouseEvent& e) override
	{
		//printf("WindowManager.mouseEvent: (%3.0f,%3.0f) - %d\n", e.x, e.y, e.activity);

		// Mouse events are given in the coordinate space of the 
		// parent frame, so we want to convert to our local coordinate
		// space before doing anything else.
		// First subtract off the frame origin
		MouseEvent localEvent(e);
		localEvent.x = e.x - frameX();
		localEvent.y = e.y - frameY();

		auto hovered = graphicAt(localEvent.x, localEvent.y);


		switch (e.activity)
		{
		case MOUSEPRESSED:
			fIsDragging = true;
			fSelectedGraphic = hovered;
			if (fSelectedGraphic != nullptr)
			{
				moveToFront(fSelectedGraphic);
				fSelectedGraphic->mouseEvent(localEvent);
			}
			break;

		case MOUSERELEASED:
			fIsDragging = false;
			if (fSelectedGraphic != nullptr)
			{
				fSelectedGraphic->mouseEvent(localEvent);
			}
			break;

		case MOUSEMOVED:
			// If dragging, keep sending to the original 
			// graphic, even if we're beyond its bounds
			if (fIsDragging)
			{
				if (fSelectedGraphic != nullptr)
					fSelectedGraphic->mouseEvent(localEvent);
			}
			else {
				if (hovered)
					hovered->mouseEvent(localEvent);
			}
			break;

		default:
			if (fSelectedGraphic != nullptr)
				fSelectedGraphic->mouseEvent(localEvent);
			break;
		}

	}
	/*
	bool mouseMoved(const MouseEvent& e) override
	{
		bool handled = false;

		// Figure out which child the mouse pointer 
		// is currently over, if any
		auto hover = graphicAt(e.x, e.y);

		if (fActiveGraphic != nullptr)
		{
			MouseEvent activeEvent(e);
			activeEvent.x = (e.x - fActiveGraphic->frameX());
			activeEvent.y = (e.y - fActiveGraphic->frameY());

			if (fActiveGraphic == hover)
			{
				handled = fActiveGraphic->mouseEvent(activeEvent);
				return handled;
			}

			// Movement between graphics
			// mouse exited for the active graphic
			fActiveGraphic->mouseExited(e);
		}

		if (hover != nullptr)
		{
			// We're not mouse down, so we don't change the active graphic
			// but we do let the hovered graphic receive the event
			// BUGBUG - If we want the active graphic to also receive these
			// events, we would send them here.
			// BUGBUG - mouseEntered should also happen here, but we would
			// need to track the last hover to do that
			MouseEvent hoverEvent(e);
			hoverEvent.x = (e.x - hover->frameX());
			hoverEvent.y = (e.y - hover->frameY());
			handled = hover->mouseEvent(hoverEvent);
		}

		return handled;
	}

	bool mousePressed(const MouseEvent& e) override
	{
		bool handled = false;

		// Figure out which child the mouse pointer 
		// is currently over, if any
		auto hover = graphicAt(e.x, e.y);

		if (fActiveGraphic != nullptr)
		{
			// Active and hover are the same
			// so just send the event to the active graphic
			// and be done.
			if (fActiveGraphic == hover)
			{
				MouseEvent activeEvent(e);
				activeEvent.x = (e.x - fActiveGraphic->frameX());
				activeEvent.y = (e.y - fActiveGraphic->frameY());
				handled = fActiveGraphic->mouseEvent(activeEvent);
				return handled;
			}

			if (hover == nullptr)
			{
				fActiveGraphic->mouseExited(e);

				fActiveGraphic = nullptr;

				return true;
			}
			else {
				fActiveGraphic->mouseExited(e);
			}

		}

		// If hover is different than active graphic
		// Tell active graphic mouseExited
		if (hover != nullptr)
		{
			// Tell hover mouse Entered
			hover->mouseEntered(e);

			// Send mouse activity to hover
			MouseEvent hoverEvent(e);
			hoverEvent.x = (e.x - hover->frameX());
			hoverEvent.y = (e.y - hover->frameY());
			handled = hover->mouseEvent(hoverEvent);

			// Make hover active
			fActiveGraphic = hover;

			// bring it to the front
			moveToFront(hover);
		}

		return handled;
	}
	*/
	void fileDrop(const FileDropEvent& e) override
	{
		auto g = graphicAt(e.x, e.y);
		if (g != nullptr)
			g->fileDrop(e);
	}
};