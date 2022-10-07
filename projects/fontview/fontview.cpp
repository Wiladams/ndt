
#include "p5.hpp"
#include "emspaceview.h"
#include "gconsole.h"
#include "filestream.h"
#include "vt100stream.h"
#include "fontglyphgrid.h"

#include <cstdarg>
#include <filesystem>
#include <algorithm>
#include <cstdio>


using namespace p5;

namespace fs = std::filesystem;

using Console = GConsole;

std::shared_ptr<Console> acons;
std::shared_ptr<EMSpaceView> emView;
std::shared_ptr<FontGlyphGrid> glyphGrid;

BLFontFace face;

void displayFace(Console &cons, BLFontFace &fontFace)
{
	BLFontFaceInfo faceInfo{};

	if (!fontFace.isValid())
		return;

	faceInfo = fontFace.faceInfo();

	// BLFontDesignMetrics, give you the raw metrics from 
	// when the fontface was parsed.
	BLFontDesignMetrics designMetrics = fontFace.designMetrics();


	cons.newLine();
	cons.puts(" Font Full Name: ");

	cons.printf("%s\n", fontFace.fullName().data());
	cons.printf("         family: %s\n", fontFace.familyName().data());
	cons.printf("     sub-family: %s\n", fontFace.subfamilyName().data());
	cons.printf("postscript name: %s\n", fontFace.postScriptName().data());

	cons.newLine();

	cons.putsln(" == DESIGN METRICS ==");
	cons.newLine();

	cons.printf("          units per EM: %d\n", designMetrics.unitsPerEm);
	cons.printf("           lowest PPEM: %d\n", designMetrics.lowestPPEM);
	cons.printf("              line gap: %d\n", designMetrics.lineGap);
	cons.printf("             mean line: %d\n", designMetrics.xHeight);

	cons.newLine();
	cons.putsln("      Horizontal Layout");
	cons.printf("                ascent: %d\n", designMetrics.ascent);
	cons.printf("               descent: %d\n", designMetrics.descent);
	cons.printf("   min leading bearing: %d\n", designMetrics.hMinLSB);
	cons.printf("  min trailing bearing: %d\n", designMetrics.hMinTSB);
	cons.printf("           max advance: %d\n", designMetrics.hMaxAdvance);
	cons.newLine();

	cons.printf("            cap height: %d\n", designMetrics.capHeight);

    int gbWidth = designMetrics.glyphBoundingBox.x1 - designMetrics.glyphBoundingBox.x0;
    int gbHeight = designMetrics.glyphBoundingBox.y1 - designMetrics.glyphBoundingBox.y0;

	cons.printf("    glyph bounding box: [%d,%d,%d,%d]\n",
        designMetrics.glyphBoundingBox.x0, designMetrics.glyphBoundingBox.y0,
        designMetrics.glyphBoundingBox.x1, designMetrics.glyphBoundingBox.y1);
	cons.printf("                      : %dx%d\n",
		gbWidth, gbHeight);

	cons.printf("     underlinePosition: %d\n", designMetrics.underlinePosition);
	cons.printf("    underlineThickness: %d\n", designMetrics.underlineThickness);
	cons.printf(" strikethroughPosition: %d\n", designMetrics.strikethroughPosition);
	cons.printf("strikethroughThickness: %d\n", designMetrics.strikethroughThickness);

	BLFont fon;
	BLFontMatrix fontMatrix;
	fon.createFromFace(fontFace, 72);
	blFontGetMatrix(&fon, &fontMatrix);

	cons.newLine();
	cons.newLine();

	cons.putsln(" == FONT INFORMATION == ");

	cons.putsln("  Font Matrix ");
	cons.printf(" %f  %f\n", fontMatrix.m00, fontMatrix.m01);
	cons.printf(" %F  %f\n", fontMatrix.m10, fontMatrix.m11);

}

void selectFace(BLFontFace& face)
{
	displayFace(*acons, face);
}

void displayStream()
{
	FileStream strm("n4-wendy.ans");
	VT100Stream tstrm(strm);

	while (tstrm.next())
	{
		printf("something\n");
	}
}



void displayFamilies()
{
	for (auto& name : gFontHandler->familyNames())
	{
		acons->putsln(name.c_str());
	}
}

void draw()
{
	clear();
	background(200, 200,200);

	//noLoop();
}

void setup()
{
	frameRate(8);
	createCanvas(1280, 1024, "fontview");
	//fullscreen();
	//loadFontDirectory("c:\\windows\\fonts");
	//loadDefaultFonts();

	face = gFontHandler->queryFontFace("Courier New");
	//face = gFontHandler->queryFontFace("Cascadia Code");

	acons = std::make_shared<Console>(60, 40, "Cascadia Code");
	auto win1 = window(0, 0, acons->getBounds().w, acons->getBounds().h);
	win1->setBackgroundColor(color(25,55,55,200));
	//win1->setTitle("console");

	win1->addChild(acons);
	win1->moveTo(8, 8);


	emView = std::make_shared<EMSpaceView>(600,600);
	emView->setFace(face);
	emView->setGlyphId(100);

	auto win2 = window(0, 0, 600, 600);
	win2->addChild(emView);
	win2->moveTo(462, 8);

	glyphGrid = std::make_shared<FontGlyphGrid>(600, 0);
	glyphGrid->setFontFace(face);

	auto win3 = window(0, 0, 600, 200);
	win3->addChild(glyphGrid);
	win3->moveTo(462, 610);

	//displayFamilies();
	//displayStream();

	selectFace(face);
}

void keyReleased(const KeyboardEvent& event)
{
	switch (keyCode)
	{
		case VK_F1:
			fullscreen();
		break;

		case VK_DOWN: {
			acons->scrollUp();
		}break;
		
		case VK_ESCAPE:
		{
			halt();
		}break;
	}
}



