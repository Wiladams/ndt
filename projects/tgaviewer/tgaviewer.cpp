#include "p5.hpp"
#include "elements/codec_targa.hpp"

using namespace p5;

BLImage img{};

void setup()
{
	ndt::FileStream bs("shuttle.tga");

	targa::TargaMeta meta{};

	bool success = targa::readMetaInformation(bs, meta);

	if (!success)
	{
		printf("could not load meta information\n");
		halt();
	}

	success = targa::readBody(bs, meta, img);

	if (!success)
	{
		printf("ERROR: failed to load diffuse: %d\n", success);
		halt();
	}

	createCanvas(meta.header.Width, meta.header.Height);

}

void draw()
{
	if (isLayered())
		clear();
	else
		background(0xdd);

	image(img, 0, 0);
}