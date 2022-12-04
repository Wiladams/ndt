#pragma once


#include "FontFaceIcon.h"

struct VerticalGridLayout : public ILayoutGraphics
{
	static constexpr float columnGap = 20;

	maths::bbox2f fBoundary;
	float fBoundaryWidth = 0;

	VerticalGridLayout(maths::bbox2f boundary)
		:fBoundary(boundary)
	{
		auto sz = maths::size(fBoundary);
		fBoundaryWidth = sz.x;
	}

	virtual maths::bbox2f layout(std::deque<std::shared_ptr<GraphicElement> >& gs)
	{
		float xoffset = columnGap;
		float yoffset = columnGap;

		maths::bbox2f extent{};

		for (auto& g : gs)
		{
			g->moveTo(xoffset, yoffset);

			xoffset += (g->frameWidth() + columnGap);
			float lastX = fBoundaryWidth - g->frameWidth();
			if (xoffset > lastX)
			{
				xoffset = columnGap;
				yoffset += g->frameHeight() + columnGap;
			}

			maths::expand(extent, g->frame());
		}

		return extent;
	}

};

// We'll publish face names
struct FontIconPage : public GraphicGroup, public Topic<std::string &>
{
	FontIconPage(float x, float y, float w, float h)
		:GraphicGroup(x,y,w,h)
	{
		auto layout = std::make_shared<VerticalGridLayout>(frame());
		setLayout(layout);

		auto & f = frame();

		for (auto& name : gFontHandler->familyNames())
		{
			//printf("Name: %s\n", name.c_str());

			auto icon = std::make_shared<FontFaceIcon>(name.c_str());
			icon->subscribe(*this);

			addGraphic(icon);
		}
	}

	// Catch the clicks on specific icons.  
	// We'll get the name of the font family
	// We can re-publish if we want someone
	// to be watching and responding to, the 
	// FontIconPage.

	void operator() (const std::string& s)
	{
		//printf("FontIconPage; clicked on: %s\n", s.c_str());
		std::string newString(s);
		notify(newString);
	}
};