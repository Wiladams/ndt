#pragma once


#include "FontFaceIcon.h"

// We'll publish face names
struct FontIconPage : public Graphic, public Topic<std::string &>
{
private:
	static maths::vec2f getPreferredSize() { return {160*8,-1}; }

	//FontIconPage(const FontIconPage&) = delete;

public:
	FontIconPage(float x, float y, float w, float h)
		:Graphic(x,y,w,h)
	{
		constexpr float columnGap = 20;


		float xoffset = columnGap;
		float yoffset = columnGap;

		auto f = frame();

		for (auto& name : gFontHandler->familyNames())
		{
			//printf("Name: %s\n", name.c_str());

			auto icon = std::make_shared<FontFaceIcon>(name.c_str());
			icon->subscribe(*this);
			icon->moveTo(xoffset, yoffset);

			addChild(icon);

			xoffset += (icon->frame().w + columnGap);
			float lastX = f.w - icon->frame().w;
			if (xoffset > lastX)
			{
				xoffset = columnGap;
				yoffset += icon->frame().h+columnGap;
			}

		}

		setBounds(BLRect(0, 0, fBounds.w, yoffset + columnGap));
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