#pragma once

#include "drawable.h"
#include "layout.h"

#include <memory>
#include <vector>
#include <deque>



struct GraphicGroup : public GraphicElement
{
	std::shared_ptr<ILayoutGraphics> fLayout{};
	std::shared_ptr<GraphicElement> fActiveGraphic{};
	std::deque<std::shared_ptr<GraphicElement> > fChildren{};

	GraphicGroup(const maths::bbox2f& aframe)
		:GraphicElement(aframe)
	{}

	void setLayout(std::shared_ptr<ILayoutGraphics> layit)
	{
		fLayout = layit;
	}

	virtual void layout()
	{
		if (nullptr != fLayout) {
			auto b = fLayout->layout(fChildren);
			setBounds(b);
		}
	}

	// Find the topmost window at a given position
	virtual std::shared_ptr<GraphicElement> graphicAt(float x, float y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<std::shared_ptr<GraphicElement> >::reverse_iterator rit = fChildren.rbegin();
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
		for (auto& child : fChildren)
		{
			maths::expand(fFrame, child->frame());
		}

		return fFrame;
	}

	// Add a child to our set of children
	// If we're using layout, automatically set
	// the bounds to include the child's frame
	void addGraphic(std::shared_ptr<GraphicElement> child)
	{
		fChildren.push_back(child);
		layout();
	}

	// Move a specific graphic to the front visually
	void moveToFront(std::shared_ptr<GraphicElement> g)
	{
		std::deque<std::shared_ptr<GraphicElement> >::iterator it = fChildren.begin();
		for (it = fChildren.begin(); it != fChildren.end(); ++it)
		{
			if (*it == g) {
				fChildren.erase(it);
				fChildren.push_back(g);
				break;
			}
		}
	}

	virtual void drawSelf(IGraphics& ctx)
	{
		for (auto& g : fChildren)
		{
			// BUGBUG - really there shouldn't be any nullptr graphics
			if (nullptr == g)
				continue;

			g->draw(ctx);
		}
	}
	/*
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
};





