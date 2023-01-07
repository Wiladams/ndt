#include "p5.hpp"

#include "svgscanner.h"
#include "xmlscan.h"
#include "mmap.hpp"
#include "cssscanner.h"
#include "base64.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace p5;
using namespace ndt;
using namespace ndt_debug;
using namespace svg;

BLVar gradVar{};

struct SVGWindow : public GWindow
{
	maths::vec2f fLastDragLoc{};
	


	SVGWindow(float x, float y, float w, float h) : GWindow(x,y,w,h)
	{
		//setFrame(frame);
	}



	virtual void keyEvent(const KeyboardEvent& e) override
	{
		switch (e.activity)
		{
		case KEYPRESSED:
			if (e.keyCode == VK_RIGHT)
			{
				translateBoundsBy(10, 0);

			}
			else if (e.keyCode == VK_LEFT)
			{
				translateBoundsBy(-10, 0);
			}
			break;
		}

		if (fActiveGraphic != nullptr)
			fActiveGraphic->keyEvent(e);
	}
	
	void mouseEvent(const MouseEvent& e) override
	{
		//printf("MOUSE MOVING: %d (%3.0f,%3.0f)\n", e.activity, e.x, e.y);


		switch (e.activity) {
		case MOUSEWHEEL:
		{
			if (e.delta < 0)
				scaleBoundsBy(0.9f, 0.9f);
			else
				scaleBoundsBy(1.1f, 1.1f);

			needsRedraw(true);
		}
		break;

		case MOUSEPRESSED:
		{
			fLastDragLoc = { e.x, e.y };
		}
		break;

		case MOUSEMOVED:
		{
			if (!isMoving() && fIsDragging)
			{

				float diffx = e.x - fLastDragLoc.x;
				float diffy = e.y - fLastDragLoc.y;
				//printf("MOUSE DRAGGING: %d (%3.0f,%3.0f)\n", e.activity, diffx, diffy);

				translateBoundsBy(diffx, diffy);
				fLastDragLoc = { e.x, e.y };

				needsRedraw(true);
			}
		}
		break;
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
