#include "p5.hpp"

using namespace p5;

Color rColors[] = {
	{255,0,0,127},
	{255,165,0, 127},
	{255, 255,0,127},
	{0,255,0,127},
	{0,0,255,127},
	{93,118,203,127},
	{143,94,154,127},
};


void keyReleased(const KeyEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void draw()
{
	int nColors = sizeof(rColors) / sizeof(Color);
	int thickness = 24;

	if (!gIsLayered) {
		background(255);
	}
	else
	{
		clear();
	}


	int xoffset = 0;
	int yoffset = 0;
	for (size_t i = 0; i < nColors; i++) {
		stroke(rColors[i]);
		strokeWeight(thickness);
		bezier(
			100+xoffset, height+thickness, 
			width * 0.3, yoffset, 
			width * 0.666, yoffset, 
			width - 100-xoffset, height+thickness
		);


		stroke(0);
		noFill();
		strokeWeight(1);
		quad(
			100 + xoffset, height+thickness,
			width * 0.3, yoffset,
			width * 0.666, yoffset,
			width - 100 - xoffset, height+thickness
		);

		xoffset += thickness+(thickness/2);
		yoffset += thickness+(thickness/3);

	}


}

void setup()
{

	createCanvas(1024, 768);
	//createCanvas(displayWidth, displayHeight);
	//layered();
	//setWindowPosition(0, 0);
}