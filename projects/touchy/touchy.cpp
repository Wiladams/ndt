#include "p5.hpp"
#include <unordered_map>
#include <algorithm>

using namespace p5;


std::unordered_map<int, TouchEvent> touches;

void setup()
{
	createCanvas(1920, 1080);
	//frameRate(60);
	touch();
}

void drawTouch(std::pair<int,TouchEvent> pe)
{
	auto idx = pe.first;
	auto e = pe.second;

	noStroke();
	fill(0xc0);
	ellipse(e.x, e.y, e.w, e.h);
}

void draw()
{
	background(color(245, 246, 247));

	// Go through list of active touches
	// and draw them
	std::for_each(touches.begin(), touches.end(), drawTouch);
}

void touchPressed(const TouchEvent& e)
{
	//printf("touchPressed: %d, %d  %dx%d\n", e.x, e.y, e.w, e.h);
	touches.insert_or_assign(e.id, e);
}

void touchReleased(const TouchEvent& e)
{
	//printf("touchReleased: %d, %d\n", e.x, e.y);
	// delete event id from map
	touches.erase(e.id);
}

void touchMoved(const TouchEvent& e)
{
	//printf("touchMoved: %d, %d  %dx%d\n", e.x, e.y, e.w, e.h);
	touches.insert_or_assign(e.id, e);
}

void touchHovered(const TouchEvent& e)
{
	printf("touchHovered: %d, %d\n", e.x, e.y);
	// add to hover list
}
