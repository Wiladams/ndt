
/*
	
	Adapted From: https://thecodingtrain.com/CodingChallenges/001-starfield.html

*/
#include "p5.hpp"

#include <array>
#include "star.hpp"
#include "joystick.h"

using namespace p5;

double speed = 1;
static const double MaxSpeed = 300;
//static const int MaxStars = 2400;
static const int MaxStars = 10;
static const int bottomMargin = 160; 

int centerX;
int centerY;
int posOffsetX = 0;
int posOffsetY = 0;

std::array<Star, MaxStars> stars;
Joystick joy1(JOYSTICKID1);

void keyReleased(const KeyboardEvent& event)
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

	//if (joy1.isValid()) {
	if (false) {
		// Get joystick position information
		JoystickEvent je;
		joy1.getPosition(je);

		// get speed from joystick throttle
		speed = map(je.z, 0, 1, 0, MaxSpeed);

		centerX = map(je.x, -1, 1, width - 1, 0);
		centerY = map(je.y, -1, 1, 0, height - bottomMargin);
	}
	else {
		// use keyboard/mouse navigation
		int mx = (int)map(mouseX, 0, width - 1, width - 1, 0);
		int my = (int)map(mouseY, height - bottomMargin, height, height - bottomMargin, 0);

		std::cout << "mx: " << mx << " my: " << my << std::endl;

		// When you want to use the position for steering
		// you'll want to center to track the mouse position
		centerX = mx + posOffsetX;
		centerY = my + posOffsetY;
	}
	std::cout << centerX << " " << centerY << std::endl;

	push();
	translate(centerX, centerY);
	for (int i = 0; i < stars.size(); i++)
	{
		stars[i].update(speed);
		stars[i].draw(gAppSurface);
	}
	pop();

	noStroke();
	fill(127);
	rect(0, height - bottomMargin, width, bottomMargin);

	noLoop();
}

void setup()
{
	createCanvas(displayWidth, displayHeight-40);

	for (int i = 0; i < MaxStars; i++) {
		stars[i].init(width, height);
	}

	centerX = width / 2;
	//layered();
	setWindowPosition(0, 0);
}