
#include "p5.hpp"


#include "emspaceview.h"
#include "fontglyphgrid.h"
#include "fontfaceconsole.hpp"

#include <cstdarg>
#include <filesystem>
#include <algorithm>
#include <cstdio>


using namespace p5;

namespace fs = std::filesystem;


std::shared_ptr<EMSpaceView> emView;
std::shared_ptr<FontGlyphGrid> glyphGrid;

BLFontFace face;



std::shared_ptr<FontFaceConsole> faceCons = nullptr;


void selectFace(BLFontFace& face)
{
	faceCons->setFace(face);
}


void draw()
{
	if (isLayered())
		clear();
	else
		background(200, 200,200);
}

void setup()
{
	//frameRate(1);
	//createCanvas(displayWidth, displayHeight, "fontview");
	fullscreen();
	

	gFontHandler->queryFontFace("Courier New", face);
	//face = gFontHandler->queryFontFace("Cascadia Code");

	faceCons = std::make_shared<FontFaceConsole>(60, 40, "Cascadia Code");

	auto win1 = window(0, 0, faceCons->boundsWidth(), faceCons->boundsHeight());
	win1->addChild(faceCons);
	win1->moveTo({ 8, 8 });
	win1->setBackgroundColor(color(25, 55, 55, 200));
	win1->setTitle("console");

	emView = std::make_shared<EMSpaceView>(600,600);
	emView->setFace(face);
	emView->setGlyphId(100);

	auto win2 = window(0, 0, 600, 600);
	win2->addChild(emView);
	win2->moveTo({ 462, 8 });



	auto win3 = window(0, 0, 600, 200);

	glyphGrid = std::make_shared<FontGlyphGrid>(0, 0, win3->boundsWidth(), win3->boundsHeight());
	glyphGrid->setFontFace(face);
	
	//win3->setBackgroundColor(color(25, 55, 55, 200));
	win3->setBackgroundColor(color(0, 0, 0, 0));
	win3->addChild(glyphGrid);
	win3->moveTo({ 462, 610 });

	selectFace(face);
}

void keyReleased(const KeyboardEvent& event)
{
	switch (keyCode)
	{
		case VK_SPACE:
			layered();
		break;

		case VK_ESCAPE:
		{
			halt();
		}break;
	}
}



