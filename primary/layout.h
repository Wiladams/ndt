/*
	Layout occurs with IGraphic objects.
	The layout is given a set of graphics
	and according to some constraint, lays the
	graphics out consecutively based on that constraint.

	This can be used to create horizontal, vertical, grid,
	or whatever is required.

	Note: Instead of hard coding a specific set kind
	we should templatize so that any collection kind can 
	be used.
*/
#pragma once


#include "graphic.hpp"
#include "geometry.h"

#include <deque>
#include <memory>
#include <functional>

/*
	GraphicCollectionHandler
	Given a collection of graphics, perform an operation on them
	while returning a bounds indicating the merged space they take up.

	Most typically, we'll just this to perform layout, but it could really 
	be used for anything related to a collection of grphics.
*/
using GraphicCollectionHandler = std::function<maths::bbox2f (std::deque<std::shared_ptr<GraphicElement> >& gs)>;


struct ILayoutGraphics 
{
	enum class LayoutAlignment : unsigned
	{
		NONE
		, CENTER
		, NEAREST
		, FARTHEST
	};

	enum class LayoutPlacement : unsigned
	{
		CENTER
		,TOP
		,RIGHT
		,BOTTOM
		,LEFT
	};

	// Given two frames, a primary, and a secondary, return 
	// a bbox that could be used to adjust the secondary frame
	// to achieve the desired alignment
	static maths::rectf alignFrame(const maths::rectf& pFrame, const maths::rectf& sFrame, 
		LayoutAlignment halign = LayoutAlignment::NEAREST,
		LayoutAlignment valign = LayoutAlignment::NEAREST)
	{
		maths::rectf aframe = sFrame;
		maths::vec2f dxy{};

		switch (halign) {
			case LayoutAlignment::CENTER: {
				dxy = maths::center(pFrame) - maths::center(sFrame);
				maths::moveBy(aframe, dxy.x, 0);
			}
			break;

			case LayoutAlignment::NEAREST: {
				float dx = maths::left(pFrame) - maths::left(sFrame);
				aframe.x += dx;
			}
			break;

			case LayoutAlignment::FARTHEST: {
				float dx = maths::right(pFrame) - maths::right(sFrame);
				aframe.x += dx;
			}
			break;

			default:	// do nothing
				;
		};

		switch(valign){
			case LayoutAlignment::CENTER: {
				dxy = maths::center(pFrame) - maths::center(sFrame);
				maths::moveBy(aframe, 0, dxy.y);
			}
			break;

			case LayoutAlignment::NEAREST: {
				float dy = maths::top(pFrame) - maths::top(sFrame);
				aframe.y += dy;
			}
			break;

			case LayoutAlignment::FARTHEST: {
				float dy = maths::bottom(pFrame) - maths::bottom(sFrame);
				aframe.y += dy;
			}
			break;

			default:	// do nothing
				;
		}

		return aframe;
	}


	virtual ~ILayoutGraphics() {}

	virtual maths::rectf layout(std::deque<std::shared_ptr<GraphicElement> >& gs) = 0;

};

// different kinds of layout handlers.
// They are implemented as functors, which will 
// do the layout, and return a boundary structure
// indicating the extent of the graphics.
struct IdentityLayout : public ILayoutGraphics
{
	virtual ~IdentityLayout() {};

	maths::rectf layout(std::deque<std::shared_ptr<GraphicElement> > &gs) override
	{
		maths::rectf bounds;

		// traverse the graphics
		// expand bounding box to include their frames, without alternation
		for (auto& g : gs)	// std::shared_ptr<IGraphic> g
		{
			maths::expand(bounds, g->frame());
		}
		
		// return the expanded bounding box
		return bounds;
	}

	maths::rectf operator()(std::deque<std::shared_ptr<GraphicElement> >& gs)
	{
		return layout(gs);
	}

};

struct BinaryLayout : ILayoutGraphics
{
public:


private:
	LayoutPlacement fAlign = LayoutPlacement::CENTER;
	float fGap = 0;

public:
	BinaryLayout(const LayoutPlacement align= LayoutPlacement::CENTER, const float gap=0)
		:fAlign(align)
		,fGap(gap)
	{
	}

	virtual ~BinaryLayout() { ; }

	maths::rectf layout(std::deque<std::shared_ptr<GraphicElement> > &gs) override
	{
		maths::rectf bds{};

		// There must be at least two graphics in the vector
		// only the first two will be part of the layout
		if (gs.size() < 2)
			return bds;

		auto & primary = gs[0];
		maths::rectf pFrame = primary->frame();
		auto & secondary = gs[1];
		maths::rectf sFrame = secondary->frame();

		switch (fAlign) {
		case LayoutPlacement::CENTER: {
			// use center of primary to be center of secondary
			auto pCenter = maths::center(pFrame);
			auto sCenter = maths::center(sFrame);

			auto dxy = sCenter - pCenter;

			maths::moveBy(sFrame, dxy);
		}
			break;

		case LayoutPlacement::TOP:
			break;

		case LayoutPlacement::RIGHT:
			break;

		case LayoutPlacement::BOTTOM:
			break;

		case LayoutPlacement::LEFT:
			break;
		}

		secondary->moveTo(sFrame.x, sFrame.y);

		return bds;
	}
};

class CascadeLayout : public ILayoutGraphics
{
	static constexpr float leftMargin = 10;
	static constexpr float topMargin = 49;
	static constexpr float verticalGap = 8;

	// Where is the next graphic about to be placed
	float wX = leftMargin;
	float wY = topMargin;

	// Constraints of the boundaries
	int fCanvasWidth;
	int fCanvasHeight;

	int fVerticalOffset = 10;
	int fHorizontalOffset = -10;


public:
	CascadeLayout(int w, int h)
	{
		fCanvasWidth = w;
		fCanvasHeight = h;

		reset();
	}

