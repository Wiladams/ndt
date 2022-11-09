

/*
	Demonstration of instance sampling
	Take a snapshot of the screen as the source
	and replicate it multiple times across the screen

	Control
	Up Arrow - Add more rows, up to 16
	Right Arrow - Add more columns, up to 16

	Down, Left - Reduce rows, columns

	Using a Youtube window pegged to the left edge
	of the monitor works well.

	This is also a perfect place to check if multi
	threading will make a significant performance
	difference.

	A work stealing strategy might work out where each
	thread grabs a sub-section of the screen to pull
	pixels from.
*/

#include "p5.hpp"
#include "screensnapshot.hpp"
#include <memory>

using namespace p5;

static const int captureWidth = 1280;
static const int captureHeight = 1080;

constexpr int maxCols = 32;
constexpr int maxRows = 32;

int numCols = 1;
int numRows = 1;

const int FRAMERATE = 10;

std::shared_ptr<ScreenSnapshot> snapper = nullptr;

void drawCells(IGraphics &ctx)
{
	int cellWidth = canvasWidth / numCols;
	int cellHeight = canvasHeight / numRows;

	auto& img = snapper->getImage();
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
	snapper->next();

	drawCells(*gAppSurface);
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
	createCanvas(captureWidth, captureHeight);

	snapper = std::make_shared<ScreenSnapshot>(40, 158, 640, 450);
	frameRate(FRAMERATE);
}
