

/*
	Take a snapshot of the screen as the source
	and replicate it multiple times across the screen

	Control
	Up Arrow - Add more rows, up to 16
	Right Arrow - Add more columns, up to 16

	Down, Left - Reduce rows, columns

	Using a Youtube window pegged to the left edge
	of the monitor works well.

	This might be a place where using a multi-threaded
	BLContext might have some performance improvement.
*/

#include "p5.hpp"

#include "elements/screensnapshot.hpp"
#include "elements/framestats.h"

#include <memory>

using namespace p5;

static const int captureWidth = 1280;
static const int captureHeight = 1080;

constexpr int maxCols = 32;
constexpr int maxRows = 32;

int numCols = 1;
int numRows = 1;

const int FRAMERATE = 30;

ScreenSnapper snapper;

FrameStats stats;

void drawCells(IGraphics &ctx)
{
	int cellWidth = canvasWidth / numCols;
	int cellHeight = canvasHeight / numRows;

	auto& img = snapper.getImage();
	auto imgWidth = img.width();
	auto imgHeight = img.height();

	for (int row = 0; row < numRows; row++)
	{
		for (int col = 0; col < numCols; col++) 
		{
			int x = col * cellWidth;
			int y = row * cellHeight;


			ctx.scaleImage(img, 0, 0, imgWidth, imgHeight, x, y, cellWidth, cellHeight);
		}
	}
}

void draw()
{
	snapper.next();

	drawCells(*gAppSurface);

	stats.draw(*gAppSurface);
}

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_RIGHT:
		if (numCols < maxCols)
			numCols++;
		//resetCells();
		break;
	case VK_LEFT:
		if (numCols > 1)
			numCols--;
		//resetCells();
		break;

	case VK_DOWN:
		if (numRows > 1)
			numRows--;
		//resetCells();
		break;
	case VK_UP:
		if (numRows < maxRows)
			numRows++;
		//resetCells();
		break;
	}
}



void setup()
{
	//createCanvas(displayWidth/2, displayHeight, "multiwall", 0);
	createCanvas(1920, 1080, "multiwall", 4);

	snapper.reset(0, 0, displayWidth / 2, displayHeight);

	frameRate(FRAMERATE);
}
