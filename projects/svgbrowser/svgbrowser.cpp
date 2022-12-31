#include "p5.hpp"

#include "svgscanner.h"
#include "xmlscan.h"
#include "mmap.hpp"


using namespace p5;
using namespace ndt;
using namespace ndt_debug;
using namespace svg;


void setup()
{
	//createCanvas(1024, 768);
	fullscreen();

	//testShape();
	
	dropFiles();	// allow dropping of files

	// Use a cascading window layout
	std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	windowLayout(layout);

	frameRate(30);
}

void draw()
{

	// Clear to background, whatever it is
	if (!isLayered())
		background(0);	// (245 , 246, 247);
	else
		clear();

	//background(255);

	
	noStroke();
	fill(0x7f);
	rect(0, 0, canvasWidth, 48);

	// Put some instructional text up there
	fill(0);
	textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
	textSize(36);
	textFont("Consolas");
	text("DROP FILES HERE", displayWidth / 2.0, 24);

}

// halt the entire application once a user
// presses the 'escape' key
void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}


// A file drop may contain the names
// of several files at once

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
			auto win = window(0, 0, 800, 600);
			win->setBackgroundColor(Pixel(255, 255, 255, 255));
			win->setTitle(e.filenames[i]);
			win->addDrawable(doc);
			
			addGraphic(win);
		}
	}
}
