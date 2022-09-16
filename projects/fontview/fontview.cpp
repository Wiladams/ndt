#include "p5.hpp"

#include <cstdarg>
#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <iostream>

using namespace p5;

namespace fs = std::filesystem;

int lineSize = 16;
static int xpos = 10;
static int ypos = 10;

void print(const char* format, ...)
{

	char txtBuff[512];
	va_list args = nullptr;
	va_start(args, format);

	vsprintf_s(txtBuff, format, args);


	gAppSurface->text(txtBuff, xpos, ypos);
	va_end(args);

	auto pt = textMeasure(txtBuff);

	xpos += pt.x;

}

void printLine(const char* format, ...)
{
	char txtBuff[512];
	va_list args = nullptr;
	va_start(args, format);

	vsprintf_s(txtBuff, format, args);


	gAppSurface->text(txtBuff, xpos, ypos);
	va_end(args);

	auto pt = textMeasure(txtBuff);

	ypos += pt.y;
	xpos = 10;
}

void displayFace(BLFontFace &fontFace)
{
	BLFontFaceInfo faceInfo{};

	if (!fontFace.isValid())
		return;

	faceInfo = fontFace.faceInfo();

	// BLFontDesignMetrics, give you the raw metrics from 
	// when the fontface was parsed.
	BLFontDesignMetrics designMetrics = fontFace.designMetrics();

	textFont("Consolas");
	//textFont("Courier New");
	textSize(lineSize);
	textAlign(ALIGNMENT::LEFT, ALIGNMENT::TOP);

	fill(0);
	stroke(0);
	print("     Font Full Name:  ");
	noStroke();
	printLine("%s", fontFace.fullName().data());
	printLine("         family: %s", fontFace.familyName().data());
	printLine("     sub-family: %s", fontFace.subfamilyName().data());
	printLine("postscript name: %s", fontFace.postScriptName().data());

	printLine("");

	stroke(0);
	printLine(" == DESIGN METRICS ==");
	noStroke();
	printLine("");

	//! Units per EM square.
	printLine("          units per EM: %d", designMetrics.unitsPerEm);

	//! Lowest readable size in pixels.
	printLine("           lowest PPEM: %d", designMetrics.lowestPPEM);

	printLine("              line gap: %d", designMetrics.lineGap);

	//! Distance between the baseline and the mean line of lower-case letters.
	printLine("             mean line: %d", designMetrics.xHeight);

	printLine("");
	printLine("      Horizontal Layout");
	printLine("                ascent: %d", designMetrics.ascent);
	printLine("               descent: %d", designMetrics.descent);
	printLine("   min leading bearing: %d", designMetrics.hMinLSB);
	printLine("  min trailing bearing: %d", designMetrics.hMinTSB);
	printLine("           max advance: %d", designMetrics.hMaxAdvance);
	printLine("\n");

	//! Maximum height of a capital letter above the baseline.
	printLine("            cap height: %d", designMetrics.capHeight);

    int gbWidth = designMetrics.glyphBoundingBox.x1 - designMetrics.glyphBoundingBox.x0;
    int gbHeight = designMetrics.glyphBoundingBox.y1 - designMetrics.glyphBoundingBox.y0;

	printLine("    glyph bounding box: [%d,%d,%d,%d]",
        designMetrics.glyphBoundingBox.x0, designMetrics.glyphBoundingBox.y0,
        designMetrics.glyphBoundingBox.x1, designMetrics.glyphBoundingBox.y1);
	printLine("                      : %dx%d",
		gbWidth, gbHeight);



	printLine("     underlinePosition: %d", designMetrics.underlinePosition);
	printLine("    underlineThickness: %d", designMetrics.underlineThickness);
	printLine(" strikethroughPosition: %d", designMetrics.strikethroughPosition);
	printLine("strikethroughThickness: %d", designMetrics.strikethroughThickness);

	BLFont fon;
	BLFontMatrix fontMatrix;
	fon.createFromFace(fontFace, 72);
	blFontGetMatrix(&fon, &fontMatrix);

	printLine("");
	printLine("");
	stroke(0);
	printLine(" == FONT INFORMATION == ");
	noStroke();
	printLine("  Font Matrix ");
	printLine(" %f  %f", fontMatrix.m00, fontMatrix.m01);
	printLine(" %F  %f", fontMatrix.m10, fontMatrix.m11);

	// Display font box
	// Baseline
	// Ascent

	noStroke();
	fill(0);
	//textSize(72);


	//text("The quick brown fox", 10, 100);
}

void displayFontList()
{

//textFont("d:\\Fonts\\Old Copperfield.ttf");
//textFont("d:\\Fonts\\overhead.ttf");
//textFont("d:\\Fonts\\PackardAntique.ttf");
//textFont("d:\\Fonts\\Pamela.ttf");
//textFont("d:\\Fonts\\parsons.ttf");
//textFont("d:\\Fonts\\persuasi.ttf");
//textFont("d:\\Fonts\\pindown.ttf");
//textFont("d:\\Fonts\\pinocchio.ttf");
//textFont("d:\\Fonts\\plasdrip.ttf");

	textFont("Tahoma");

	printLine("Hello, Tahoma");
	//printLine("fm count: %d", gFontManager->faceCount());
}

void setup()
{
	createCanvas(1280, 1024, "fontview");

	background(225);

	displayFontList();

	BLFontFace face;
	face = gFontHandler->queryFontFace("Courier New");

	displayFace(face);

}