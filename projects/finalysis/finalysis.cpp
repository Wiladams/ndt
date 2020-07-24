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
#include "histoman.h"
#include "histowindow.h"

using namespace p5;


HistoManager winman;

void draw()
{
	if (!isLayered())
		background(0);	// (245 , 246, 247);
	else
		clear();
	flush();

	winman.draw(gAppSurface);
	flush();

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

	frameRate(15);
	//commandThreshold(10);
}

void fileDrop(const FileDropEvent& e)
{
	// assuming there's at least one file that 
	// has been dropped.
	for (int i = 0; i < e.filenames.size(); i++)
	{
		auto win = std::make_shared<FileHistoWindow>(e.filenames[i], 0, 0);
		winman.addWindow(win);
	}

	//redraw();
}

void mouseEvent(const MouseEvent& e)
{
	winman.mouseEvent(e);
}

void keyReleased(const KeyEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}