
/*
	
	Adapted From: https://thecodingtrain.com/CodingChallenges/001-starfield.html

*/
#include "p5.hpp"

#include <array>
#include "star.hpp"
#include "joystick.h"

using namespace p5;

double speed = 1;
static constexpr double MaxSpeed = 300;
static constexpr int MaxStars = 2400;
//static const int MaxStars = 10;
static const int bottomMargin = 160; 

float centerX;
float centerY;
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

	if (joy1.isValid()) 
	{
		// Get joystick position information
		JoystickEvent je;
		joy1.getPosition(je);

		// get speed from joystick throttle
		speed = map(je.z, 0, 1, 0, MaxSpeed);

		centerX = map(je.x, -1, 1, canvasWidth - 1, 0);
		centerY = map(je.y, -1, 1, 0, canvasHeight - bottomMargin);
	} else {
		// use keyboard/mouse navigation
		float mx = (int)map(mouseX, 0, canvasWidth - 1, canvasWidth - 1, 0);
		float my = (int)map(mouseY, canvasHeight - bottomMargin, canvasHeight, canvasHeight - bottomMargin, 0);

		// When you want to use the position for steering
		// you'll want to center to track the mouse position
		centerX = mx + posOffsetX;
		centerY = my + posOffsetY;
	}


	push();
	translate(centerX, centerY);
	for (int i = 0; i < stars.size(); i++)
	{
		stars[i].update(speed);
		stars[i].draw(*gAppSurface);
	}
	pop();

	noStroke();
	fill(127,127);
	rect(0, canvasHeight - bottomMargin, canvasWidth, bottomMargin);

	flush();
}

void setup()
{
	createCanvas(displayWidth, displayHeight-40);
	layered();
	setCanvasPosition(0, 0);

	for (int i = 0; i < MaxStars; i++) {
		stars[i].init(canvasWidth, canvasHeight);
	}

	centerX = canvasWidth / 2.0;

}