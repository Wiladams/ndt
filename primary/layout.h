#pragma once

#include "drawable.h"
#include "geometry.h"

#include <deque>
#include <memory>

/*
	given a set of graphics, perform
	whatever layout operation we need to
*/
struct ILayoutGraphics 
{
	virtual ~ILayoutGraphics() {}

	virtual maths::bbox2f layout(std::deque<std::shared_ptr<IGraphic> >& gs) = 0;

};

struct IdentityLayout : public ILayoutGraphics
{
	virtual ~IdentityLayout() {};

	maths::bbox2f layout(std::deque<std::shared_ptr<IGraphic> > &gs) override
	{
		maths::bbox2f bounds;

		// traverse the graphics
		// expand bounding box to include their frames, without alternation
		for (auto& g : gs)	// std::shared_ptr<IGraphic> g
		{
			maths::expand(bounds, g->frame());
		}
		
		// return the expanded bounding box
		return bounds;
	}
};

class BinaryLayout : ILayoutGraphics
{

	enum class BinaryAlignment : unsigned
	{
		CENTER,
		TOP,
		RIGHT,
		BOTTOM,
		LEFT
	};

	BinaryAlignment fAlign = BinaryAlignment::RIGHT;
	//float fGap = 0;

public:
	/*
	BinaryLayout(std::shared_ptr<IGraphic> primary, 
		std::shared_ptr<IGraphic> secondary,
		BinaryAlignment align, 
		double gap)
		: fAlign(align)
	{
		std::deque<std::shared_ptr<IGraphic> > gs{
			primary,
			secondary
		};

		//layout(gs);
	}
	*/
	BinaryLayout(BinaryAlignment align, double gap)
		:fAlign(align)
		//,fGap(gap)
	{
	}

	virtual ~BinaryLayout() {}

	maths::bbox2f layout(std::deque<std::shared_ptr<IGraphic> > &gs) override
	{
		maths::bbox2f bounds{};

		// There must be at least two graphics in the vector
		// only the first two will be part of the layout
		if (gs.size() < 2)
			return bounds;

		auto & primary = gs[0];
		maths::bbox2f pFrame = primary->frame();
		auto & secondary = gs[1];
		maths::bbox2f sFrame = secondary->frame();

		switch (fAlign) {
		case BinaryAlignment::CENTER: {
			// use center of primary to be center of secondary
			auto pCenter = maths::center(pFrame);
			auto sCenter = maths::center(sFrame);

			auto dxy = sCenter - pCenter;

			sFrame.min += dxy;
			sFrame.max += dxy;
		}
			break;

		case BinaryAlignment::TOP:
			break;

		case BinaryAlignment::RIGHT:
			break;

		case BinaryAlignment::BOTTOM:
			break;

		case BinaryAlignment::LEFT:
			break;
		}

		secondary->moveTo(sFrame.min.x, sFrame.min.y);

		return bounds;
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

	virtual void addGraphic(std::shared_ptr<IGraphic> gr, maths::bbox2f &b)
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
	maths::bbox2f layout(std::deque<std::shared_ptr<IGraphic> > &gs) override
	{
		reset();
		maths::bbox2f b{};

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

	virtual void addGraphic(std::shared_ptr<IGraphic> gr, maths::bbox2f &b)
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
	maths::bbox2f layout(std::deque<std::shared_ptr<IGraphic> > &gs) override
	{
		reset();
		maths::bbox2f b{};

		for (std::shared_ptr<IGraphic> g : gs)
		{
			addGraphic(g, b);
		}

		return b;
	}
};

// Single column, vertical layout
class ColumnLayout : public ILayoutGraphics
{
	float xOffset = 0;
	float yOffset = 0;

	//float maxX = 0;
	//float maxY = 0;

	// Gap between elements
	float fGap = 8;


public:
	ColumnLayout(float gap=8)
		:fGap(gap)
	{}

	virtual void reset()
	{
		xOffset = 0;
		yOffset = 0;
	}

	virtual void addGraphic(std::shared_ptr<IGraphic> gr, maths::bbox2f &b)
	{
		// Move to next location
		gr->moveTo(xOffset, yOffset);

		// expand the bounds to include the new frame
		maths::expand(b, gr->frame());

		// calculate where next position should be
		yOffset = gr->frame().max.y + fGap;
	}

	// Perform layout starting from scratch
	maths::bbox2f layout(std::deque<std::shared_ptr<IGraphic> > &gs) override
	{
		reset();

		maths::bbox2f b{};

		for (std::shared_ptr<IGraphic> g : gs)
		{
			addGraphic(g, b);
		}

		return b;
	}
};

