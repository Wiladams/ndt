/*
	infinical

	An infinite scrolling calendar.  It actually only displays 1000 years

	UI:
	Zoom In
	  Up Arrow
	  Mouse Wheel forward
	Zoom Out
	  Down Arrow
	  Mouse Wheel back
	Move
	  Mouse click/drag
	  pointer touch/drag
*/


#include "p5.hpp"

#include "CalendarMonth.hpp"
#include "yearofmonths.h"
#include "Recorder.hpp"

using namespace p5; 
using namespace Calendar;



//	class Decade
//
//	Display 10 years at a time
//
class Decade : public Graphic
{
	int fBaseYear;

public:
	Decade(int baseYear)
		:fBaseYear(baseYear)
	{
		// set a horizontal layout
		setLayout(std::make_shared<HorizontalLayout>());

		// Add children to the graphic
		for (int year = baseYear; year < baseYear + 10; year++) 
		{
			addChild(std::make_shared<YearOfMonths>(year));
		}

		auto sz = YearOfMonths::getPreferredSize();
		setFrame({ 0,0,(double)(sz.w*10),(double)(sz.h) });
	}
};

//
// class Century
//
// Display 100 years at a time
//
class Century : public Graphic
{
	int fBaseYear;

public:
	Century(int baseYear) :
		fBaseYear(baseYear)
	{
		setLayout(std::make_shared < VerticalLayout>());

		// Add decades to the graphic
		for (int year = baseYear; year < (baseYear + 100); year += 10) {
			addChild(std::make_shared<Decade>(year));
		}
	}
};


Century cent(2000);
std::shared_ptr<Recorder> recorder;

double gScaleFactor = 0.05;
double gScale = 1.0;



void setup()
{
	createCanvas(1280, 1024, "infinical");

	recorder = std::make_shared<Recorder>(gAppSurface, "infinical-");
	//recorder->record();
}

void draw()
{
	background(255, 255, 240);

	push();

	scale(gScale);

	cent.draw(gAppSurface);

	recorder->saveFrame();

	pop();
}



void keyPressed(const KeyboardEvent& e)
{
	switch (e.keyCode) {
		// left 
		// right

		// up - zoom in
	case VK_UP:
		gScale += gScaleFactor;
		break;

	case VK_DOWN:
		// down - zoom out
		gScale -= gScaleFactor;
		break;
	}
}

void keyReleased(const KeyboardEvent& e)
{
	switch (e.keyCode) {
	case VK_SPACE:
		// toggle recording
		recorder->toggleRecording();

		break;
	}
}

//
// mouseWheel
// We'll use this to zoom in and out
//
void mouseWheel(const MouseEvent& e)
{
	if (e.delta > 0)
		gScale += gScaleFactor;
	else
		gScale -= gScaleFactor;
}


// Do panning using a mouse drag
void mouseDragged(const MouseEvent& e)
{
	int deltaX = e.x - pmouseX;
	int deltaY = e.y - pmouseY;

	//printf("mouseDragged: %d %d\n", deltaX, deltaY);

	double scrollSize = 10;

	cent.translateBy(deltaX, deltaY);
}

// Handling panning
// do translation
void panMoved(const GestureEvent& e)
{
	cent.translateBy(panVecX, panVecY);
}

// Handle zooming gesture
// turn it into scaling
void zoomMoved(const GestureEvent& e)
{
	printf("zoomMoved: %d\n", e.distance);
}

