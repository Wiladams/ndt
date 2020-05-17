#include "p5.hpp"
#include "codec_targa.hpp"

using namespace p5;

BLImage* diffuse = nullptr;

void preload()
{
	targa::TargaMeta meta;
	diffuse = targa::readFromFile("models\\grid.tga", meta);

	printf("loaded diffuse: %p\n", diffuse);
}

void setup()
{
	createCanvas(800, 800);
}

void draw()
{
	if (nullptr != diffuse)
		image(*diffuse, 0, 0);
}