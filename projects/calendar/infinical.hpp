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

#include "MonthTile.hpp"
#include "yearofmonths.h"


using namespace p5;
using namespace Calendar;








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
	//background(255, 255, 240);
	background(255);

	push();

	scale(gScale);

	cent.draw(*gAppSurface);

	recorder->saveFrame();

	pop();

	noLoop();
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
	case 'R':
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

void mouseHWheel(const MouseEvent& e)
{
	double scrollSize = 10;

	if (e.delta > 0)
		cent.translateBoundsBy(scrollSize, 0);
	else
		cent.translateBoundsBy(-scrollSize, 0);
}


// Do panning using a mouse drag
void mouseDragged(const MouseEvent& e)
{
	int deltaX = e.x - pmouseX;
	int deltaY = e.y - pmouseY;

	//printf("mouseDragged: %d %d\n", deltaX, deltaY);

	//double scrollSize = 10;

	cent.translateBoundsBy(deltaX, deltaY);
}

// Handling panning
// do translation
void panMoved(const GestureEvent& e)
{
	cent.translateBoundsBy(panVecX, panVecY);
}

// Handle zooming gesture
// turn it into scaling
void zoomMoved(const GestureEvent& e)
{
	printf("zoomMoved: %d\n", e.distance);
}

