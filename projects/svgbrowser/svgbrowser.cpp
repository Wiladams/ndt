#include "p5.hpp"

#include "svgscanner.h"
#include "xmlscan.h"
#include "mmap.hpp"
#include "cssscanner.h"
#include "base64.h"

using namespace p5;
using namespace ndt;
using namespace ndt_debug;
using namespace svg;

BLVar gradVar{};

struct SVGWindow : public GWindow
{
	SVGWindow(float x, float y, float w, float h) : GWindow(x,y,w,h)
	{
		//setFrame(frame);
	}

	void mouseEvent(const MouseEvent& e) override
	{

		if (e.activity == MOUSEWHEEL)
		{
			translateBoundsBy(0, e.delta * 10);
		}

		GWindow::mouseEvent(e);
	}

};

void setup()
{

	//testBase64();
	
	//createCanvas(1024, 768);
	fullscreen();

	//testShape();
	
	dropFiles();	// allow dropping of files

	// Use a cascading window layout
	//std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	//windowLayout(layout);

	frameRate(30);
}

void testGradient()
{
	BLGradient gradient;
	gradient.create(BLRadialGradientValues(mouseX, mouseY, mouseX, mouseY, canvasHeight / 2), BL_EXTEND_MODE_PAD);
	gradient.addStop(0.0, BLRgba32(0x00000000));
	gradient.addStop(1.0, BLRgba32(0xFFFFFFFF));
	blVarAssignWeak(&gradVar, &gradient);
	gAppSurface->fill(gradVar);
	gAppSurface->rect(0, 0, canvasWidth, canvasHeight);
}

void draw()
{

	// Clear to background, whatever it is
	if (!isLayered())
		background(0);	// (245 , 246, 247);
	else
		clear();

	//background(255);

	//testGradient();
	
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
			auto win = std::make_shared<SVGWindow>(0, 0, 800, 600);
			//auto win = window(0, 0, 1920, 1080);
			win->setBackgroundColor(Pixel(255, 255, 255, 255));
			//win->setTitle(e.filenames[i]);
			win->addDrawable(doc);
			
			addGraphic(win);
		}
	}
}
