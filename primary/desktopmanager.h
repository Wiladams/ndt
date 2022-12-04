#pragma once

#include "apphost.h"
#include "graphic.hpp"


//
// DesktopManager
// 
// A class that implements desktop behavior
// this is primarily dispatching mouse, keyboard events
// but it's also about moving windows to the front
// switching between windows, and the like. 
//
struct DesktopBehavior 
{
	GraphicGroup& fGroup;
	std::shared_ptr<GraphicElement> fSelectedGraphic{};
	
	std::shared_ptr<GraphicElement> fHoverGraphic{};
	std::shared_ptr<GraphicElement> fHoverGraphicLast{};


	bool fIsDragging = false;

	DesktopBehavior(GraphicGroup& group)
		:fGroup(group)
	{
		fSelectedGraphic = nullptr;

		// Subscribe to the various events we care about 
		// We're subscribing at the application level
		subscribe([this](const MouseEvent& e) {mouseEvent(e); });
		subscribe([this](const KeyboardEvent& e) {keyboardEvent(e); });
	}

	auto selectedGraphic() { return fSelectedGraphic; }
	auto hoverGraphic() { return fHoverGraphic; }

	void keyboardEvent(const KeyboardEvent& e)
	{
		//printf("DesktopBehavior::keyboardEvent() %d\n", e.keyCode);
		if (e.keyCode == VK_ESCAPE)
			halt();
	}

	void mouseEvent(const MouseEvent& e)
	{
		// Mouse events are given in the coordinate space of the 
		// parent frame, so we want to convert to our local coordinate
		// space before doing anything else.
		// First subtract off the frame origin
		MouseEvent localEvent(e);
		localEvent.x = e.x - fGroup.frame().x;
		localEvent.y = e.y - fGroup.frame().y;

		//printf("DesktopBehavior::mouseEvent (%3.0f,%3.0f)\n", e.x,e.y);
		//printf("                            (%3.0f,%3.0f)\n", localEvent.x, localEvent.y);

		fHoverGraphic = fGroup.graphicAt(localEvent.x, localEvent.y);

		switch (e.activity)
		{
		case MOUSEPRESSED:
			fIsDragging = true;
			fSelectedGraphic = fHoverGraphic;
			if (fSelectedGraphic != nullptr)
			{
				fGroup.moveToFront(fSelectedGraphic);
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
				if (fHoverGraphic)
					fHoverGraphic->mouseEvent(localEvent);
			}
			break;

		default:
			if (fSelectedGraphic != nullptr)
				fSelectedGraphic->mouseEvent(localEvent);
			break;
		}


	}
};
