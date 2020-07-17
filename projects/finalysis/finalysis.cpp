// this sample is really just a test of the FileStream
// class.  It will open up a filestream, then create a histogram
// of the bytes, and display those values in a bar chart.
//
// Usage
// Drag-n-Drop a file onto the open window.  It's histogram
// should show up
//
#include "p5.hpp"


#include <memory>
#include <deque>
#include "filehisto.h"

using namespace p5;

std::shared_ptr<FileHistoWindow> histowindow = nullptr;
std::deque<std::shared_ptr<GWindow> > windows;

int wX = 10;
int wY = 10;

void draw()
{
	background(245, 246, 247);

	for (std::shared_ptr<GWindow> win : windows) {
		win->draw(gAppSurface);
	}
}

void setup()
{
	createCanvas(1024, 768);
	dropFiles();
	noLoop();
}

void fileDrop(const FileDropEvent& e)
{
	// assuming there's at least one file that 
	// has been dropped.
	for (int i = 0; i < e.filenames.size(); i++)
	{
		int x = wX;
		int y = wY;
		wX += 256 + 10;
		

		if (wX > width - 256) {
			wX = 10;
			wY += 256 + 8;
		}

		auto win = std::make_shared<FileHistoWindow>(e.filenames[i], x, y);

		windows.push_back(win);
	}

	redraw();
}
