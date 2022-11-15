/*
	Demonstrate screen capture and recording.
*/
#include "p5.hpp"
#include "elements/screensnapshot.hpp"

#include <memory>

using namespace p5;

ScreenSnapper snapper;

void draw()
{
	snapper.next();

	image(snapper.getImage(), 0, 0);
}

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode)
	{
	case VK_ESCAPE:
		halt();
		break;
	}
}

void setup()
{
	createCanvas(displayWidth / 2, displayHeight);

	snapper.reset(displayWidth / 2, 0, displayWidth / 2, displayHeight);
}