	void reset()
	{
		wX = leftMargin;
		wY = topMargin;
	}

	virtual void addGraphic(std::shared_ptr<GraphicElement> gr, maths::rectf&b)
	{
		// Move the graphic to the next position
		gr->moveTo(wX, wY);
		maths::expand(b, gr->frame());

		// Calculate what will be the next position
		maths::vec2f sz = maths::size(gr->frame());
		wX += (int)(sz.x + fHorizontalOffset);
		wY += (int)fVerticalOffset;

		if (wX > fCanvasWidth - 256)
		{
			wX = leftMargin;
			wY += 256 + verticalGap;
		}
	}

	// Perform layout starting from scratch
	maths::rectf layout(std::deque<std::shared_ptr<GraphicElement> > &gs) override
	{
		reset();
		maths::rectf b{};

		for (auto & g : gs)	// std::shared_ptr<IGraphic> g
		{
			addGraphic(g, b);
			/*
			int offsetx = x;
			int offsety = y;

			x += (int)(g->frame().w + fHorizontalOffset);
			y += fVerticalOffset;

			if (x > fWidth - 256) {
				x = 10;
				y += 256 + fVerticalGap;
			}

			g->moveTo(offsetx, offsety);
			*/
		}

		return b;
	}
};


class HorizontalLayout : public ILayoutGraphics
{
	// While doing layout, these tell us
	// where the next coordinate should be
	float xOffset = 0;
	float yOffset = 0;

	float maxX = 0;
	float maxY = 0;

	// Imposed extent limits
	float fCanvasExtent{};		// Maximum allowable width of row

	// Gap between elements
	float fHorizontalGap = 8;
	float fVerticalGap = 8;


public:
	HorizontalLayout()
	{
	}

	HorizontalLayout(float w)
		:fCanvasExtent{w}
	{
	}

	virtual void reset()
	{
		xOffset = 0;
		yOffset = 0;

		maxX = 0;
		maxY = 0;
	}

	virtual void addGraphic(std::shared_ptr<GraphicElement> gr, maths::rectf&b)
	{
		// Move the graphic to the current specified location
		gr->moveTo(xOffset, yOffset);
		maths::expand(b, gr->frame());

		// Calculate next position
		maxX = gr->frameX() + gr->frameWidth();
		maxY = maths::max(maxY, gr->frameY() + gr->frameHeight());

		// Advance to the next column position
		xOffset = maxX + fHorizontalGap;

		// Move to the next row if we've gone beyond
		// the horizontal constraint
		if (xOffset >= fCanvasExtent)
		{
			xOffset = 0;
			yOffset = maxY + fVerticalGap;

			maxX = xOffset;
			maxY = yOffset;
		}
	}

	// Perform layout starting from scratch
	maths::rectf layout(std::deque<std::shared_ptr<GraphicElement> > &gs) override
	{
		reset();
		maths::rectf b{};

		for (auto & g : gs)
		{
			addGraphic(g, b);
		}

		return b;
	}
};

// Single column, vertical layout
// Also known as VerticalLayout
class ColumnLayout : public ILayoutGraphics
{
	float xOffset = 0;
	float yOffset = 0;

	float fHExtent = 0;
	LayoutAlignment fHAlign;	// horizontal alignment within column
	// Gap between elements
	float fGap = 8;


public:
	ColumnLayout(const float hExtent, const LayoutAlignment halign=LayoutAlignment::NEAREST , const float gap=8)
		:fHExtent(hExtent)
		,fHAlign(halign)
		,fGap(gap)
	{}

	virtual void reset()
	{
		xOffset = 0;
		yOffset = 0;
	}

	virtual void addGraphic(std::shared_ptr<GraphicElement> gr, maths::rectf&b)
	{
		// calculate box based on hextent, graphic size, and current position
		maths::rectf box = { xOffset,yOffset,fHExtent,gr->frameHeight()};
		maths::rectf aframe = ILayoutGraphics::alignFrame(box, gr->frame(),
			fHAlign, ILayoutGraphics::LayoutAlignment::NEAREST);

		// Move to next location
		gr->moveTo(aframe.x,aframe.y);
		//gr->moveTo(xOffset, yOffset);

		// expand the bounds to include the new frame
		maths::expand(b, gr->frame());

		// calculate where next position should be
		yOffset = maths::right(gr->frame()) + fGap;
	}

	// Perform layout starting from scratch
	maths::rectf layout(std::deque<std::shared_ptr<GraphicElement> > &gs) override
	{
		reset();

		maths::rectf b{};

		for (auto & g : gs)
		{
			addGraphic(g, b);
		}

		return b;
	}
};

struct VerticalGridLayout : public ILayoutGraphics
{
	static constexpr float columnGap = 20;

	maths::rectf fBoundary;
	float fBoundaryWidth = 0;
	float fColumnGap = columnGap;

	VerticalGridLayout(const maths::rectf& boundary, float gap = columnGap)
		:fBoundary(boundary),
		fColumnGap(gap)
	{
		fBoundaryWidth = boundary.w;
	}

	virtual maths::rectf layout(std::deque<std::shared_ptr<GraphicElement> >& gs)
	{
		float xoffset = columnGap;
		float yoffset = columnGap;

		maths::rectf extent{};

		for (auto& g : gs)
		{
			g->moveTo(xoffset, yoffset);

			xoffset += (g->frameWidth() + fColumnGap);
			float lastX = fBoundaryWidth - g->frameWidth();
			if (xoffset > lastX)
			{
				xoffset = columnGap;
				yoffset += g->frameHeight() + fColumnGap;
			}

			maths::expand(extent, g->frame());
		}

		return extent;
	}

};
