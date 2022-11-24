#pragma once

#include "apphost.h"
#include "graphic.hpp"

struct GraphicGroup : public IDrawable
{
	maths::bbox2f fFrame{};
	std::deque<std::shared_ptr<IGraphic> > fChildren{};

	GraphicGroup(const maths::bbox2f& aframe)
		:fFrame(aframe)
	{}

	// Find the topmost window at a given position
	virtual std::shared_ptr<IGraphic> graphicAt(float x, float y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<std::shared_ptr<IGraphic> >::reverse_iterator rit = fChildren.rbegin();
		for (rit = fChildren.rbegin(); rit != fChildren.rend(); ++rit)
		{
			if ((*rit) != nullptr)
			{
				if ((*rit)->contains(x, y))
					return *rit;
			}
			else {
				//printf("GraphicAt() - contains null reference\n");
			}
		}

		//printf("Graphic::graphicAt(NULL)\n");

		return nullptr;
	}

	maths::bbox2f culateExtent()
	{
		fFrame = {};
		for (auto &child : fChildren)
		{
			maths::expand(fFrame, child->frame());
		}

		return fFrame;
	}

	// Add a child to our set of children
	// If we're using layout, automatically set
	// the bounds to include the child's frame
	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
		maths::expand(fFrame, child->frame());
	}

	// Move a specific graphic to the front visually
	void moveToFront(std::shared_ptr<IGraphic> g)
	{
		std::deque<std::shared_ptr<IGraphic> >::iterator it = fChildren.begin();
		for (it = fChildren.begin(); it != fChildren.end(); ++it)
		{
			if (*it == g) {
				fChildren.erase(it);
				fChildren.push_back(g);
				break;
			}
		}
	}

	virtual void drawChildren(IGraphics& ctx)
	{
		for (auto& g : fChildren)
		{
			// BUGBUG - really there shouldn't be any nullptr graphics
			if (nullptr == g)
				continue;

			g->draw(ctx);
		}
	}

	void draw(IGraphics& ctx) override
	{
		// Start by saving the context state
		// so we're free to mess around with it
		// while we're drawing ourself.
		ctx.push();

		// Before we do anything else, and while we're still
		// in the coordinate system of our parent, we want to setup 
		// a clip for our frame.
		// Once the clip is set, we want to transform our
		// coordinate sytem to have 0,0 be at the upper left corner.
		//ctx.clip(frame());

		// Apply user specified transform

		drawChildren(ctx);

		//ctx.noClip();
		ctx.pop();

	}

};

//
// DesktopManager
// 
// A class that implements desktop behavior
// this is primarily dispatching mouse, keyboard events
// but it's also about moving windows to the front
// switching between windows, and the like. 
//
struct DesktopManager : public GraphicGroup
{
	//maths::bbox2f fFrame{};
	//std::deque<std::shared_ptr<IGraphic> > fChildren{};
	std::shared_ptr<IGraphic> fSelectedGraphic{};
	bool fIsDragging = false;

	DesktopManager(const maths::bbox2f& aframe)
		:GraphicGroup(aframe)
	{
		fSelectedGraphic = nullptr;

		// Subscribe to the various events we care about 
		// We're subscribing at the application level
		subscribe([this](const MouseEvent& e) {mouseEvent(e); });
		subscribe([this](const KeyboardEvent& e) {keyboardEvent(e); });
	}
/*
	// Add a child to our set of children
	// If we're using layout, automatically set
	// the bounds to include the child's frame
	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
	}

	// Find the topmost window at a given position
	virtual std::shared_ptr<IGraphic> graphicAt(float x, float y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<std::shared_ptr<IGraphic> >::reverse_iterator rit = fChildren.rbegin();
		for (rit = fChildren.rbegin(); rit != fChildren.rend(); ++rit)
		{
			if ((*rit) != nullptr)
			{
				if ((*rit)->contains(x, y))
					return *rit;
			}
			else {
				//printf("GraphicAt() - contains null reference\n");
			}
		}

		//printf("Graphic::graphicAt(NULL)\n");

		return nullptr;
	}

	void moveToFront(std::shared_ptr<IGraphic> g)
	{
		std::deque<std::shared_ptr<IGraphic> >::iterator it = fChildren.begin();
		for (it = fChildren.begin(); it != fChildren.end(); ++it)
		{
			if (*it == g) {
				fChildren.erase(it);
				fChildren.push_back(g);
				break;
			}
		}
	}

	virtual void drawChildren(IGraphics& ctx)
	{
		for (auto& g : fChildren)
		{
			// BUGBUG - really there shouldn't be any nullptr graphics
			if (nullptr == g)
				continue;

			g->draw(ctx);
		}
	}

	void draw(IGraphics& ctx) override
	{
		// Start by saving the context state
		// so we're free to mess around with it
		// while we're drawing ourself.
		ctx.push();

		// Before we do anything else, and while we're still
		// in the coordinate system of our parent, we want to setup 
		// a clip for our frame.
		// Once the clip is set, we want to transform our
		// coordinate sytem to have 0,0 be at the upper left corner.
		//ctx.clip(frame());

		// Apply user specified transform

		drawChildren(ctx);

		//ctx.noClip();
		ctx.pop();

	}
	*/
	void keyboardEvent(const KeyboardEvent& e)
	{
		//printf("DesktopManager::keyboardEvent() %d\n", e.keyCode);
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
		localEvent.x = e.x - fFrame.min.x;
		localEvent.y = e.y - fFrame.min.y;

		//printf("DesktopBehavior::mouseEvent (%3.0f,%3.0f)\n", e.x,e.y);
		//printf("                            (%3.0f,%3.0f)\n", localEvent.x, localEvent.y);

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

		}
	}
};
