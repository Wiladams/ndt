#pragma once

#include "graphic.hpp"
#include "svgwindow.h"

struct SVGIconPage : public GraphicGroup, public Topic<std::shared_ptr<svg::SVGDocument>>
{
	SVGIconPage(float x, float y, float w, float h)
		:GraphicGroup(x, y, w, h)
	{
		auto layout = std::make_shared<VerticalGridLayout>(frame(), 8);
		setLayout(layout);
	}

	// Accept a drop of files
	void fileDrop(const FileDropEvent& e)
	{
		// assuming there's at least one file that 
		// has been dropped.
		for (int i = 0; i < e.filenames.size(); i++)
		{
			// Create a new SVGDocument for each file
			// And create a window to display each document
			auto doc = svg::SVGDocument::createFromFilename(e.filenames[i]);

			if (doc != nullptr)
			{
				auto win = std::make_shared<SVGIcon>(0,0,100,100, doc);
				win->setBackgroundColor(Pixel(255, 255, 255, 255));
				//win->document(doc);
				win->setMoveable(false);
				win->subscribe([this](std::shared_ptr<svg::SVGDocument> doc) { this->notify(doc); });

				addGraphic(win);
			}
		}
	}

	
};