#include "p5.hpp"

#include "displaydevice.h"
#include "elements/gmonitor.h"
#include "elements/screensnapshot.hpp"

#include <memory>

using namespace p5;





struct MonitorSnapshot : public GraphicElement
{
	// Has a snapper
	ScreenSnapper fSnapper{};

	// Has a GMonitor
	GMonitor& fMonitor;
	
	MonitorSnapshot(GMonitor &monitor) : fMonitor(monitor) 
	{ 
		fSnapper.reset(0, 0, fMonitor.frameWidth(), fMonitor.frameHeight(), fMonitor.getMonitorDC());
		setFrame(monitor.frame());
		setBounds(monitor.bounds());
	}


	void draw(IGraphics& ctx)
	{
		fSnapper.next();
		//image(snapper.getImage(), 0, 0);
		
		//scaleImage(fSnapper.getImage(), 0, 0, fSnapper.width(), fSnapper.height(),
		//	0, 0, boundsWidth(), boundsHeight());
		scaleImage(fSnapper.getImage(), 0, 0, fSnapper.width(), fSnapper.height(),
			frameX(), frameY(), frameWidth(), frameHeight());

	}
};


std::vector<GMonitor> mons{};
std::vector<std::shared_ptr<MonitorSnapshot>> snappers{};

maths::bbox2f vbox;
float aspect = 1.0f;
float xscale = 1.0f;
float yscale = 1.0f;


// Do some drawing on top of everything else
/*
void onComposed()
{
	// Draw axis
	stroke(255, 255, 0);
	strokeWeight(8);

	push();
	
	scale(xscale, yscale);
	translate(-vbox.min.x, -vbox.min.y);

	line(vbox.min.x, 0, vbox.max.x, 0); // horizontal
	line(0, vbox.min.y, 0, vbox.max.y);	// vertical
	
	pop();
}
*/
//
// The guts of drawing
//
void draw()
{
	background(0xc0);

	push();

	scale(xscale, yscale);
	translate(-vbox.min.x, -vbox.min.y);

	for (auto & snapper : snappers)
	{
		snapper->draw(*gAppSurface);
	}

	pop();
}

void setup()
{
	// Get list of monitors
	// And the size of the virtual space they take up
	//DisplayDevice::interfaces();

	printf("============== DISPLAYS ===============\n");
	printf("Number Of Monitors: %d\n", GMonitor::numberOfMonitors());
	vbox = GMonitor::virtualDisplayBox();
	auto vsize = maths::size(vbox);
	aspect = vsize.x / vsize.y;


	float desiredWidth = 1920;
	float desiredHeight = desiredWidth * (1.0f / aspect);

	createCanvas((int)desiredWidth, (int)desiredHeight, "displays", 6);
	frameRate(30);

	// Figure out the monitors
	// and create a MonitorSnapshot for each
	xscale = desiredWidth / vsize.x;
	yscale = desiredHeight / vsize.y;

	//printf("display box: %3.f, %3.0f  %3.f, %3.0f\n", vbox.min.x, vbox.min.y, vbox.max.x, vbox.max.y);
	//printf("ASPECT: %3.2f\n", aspect);
	//printf("Scale: %3.2f  %3.2f\n", xscale, yscale);


	vbox = GMonitor::monitors(mons);

	//scale(xscale, yscale);
	//translate(-vbox.min.x, -vbox.min.y);

	// Construct the individual snapshots
	for (auto& mon : mons)
	{
		auto snapper = std::make_shared<MonitorSnapshot>(mon);
		//auto win = window(snapper->frameX(), snapper->frameY(), snapper->frameWidth(), snapper->frameHeight());
		//win->addChild(snapper);

		snappers.push_back(snapper);
	}



	//noLoop();
}