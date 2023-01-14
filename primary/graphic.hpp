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
	std::deque<std::shared_ptr<IDrawable> > fDrawables{};
	
	GraphicGroup()
		:GraphicElement()
	{;}
	
	GraphicGroup(float x, float y, float w, float h)
		:GraphicElement(x,y,w,h)
	{
		;
	}

	GraphicGroup(const maths::rectf& aframe)
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

	maths::rectf calculateExtent()
	{
		maths::rectf ext{};

		for (auto& child : fChildren)
		{
			maths::expand(ext, child->frame());
		}

		return ext;
	}

	// Add a child to our set of children
	// If we're using layout, automatically set
	// the bounds to include the child's frame
	void addGraphic(std::shared_ptr<GraphicElement> child)
	{
		fChildren.push_back(child);
		layout();
	}

	void clearDrawables()
	{
		fDrawables.clear();
	}
	
	void addDrawable(std::shared_ptr<IDrawable> drawable)
	{
		fDrawables.push_back(drawable);
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

	virtual void drawDrawables(IGraphics& ctx)
	{
		for (auto& d : fDrawables)
		{
			d->draw(ctx);
		}
	}
	
	virtual void drawSelf(IGraphics& ctx)
	{
		// draw the child graphics
		for (auto& g : fChildren)
		{
			// BUGBUG - really there shouldn't be any nullptr graphics
			if (nullptr == g)
				continue;

			g->draw(ctx);
		}

		drawDrawables(ctx);
	}

	void mouseEvent(const MouseEvent& e) override
	{
		//printf("GraphicGroup:mouseEvent: %d\n", e.activity);
		
		MouseEvent lev(e);
		lev.x = e.x - frameX();
		lev.y = e.y - frameY();
		
		// adjust for boundary translation
		// BUGBUG - this should be general transformation, including scaling
		lev.x += fTranslation.x;
		lev.y += -fTranslation.y;
		
		//printf("mouseEvent: e.xy: %f %f, lev.xy: %f %f\n", e.x, e.y, lev.x, lev.y);
		
		// figure out which graphic the mouse is over
		auto g = graphicAt(lev.x, lev.y);

		if (g != fActiveGraphic)
		{
			// tell the active graphic it's no longer
		}
		
		if (g != nullptr)
		{
			fActiveGraphic = g;

			g->mouseEvent(lev);
		}
	}
};





