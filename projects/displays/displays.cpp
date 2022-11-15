#include "p5.hpp"

#include "displaydevice.h"
#include "elements/gmonitor.h"

#include <memory>

using namespace p5;


std::vector<std::shared_ptr<GMonitor> > mons{};
maths::bbox2f vbox;
float aspect = 1.0f;
float xscale = 1.0f;
float yscale = 1.0f;

// Do some drawing on top of everything else
void onComposed()
	{
		// Draw axis
		stroke(255, 255, 0);
		strokeWeight(8);

		push();
		translate(-aspect * vbox.min.x, -aspect * vbox.min.y);
		//scale(xscale, yscale);
		line(vbox.min.x, 0, vbox.max.x, 0); // horizontal
		line(0, vbox.min.y, 0, vbox.max.y);	// vertical
		pop();
	}

	//
	// The guts of drawing
	//
void draw()
	{
		printf("VBOX: %3.0f, %3.0f, %3.0f, %3.0f\n", vbox.min.x, vbox.min.y, vbox.max.x, vbox.max.y);
		printf("ASPECT: %3.2f, LEFT: %3.2f\n", aspect, aspect * vbox.min.x);

		background(0xc0);

		push();
		translate(-aspect*vbox.min.x, -aspect*vbox.min.y);
	

		scale(xscale, yscale);


		for (auto& g : mons)
		{
			g->draw(*gAppSurface);
			g->print();
		}

		pop();
	}

void setup()
	{
		// Get list of monitors
		// And the size of the virtual space they take up
		DisplayDevice::interfaces();

		printf("============== MONITORS ===============\n");
		printf("Number Of Monitors: %d\n", GMonitor::numberOfMonitors());
		auto vdbox = GMonitor::virtualDisplayBox();
		printf("display box: %3.f, %3.0f  %3.f, %3.0f\n", vdbox.min.x, vdbox.min.y, vdbox.max.x, vdbox.max.y);

		vbox = GMonitor::monitors(mons);
		auto vsize = maths::size(vbox);

		aspect = vsize.y / vsize.x;
		float desiredWidth = displayWidth;
		xscale = desiredWidth / vsize.x;


		float desiredHeight = desiredWidth * aspect;
		yscale = desiredHeight / vsize.y;

		//printf("Virtual Screen Size: %3.0fx%3.0f\n", vsize.x, vsize.y);
		//printf("Aspect: %3.2f\n", aspect);

		createCanvas((int)desiredWidth, (int)desiredHeight, "displays");



		noLoop();
	}