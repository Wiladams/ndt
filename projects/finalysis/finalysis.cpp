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
	background(245, 246, 247);
	winman.draw(gAppSurface);
}

void setup()
{
	createCanvas(1024, 768);
	dropFiles();
	//noLoop();
	//frameRate(15);
	//layered();
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

	redraw();
}

void mouseEvent(const MouseEvent& e)
{
	winman.mouseEvent(e);
}

