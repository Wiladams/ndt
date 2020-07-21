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


class HistoManager : public IDrawable
{
	int wX = 10;
	int wY = 10;

	std::deque<std::shared_ptr<GWindow> > windows;

public:
	void draw(IGraphics* ctx)
	{
		for (std::shared_ptr<GWindow> win : windows) 
		{
			win->draw(ctx);
			ctx->flush();
		}
	}

	void addWindow(std::shared_ptr<GWindow> win)
	{
		// do rudimentary layout as we add windows
		//layout();

		int x = wX;
		int y = wY;
		wX += 256 - 10;
		wY += 10;

		if (wX > width - 256) {
			wX = 10;
			wY += 256 + 8;
		}

		win->moveTo(x, y);

		windows.push_back(win);
	}

	std::shared_ptr<GWindow> windowAt(int x, int y)
	{
		// traverse through windows in reverse order
		// return when one of them contains the mouse point
		std::deque<shared_ptr<GWindow> >::reverse_iterator rit = windows.rbegin();
		for (rit = windows.rbegin(); rit != windows.rend(); ++rit)
		{
			if ((*rit)->contains(x, y))
				return *rit;
		}

		return nullptr;
	}

	void moveToFront(std::shared_ptr<GWindow> win)
	{
		std::deque<std::shared_ptr<GWindow> >::iterator it = windows.begin();
		for (it = windows.begin(); it != windows.end(); ++it)
		{
			if (*it == win) {
				windows.erase(it);
				windows.push_back(win);
				break;
			}
		}
	}

	void mousePressed(const MouseEvent& e)
	{
		// Figure out which window is being 
		// clicked
		auto win = windowAt(e.x, e.y);

		// if not clicked on a view, then simply return
		if (nullptr == win)
			return;

		// bring it to the front
		moveToFront(win);
		win->mousePressed(e);

		std::cout << "WindowManager.mousePressed " << e.x << ", " << e.y << std::endl;
	}

	void mouseDragged(const MouseEvent& e)
	{		
		// figure out which window we're dragging
		auto win = windowAt(e.x, e.y);

		// if not clicked on a view, then simply return
		if (nullptr == win)
			return;

		win->mouseDragged(e);
	}
};


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

void mousePressed(const MouseEvent& e)
{
	winman.mousePressed(e);
}

void mouseDragged(const MouseEvent& e)
{
	winman.mouseDragged(e);
}