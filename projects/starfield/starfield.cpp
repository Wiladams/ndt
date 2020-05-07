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


std::array<Star, MaxStars> stars;

void keyReleased(const KeyEvent& event)
{
	if (keyCode == VK_ESCAPE) {
		halt();
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
	centerX = map(mouseX, 0,width-1, width-1, 0);
	//centerX = mouseX;
	centerY = map(mouseY, height - bottomMargin, height, height - bottomMargin, 0);


	clear();

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