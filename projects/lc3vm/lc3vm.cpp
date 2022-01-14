#include "p5.hpp"
#include "lc3.h"
#include "filestream.h"
#include "vmview.h"

#include <memory>

using namespace p5;

std::shared_ptr<GWindow> appWindow=nullptr;

class AppView : public Graphic
{
public:
	void draw(std::shared_ptr<IGraphics> ctx)
	{
		ctx->noStroke();
		ctx->fill(0);
		ctx->textSize(24);
		ctx->textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
		ctx->text("Drop Some Code Here", 100, 100);
	}
};

//
// https://justinmeiners.github.io/lc3-vm/
//
// A VM implementation
// The general idea here is to create a virtual machine
// that uses blend2d as a graphics co-processor
// The integration with the graphics can happen several ways.
// It can be a library of routines that are available through assembly,
// or it can be more specialized with actual graphics instructions baked
// into the instruction set of the vm.
// Perhaps the most flexible will be to implement as memory mapped
// registers, which makes it look like a real graphics co-processor
//
// It's just a fun experimental thing to play with creating a full fledged
// VM with graphics capability.
//
void setup()
{
	fullscreen();

	// Create the window that will display our app's stuff
	appWindow = window(0, 0, 640, 480);
	appWindow->setTitle("lc3vm");
	appWindow->addChild(std::make_shared<AppView>());

	// load a program from command line
	// run the program on a vm
	dropFiles();
}

void draw()
{
	clear();
}

// A file drop may contain the names
// of several files at once
void fileDrop(const FileDropEvent& e)
{
	// assuming there's at least one file that 
	// has been dropped.
	// Create a little terminal window for each 
	// program
	for (int i = 0; i < e.filenames.size(); i++)
	{
		auto win = window(0, 0, 320,480);
		win->setTitle(e.filenames[i]);
		win->addChild(Lc3VMView::fromFilename(e.filenames[i]));
	}
}

// halt the entire application once a user
// presses the 'escape' key
void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}
