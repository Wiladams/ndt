#include "p5.hpp"
#include "codec_targa.hpp"

using namespace p5;

BLImage* diffuse = nullptr;

void preload()
{
	targa::TargaMeta meta;
	//diffuse = targa::readFromFile("models\\grid.tga", meta);
	diffuse = targa::readFromFile("models\\floor_diffuse.tga", meta);

	printf("loaded diffuse: %p\n", diffuse);
}

void setup()
{
	preload();
	createCanvas(800, 800);
}

void draw()
{
	if (nullptr != diffuse)
		image(*diffuse, 0, 0);
}