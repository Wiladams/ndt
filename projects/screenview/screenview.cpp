/*
	Demonstrate screen capture and recording.
*/

#include "apphost.h"
#include "p5.hpp"
#include "screensnapshot.hpp"

#include <memory>

using namespace p5;

std::shared_ptr<ScreenSnapshot> snapper=nullptr;

void draw()
{
	snapper->next();

	image(snapper->getImage(), 0, 0);
}

void setup()
{
	createCanvas(displayWidth / 2, displayHeight);

	snapper = std::make_shared<ScreenSnapshot>(displayWidth / 2, 0, displayWidth / 2, displayHeight);
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