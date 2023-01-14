#pragma once


#include "graphic.hpp"
#include "pubsub.h"
#include "FontFaceIcon.h"


//
// FontIconPage
// This page displays an icon for each of the fonts found in
// the familyNames() collection of the font manager.
//
// The page is a publishes of a std::string event, so that
// when you click on the icon, you can get notified. 
//
struct FontIconPage : public GraphicGroup, public Topic<std::string &>
{
	FontIconPage(float x, float y, float w, float h)
		:GraphicGroup(x,y,w,h)
	{
		auto layout = std::make_shared<VerticalGridLayout>(frame(), 8);

		setLayout(layout);

		auto & f = frame();

		// Create a button for each font face
		// and setup the callback notification
		for (auto& name : gFontHandler->familyNames())
		{
			auto icon = std::make_shared<FontFaceIcon>(name.c_str());
			icon->subscribe([this](std::string& name) { this->notify(name); });
			
			addGraphic(icon);
		}
	}
};