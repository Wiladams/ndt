#include "p5.hpp"
#include <cstring>
#include <cstdio>

using namespace p5;

void setup()
{
	createCanvas(400, 400);

	frameRate(15);
	fill(0);
	textSize(30);
	textAlign(ALIGNMENT::CENTER, ALIGNMENT::CENTER);
}

void draw() {
	background(200);

	char buff[256];
	sprintf_s(buff, 256, "%d", frameCount);
	text(buff, width / 2, height / 2);

	sprintf_s(buff, 256, "FPS: %3.2f", frameCount/seconds());
	text(buff, width / 2, (height / 2)+34);
}