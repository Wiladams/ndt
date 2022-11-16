//
// monitorview
// 
// Target specific monitor to take a screen snapshot
//
#include "p5.hpp"
#include "elements/screensnapshot.hpp"
#include "elements/gmonitor.h"

#include <memory>

using namespace p5;

ScreenSnapper snapper;

void draw()
{
	auto success = snapper.next();

	//image(snapper.getImage(), 0, 0);
	scaleImage(snapper.getImage(), 0, 0, snapper.getImage().width(), snapper.getImage().height(),
		0, 0, canvasWidth, canvasHeight);
}

void setup()
{
	createCanvas(800, 600, "monitorview", 4);

	// Get monitor we want to capture
	// 
	std::vector<GMonitor> mons{};
	auto bbox = GMonitor::monitors(mons);

	printf("VBox: %3.0f,%3.0f  %3.0f,%3.0f\n", bbox.min.x, bbox.min.y, bbox.max.x, bbox.max.y);

	// Setup the screen snapshot
	int numMons = mons.size();
	auto& myMon = mons[1];

	//snapper.reset(myMon.frameX(), myMon.frameY(), 800, 600, myMon.getMonitorDC());
	snapper.reset(0, 0, myMon.frameWidth(), myMon.frameHeight(), myMon.getMonitorDC());

	//noLoop();
}