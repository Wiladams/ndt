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
	void addChild(std::shared_ptr<GraphicElement> child)
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





/*
class Graphic : public GraphicElement
{
protected:
	std::deque<std::shared_ptr<IGraphic> > fChildren{};
	std::shared_ptr<ILayoutGraphics> fLayout{};
	std::shared_ptr<IGraphic> fActiveGraphic{};
	
public:
	Graphic()
		:Graphic(0.0, 0, 0, 0)
	{}

	Graphic(const maths::bbox2f &gframe)
		:GraphicElement(gframe)
	{}

	Graphic(const float x, const float y, const float w, const float h)
		:GraphicElement(x, y, w, h)
	{
	}

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

	// Add a child to our set of children
	// If we're using layout, automatically set
	// the bounds to include the child's frame
	void addChild(std::shared_ptr<IGraphic> child)
	{
		fChildren.push_back(child);
		layout();
	}
	




	
	virtual bool keyEvent(const KeyboardEvent& e) override
	{
		printf("Graphic:keyEvent\n");
		bool handled = false;

		switch (e.activity)
		{
		case KEYPRESSED:
			handled = keyPressed(e);
			break;

		case KEYRELEASED:
			handled = keyReleased(e);
			break;

		case KEYTYPED:
			handled = keyTyped(e);
			break;
		}

		return handled;
	}

	virtual bool keyPressed(const KeyboardEvent& e) override
	{
		printf("Graphic:keyPressed\n");
		// do nothing
		return false;
	}

	virtual bool keyReleased(const KeyboardEvent& e) override
	{
		printf("Graphic:keyReleased\n");

		// do nothing
		return false;
	}

	virtual bool keyTyped(const KeyboardEvent& e) override
	{
		printf("Graphic:keyTyped\n");

		// do nothing
		return false;
	}
	

	// Handling mouse events
	void mouseEvent(const MouseEvent& e) override
	{
		printf("Graphic.mouseEvent: %d\n", e.activity);



		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		// 
		int tx = e.x + fTranslation.x;
		int ty = e.y - fTranslation.y;

		// Figure out which child the mouse pointer 
		// is currently over
		auto g = graphicAt(tx, ty);

		// translate according to the transformation
		//auto pt = fTransform.mapPoint(e.x, e.y);
		//auto newEvent(e);
		//newEvent.x = (int)(pt.x + fFrame.x);
		//newEvent.y = (int)(pt.y + fFrame.y);

		if (g != nullptr) {
			// If it's a sub-graphic, then continue down the chain
			MouseEvent newEvent = e;
			newEvent.x = (tx - g->frame().min.x);
			newEvent.y = (ty - g->frame().min.y);

			handled = g->mouseEvent(newEvent);
		}
		else {
			// If the mouse event didn't land on a child
			// then do our own processing
			switch (e.activity) {
			case MOUSEDRAGGED:
				handled = mouseDragged(e);
				break;

			case MOUSEPRESSED:
				handled = mousePressed(e);
				break;

			case MOUSEMOVED:
				handled = mouseMoved(e);
				break;

			case MOUSERELEASED:
				handled = mouseReleased(e);
				break;

			case MOUSEWHEEL:
				handled = mouseWheel(e);
				break;

			case MOUSEHWHEEL:
				handled = mouseHWheel(e);
				break;
			}
		}
		

	}

};
*/