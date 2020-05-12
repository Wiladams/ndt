
/*
	
	Adapted From: https://thecodingtrain.com/CodingChallenges/001-starfield.html

*/
#include "p5.hpp"

#include <array>
#include "Star.hpp"

using namespace p5;

double speed = 1;
static const double MaxSpeed = 100;
static const int MaxStars = 2400;
static const int bottomMargin = 160; 

int centerX;
int centerY;
int posOffsetX = 0;
int posOffsetY = 0;

std::array<Star, MaxStars> stars;

void keyReleased(const KeyEvent& event)
{
	switch (keyCode) {
	case VK_ESCAPE: {
		halt();
	} break;
	case VK_RIGHT:
		posOffsetX += 10;
		break;
	case VK_LEFT:
		posOffsetX -= 10;
		break;
	}
}

void mouseWheel(const MouseEvent& e)
{
	if (e.delta < 0) {
		speed -= 10;
		if (speed < 0)
			speed = 0;
	}
	else {
		speed += 10;
	}
}



void draw()
{

	clear();

	// When the mouse drives where you're looking
	// you want to use this inverse relationaship between its
	// position and where the center is.
	int mx = (int)map(mouseX, 0,width-1, width-1, 0);
	int my = (int)map(mouseY, height - bottomMargin, height, height - bottomMargin, 0);

	// When you want to use the position for steering
	// you'll want to center to track the mouse position



	centerX = mx + posOffsetX;
	centerY = my + posOffsetY;

	push();
	translate(centerX, centerY);
	for (int i = 0; i < stars.size(); i++)
	{
		stars[i].update(speed);
		stars[i].draw(*gAppSurface);
	}
	pop();

	noStroke();
	fill(127);
	rect(0, height - bottomMargin, width, bottomMargin);
}

void setup()
{
	createCanvas(displayWidth, displayHeight-40);

	for (int i = 0; i < MaxStars; i++) {
		stars[i].init(width, height);
	}

	centerX = width / 2;
	layered();
	setWindowPosition(0, 0);
}