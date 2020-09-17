#include "p5.hpp"


using namespace p5;

void setup()
{
	createCanvas(400, 400);

	auto win = window(0, 0, 300, 300);
	win->setTitle("Primary Window");
}

void draw()
{
	clear();
	//printf("draw\n");
}
