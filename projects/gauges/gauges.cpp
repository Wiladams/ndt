#include "p5.hpp"
#include "Recorder.hpp"

#include "segmentgauge.h"
#include "slider.h"


using namespace p5;


std::shared_ptr<SegmentGauge> seggauge = nullptr;
std::shared_ptr<Recorder> recorder = nullptr;


void progressChanged(const float pos)
{
	//printf("Position: %f\n", pos);
	if (seggauge != nullptr)
		seggauge->setProgress(pos);
}

void onComposed()
{
	if (recorder != nullptr)
		recorder->saveFrame();
}

void draw()
{
	if (isLayered)
		clear();
	else
		background(127);
}

void setup()
{
	createCanvas(400, 400, "gauges");
	//frameRate(20);
	//layered();

	recorder = std::make_shared<Recorder>(gAppSurface, "gauges-");

	//gAppSurface->textSize(24);
	//gAppSurface->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);

	auto win1 = window(0, 0, canvasWidth, canvasHeight);
	win1->setBackgroundColor(Pixel(0,50,50,10));

	seggauge = std::make_shared<SegmentGauge>(BLRect(4,4, 300,300), 40);

	auto sld = Slider::create({ 10, float(canvasHeight - 30) }, { float(canvasWidth - 10), float(canvasHeight - 30) });
	sld->subscribe(progressChanged);

	win1->addChild(seggauge);
	win1->addChild(sld);

	//recorder->record();
}