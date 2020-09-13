#include "p5.hpp"
#include <unordered_map>
#include <algorithm>

using namespace p5;


std::unordered_map<int, TouchEvent> touches;
std::unordered_map<int, Pixel> tcolor;

Pixel randomColor()
{
	return color(random(255), random(255), random(255));
}

void setup()
{
	createCanvas(1920, 1080);
	//fullscreen();

	//frameRate(60);
	
	// turn on using raw touch input
	// precludes gesture processing
	auto isTouchy = touch();
	std::cout << "isTouchy: " << isTouchy << std::endl;
}

void drawTouch(std::pair<int,TouchEvent> pe)
{
	auto idx = pe.first;
	auto e = pe.second;

	noStroke();
	fill(tcolor[idx]);
	ellipse(e.x, e.y, e.w, e.h);
}

void draw()
{
	background(color(245, 246, 247));

	// Go through list of active touches
	// and draw them
	std::for_each(touches.begin(), touches.end(), drawTouch);
}

void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();
}

void touchStarted(const TouchEvent& e)
{
	printf("touchPressed: %d, %d  %dx%d\n", e.x, e.y, e.w, e.h);
	touches.insert_or_assign(e.id, e);
	tcolor.insert_or_assign(e.id, randomColor());
}

void touchEnded(const TouchEvent& e)
{
	printf("touchReleased: %d, %d\n", e.x, e.y);
	// delete event id from map
	touches.erase(e.id);
	tcolor.erase(e.id);
}

void touchMoved(const TouchEvent& e)
{
	printf("touchMoved: %d, %d  %dx%d\n", e.x, e.y, e.w, e.h);
	touches.insert_or_assign(e.id, e);
}

void touchHovered(const TouchEvent& e)
{
	printf("touchHovered: %d, %d\n", e.x, e.y);
	// add to hover list
}
