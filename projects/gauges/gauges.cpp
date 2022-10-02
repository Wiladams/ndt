#include "p5.hpp"
#include "segmenttick.h"
#include "Recorder.hpp"
#include "slider.h"


using namespace p5;


std::shared_ptr<SegmentGauge> seggauge = nullptr;
std::shared_ptr<Recorder> recorder = nullptr;
std::shared_ptr<Slider> sld = nullptr;

double gProgress = 0.0;
double gDir = 1.0;
double gIncrement = 0.01;


void progressChanged(const Topic<BLPoint> &pub, BLPoint pos)
{
	//printf("Position: %f, %f\n", pos.x, pos.y);
	gProgress = pos.x;
	if (seggauge != nullptr)
		seggauge->setProgress(gProgress);
}

void onComposed()
{
	if (recorder != nullptr)
		recorder->saveFrame();
}



void draw()
{
	background(0);
}

void setup()
{
	createCanvas(400, 400, "gauges");
	frameRate(20);

	recorder = std::make_shared<Recorder>(gAppSurface, "gauges-");

	//loadDefaultFonts();
	gAppSurface->textFont("Consolas");

	gAppSurface->textSize(24);
	gAppSurface->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);

	auto win1 = window(0, 0, canvasWidth, canvasHeight);
	win1->setBackgroundColor(Pixel(0,0,0));

	seggauge = std::make_shared<SegmentGauge>(150.0, canvasWidth/2.0, canvasHeight/2.0);
	seggauge->setSegmentThickness(64.0);

	sld = Slider::create(BLPoint(10, canvasHeight-30.0), BLPoint(canvasWidth - 10, canvasHeight-30));
	sld->subscribe(progressChanged);

	win1->addChild(seggauge);
	win1->addChild(sld);

	//recorder->record();
}