#include "apphost.h"

#include "GraphicsDecoder.hpp"
#include "GraphicsEncoder.hpp"
#include "binstream.hpp"
#include "blend2d.h"
#include "gpath.h"

using namespace p5;


uint8_t encBuff[1024*64];
size_t encBuffLen = 1024 * 64;

BinStream bs(encBuff, encBuffLen);
GraphicsEncoder enc(bs);


//BinStream subrange = bs.range(0, bs.tell());
//GraphicsDecoder dec(subrange, gAppSurface);
//dec.run();

GPath mainPath;

BLPoint fStartDrag;
BLPoint fEndDrag;
bool fIsDragging = false;

void draw()
{
	background(255);

	stroke(Pixel(0,0,0));
	path(mainPath);

	if (fIsDragging) {
		stroke(Pixel(255, 0, 0));
		line(fStartDrag.x, fStartDrag.y, fEndDrag.x, fEndDrag.y);
	}
}

void setup()
{
	createCanvas(800, 600);
}

void mousePressed(const MouseEvent& e)
{
	printf("mouseDown\n");

	// start a stroke
	// we'll begin with only straight lines
	// indicated by mouse down, drag, release
	fStartDrag = { (double)mouseX, (double)mouseY };
	fIsDragging = true;
}

void mouseMoved(const MouseEvent& e)
{
	printf("mouseMoved: %d, %d\n", mouseX, mouseY);

	if (fIsDragging) {
		fEndDrag = BLPoint((double)mouseX, mouseY);
	}
}

void mouseReleased(const MouseEvent& e)
{
	printf("mouseReleased: %d, %d\n", mouseX, mouseY);
	fIsDragging = false;
	mainPath.addLine(BLLine(fStartDrag.x, fStartDrag.y, fEndDrag.x, fEndDrag.y));
}