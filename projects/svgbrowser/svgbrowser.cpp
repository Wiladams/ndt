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

	void setDocument(std::shared_ptr<SVGDocument> doc)
	{
		// calculate the bounds of the document
		// so we can adjust the scaling for drawing
		addDrawable(doc);
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


void testSomething()
{
	DataChunk param = ndt::chunk_from_cstr("matrix(0.656, 0, 0, 0.656, 427.7833, -461.2639)");
	DataChunk s = param;
	DataChunk field = chunk_trim(chunk_token(param, "("), wspChars);
	printChunk(field);
	param = chunk_trim(param, wspChars + ")");
	printChunk(param);

	BLMatrix2D m{};
	s = parseMatrix(s, m);

	printf("%3.2f %3.2f %3.2f %3.2f %3.2f %3.2f \n", m.m[0], m.m[1], m.m[2], m.m[3], m.m[4], m.m[5]);
}

void setup()
{

	//testSomething();
	
	//createCanvas(1920, 1080);
	fullscreen();

	
	dropFiles();	// allow dropping of files

	// Use a cascading window layout
	//std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	//windowLayout(layout);

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
			auto win = std::make_shared<SVGWindow>(0, 0, 800, 800);
			win->setBackgroundColor(Pixel(255, 255, 255, 255));
			//win->setTitle(e.filenames[i]);

			win->setDocument(doc);
			
			addGraphic(win);
		}
	}
}
