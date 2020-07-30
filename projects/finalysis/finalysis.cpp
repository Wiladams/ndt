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

#include <memory>
#include <deque>
#include "filehisto.h"


using namespace p5;


void draw()
{
	if (!isLayered())
		background(0);	// (245 , 246, 247);
	else
		clear();


	noStroke();
	fill(255, 0, 0);
	rect(0, 0, width, 48);
	flush();
}

void setup()
{
	//createCanvas(1024, 768);
	fullscreen();

	dropFiles();	// allow dropping of files
}

void fileDrop(const FileDropEvent& e)
{
	// assuming there's at least one file that 
	// has been dropped.
	for (int i = 0; i < e.filenames.size(); i++)
	{
		auto win = window(0, 0, 256, 256);
		win->setBackgroundColor(Pixel(255, 255, 255,255));
		win->setTitle(e.filenames[i]);
		win->setPage(FileHistogram::fromFile(e.filenames[i]));
	}
}

void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}