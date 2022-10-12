// this sample is really just a test of the FileStream
// class.  It will open up a filestream, then create a histogram
// of the bytes, and display those values in a bar chart.
//
// Usage
// Drag-n-Drop a file onto the open window.  It's histogram
// should show up
//
// Packing references
// https://web.archive.org/web/20170109223003/http://clb.demon.fi/files/RectangleBinPack.pdf
// https://github.com/Lalaland/PixelPacker
//

#include "p5.hpp"

#include "filehisto.h"


using namespace p5;


void draw()
{

	// Clear to background, whatever it is
	if (!isLayered())
		background(0);	// (245 , 246, 247);
	else
		clear();

	noStroke();
	fill(0x7f);
	rect(0, 0, canvasWidth, 48);

	// Put some instructional text up there
	fill(0);
	textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
	textSize(36);
	textFont("Consolas");
	text("DROP FILES HERE", displayWidth/2.0, 24);

	flush();
}

void setup()
{
	//createCanvas(1024, 768);
	fullscreen();

	dropFiles();	// allow dropping of files

	// Use a cascading window layout
	std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	windowLayout(layout);
}

// A file drop may contain the names
// of several files at once
void fileDrop(const FileDropEvent& e)
{
	// assuming there's at least one file that 
	// has been dropped.
	for (int i = 0; i < e.filenames.size(); i++)
	{
		auto win = window(0, 0, 256, 256);
		win->setBackgroundColor(Pixel(255, 255, 255,255));
		win->setTitle(e.filenames[i]);
		win->addChild(FileHistogram::fromFile(e.filenames[i]));
	}
}

// halt the entire application once a user
// presses the 'escape' key
void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}