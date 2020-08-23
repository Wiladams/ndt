#pragma once

#include "graphic.hpp"
#include <memory>

/*
	given a vector of graphics, perform
	whatever layout operation we need to
*/
class ILayoutGraphics {
public:

	virtual void layout(std::vector<std::shared_ptr<IGraphic> > gs)
	{
		// by default, do nothing
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

	double fGap;
	BinaryAlignment fAlign;

public:

	BinaryLayout(std::shared_ptr<IGraphic> primary, 
		std::shared_ptr<IGraphic> secondary,
		BinaryAlignment align, 
		double gap)
		: fAlign(align),
		fGap(gap)
	{
		std::vector<std::shared_ptr<IGraphic> > gs{
			primary,
			secondary
		};

		layout(gs);
	}

	BinaryLayout(BinaryAlignment align, double gap)
		:fGap(gap),
		fAlign(align)
	{
	}

	virtual void layout(std::vector<std::shared_ptr<IGraphic> > gs)
	{
		// There must be at least two graphics in the vector
		// only the first two will be part of the layout
		if (gs.size() < 2)
			return;

		auto primary = gs[0];
		auto pFrame = primary->getFrame();
		auto secondary = gs[1];
		auto sFrame = secondary->getFrame();

		switch (fAlign) {
		case BinaryAlignment::CENTER: {
			// use center of primary to be center of secondary
			double pCenterX = pFrame.x + pFrame.w / 2.0;
			double pCenterY = pFrame.y + pFrame.h / 2.0;
			sFrame.x = pCenterX - sFrame.w / 2.0;
			sFrame.y = pCenterY - sFrame.h / 2.0;
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

		secondary->moveTo(sFrame.x, sFrame.y);
	}
};