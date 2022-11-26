//
// monitorview
// 
// Turn each monitor into a moveable graphic window
//
#include "studio.hpp"

#include "elements/displayview.hpp"

#include <memory>

/*
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
*/


std::vector<DisplayMonitor> mons{};

void setup()
{
	// Get list of all connected monitors
	// 
	auto bbox = DisplayMonitor::monitors(mons);

	// Construct the individual snapshots
	float xoffset = 100;
	float yoffset = 100;
	for (auto& mon : mons)
	{
		auto snap = std::make_shared<DisplayView>(mon, maths::bbox2f{ {xoffset,yoffset},{xoffset+640,xoffset+480} });
		// Assign a UIbehavior to the graphic so it can move around

		addGraphic(snap);

		xoffset += 200;
		yoffset += 200;
	}
}