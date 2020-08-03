#include "p5.hpp"
#include "texture.h"

using namespace p5;

void setup()
{
	createCanvas(400, 400);
}

void draw()
{
	printf("draw\n");
}

void update(const double value)
{
	std::cout << "update: " << std::to_string(value) << std::endl;
}