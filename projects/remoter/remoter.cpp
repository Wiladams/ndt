#include "p5.hpp"
#include "GraphicsDecoder.hpp"
#include "GraphicsEncoder.hpp"
#include "binstream.hpp"

using namespace p5;


void drawRaw()
{
	background(color(0xc0));
	stroke(color(255, 0, 0));
	fill(color(0));
	rect(100, 100, 200, 200);
	//flush();
}

void drawDirect()
{
	gAppSurface->background(color(0xc0));
	gAppSurface->stroke(Pixel(255, 0, 0, 255));
	gAppSurface->fill(color(0));
	gAppSurface->rect(100, 100, 200, 200);
	gAppSurface->stroke(color(255,255,0));
	gAppSurface->line(10, 10, 100, 100);
}

void drawRemote()
{
	uint8_t buff[4096];
	size_t buffLen = 4096;

	BinStream bs(buff, buffLen);
	GraphicsEncoder ctx(bs);

	// Black rectangle, red border
	ctx.background(color(0xc0));
	ctx.stroke(color(255, 0, 0));
	ctx.fill(color(0));
	ctx.rect(100, 100, 200, 200);

	// yellow lines outside rectangle
	ctx.stroke(color(255,255,0));
	ctx.line(10, 10, 100, 10);
	ctx.line(100, 10, 100, 100);
	ctx.line(100, 100, 10, 100);
	ctx.line(10, 100, 10, 10);

	// rewind the bs and replay
	//printf("Command Size: %zd\n", bs.tell());
	BinStream subrange = bs.range(0, bs.tell());
	GraphicsDecoder dec(subrange, gAppSurface);

	dec.run();
}

void draw()
{
	//drawRaw();
	drawRemote();
	//drawDirect();
}

void setup()
{
	createCanvas(400, 400);
	//frameRate(1);
}