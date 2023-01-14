#include "p5.hpp"

#include "svgdocument.h"
#include "svgwindow.h"
#include "svgiconpage.h"

using namespace p5;
using namespace ndt;
using namespace ndt_debug;
using namespace svg;

std::shared_ptr<SVGIconPage> gIconPage{};

void testSomething()
{	
	int key_size = 8;
	int init_bulk = maths::max(1 << (key_size + 1), 0x100);
	int nentries = (1 << key_size) + 2;
	int ksize = ((uint8_t)1 << key_size);

	printf("key_size: %d\n", key_size);
	printf("init_bulk: %d\n", init_bulk);
	printf("nentries: %d\n", nentries);
	printf("ksize: %d\n", ksize);
}

void setup()
{
	//testSomething();
	
	//createCanvas(1920, 1200);
	fullscreen();

	dropFiles();	// allow dropping of files

	// Use a cascading window layout
	//std::shared_ptr<ILayoutGraphics> layout = std::make_shared<CascadeLayout>(canvasWidth, canvasHeight);
	//windowLayout(layout);

	frameRate(30);

	auto iconWin = window(20, 60, 360, displayHeight-60-48);
	gIconPage = std::make_shared<SVGIconPage>(0, 0, 340, iconWin->frameHeight());
	iconWin->addGraphic(gIconPage);

	auto detailWin = window(440, 60, 800, 800);
	auto detailPage = std::make_shared<SVGWindow>(0, 0, 800, 800, nullptr);
	detailWin->addGraphic(detailPage);
	
	gIconPage->subscribe([detailPage](std::shared_ptr<svg::SVGDocument> doc) {detailPage->document(doc, true); });

	

}


void draw()
{
	// Clear to background, whatever it is
	if (!isLayered())
		background(Pixel(0,0,0,0));	// (245 , 246, 247);
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
	gIconPage->fileDrop(e);
}
