#include "p5.hpp"

using namespace p5;

//
// demonstrating how to us setPpiUnits()
// to setup user determined drawing units
//
// Draw some lines that should be specific
// physical lengths on the screen
//
void drawHundreds()
{
	stroke(127);

	//
	// print lines of different lengths
	// increasing width as we go
	strokeWeight(2);
	line(0, 70, 50, 70);	// half inch
	line(0, 95, 100, 95);	// one inch
	line(0, 120, 150, 120);	// one and a half

	noStroke();
	fill(255);
	textSize(16);
	textAlign(ALIGNMENT::LEFT, ALIGNMENT::BASELINE);
	text("50 units", 175, 70);
	text("100 units", 175, 95);
	text("175 units", 175, 120);

	// Use fractins of an inch
	//strokeWeight(1.0/96);
	//line(0, .5, 0.5, 0.5);	// half inch
	//line(0, .75, 1, 0.75);	// one inch
	//line(0, 1, 1.5, 1);		// one and a half


}

void draw()
{
	//background(255, 255, 0);
	noStroke();
	fill(255);
	textAlign(ALIGNMENT::RIGHT, ALIGNMENT::TOP);
	textSize(16);

	char txtBuff[256];
	sprintf_s(txtBuff, "Display Pixels Per Inch: %d", systemDpi);
	text(txtBuff, 240, 4);
	text("User Units/inch: 100 ", 240,20);

	drawHundreds();



	noLoop();
}

void setup()
{
	createCanvas(320, 240, "calibration");

	// Set user space units to be 96/inch
	setUnitsPerInch(100);
	//setUnitsPerInch(1);
}