#include "p5.hpp"

#include "segmentgauge.h"
#include "elements/slider.h"


using namespace p5;


std::shared_ptr<SegmentGauge> seggauge = nullptr;

void progressChanged(const float pos)
{
	//printf("Position: %f\n", pos);
	if (seggauge != nullptr)
		seggauge->setProgress(pos);
}

void draw()
{
	// For effect
	// when displaying with layering, just have a transparent
	// background, so desktop shows
	// when in a window, use a semi-transparent color
	// to see it fade over time
	//if (isLayered())
		clear();
	//else
		background(0, 50, 50, 10);
}

void setup()
{
	//createCanvas(400, 400, "gauges");
	frameRate(30);
	fullscreen();

	seggauge = std::make_shared<SegmentGauge>(BLRect(4,4, 300,300), 40);

	auto sld = Slider::create({ 10, float(canvasHeight - 30) }, { float(canvasWidth - 10), float(canvasHeight - 30) });
	sld->subscribe(progressChanged);

	addWindow(sld);
	addWindow(seggauge);
}