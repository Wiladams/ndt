#include "p5.hpp"

#include "CalendarMonth.hpp"
#include "yearofmonths.h"
#include "Recorder.hpp"

using namespace p5; 
using namespace Calendar;


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

void mouseMoved(const MouseEvent& e)
{
	//printf("%d,%d  %d\n", e.x, e.y, e.shift);
}

void mouseDragged(const MouseEvent& e)
{
	int deltaX = e.x - pmouseX;
	int deltaY = e.y - pmouseY;

	//printf("mouseDragged: %d %d\n", deltaX, deltaY);

	double scrollSize = 10;

	cent.translateBy(deltaX, deltaY);
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

void draw()
{
	background(255, 255,240);

	gAppSurface->push();
	gAppSurface->scale(gScale);

	cent.draw(gAppSurface);

	recorder->saveFrame();
	
	gAppSurface->pop();

}

void setup()
{
	createCanvas(1280, 1024);

	recorder = std::make_shared<Recorder>(gAppSurface, "infinical-");
	//recorder->record();
}



void panMoved(const GestureEvent& e)
{
	cent.translateBy(panVecX, panVecY);
}


void zoomMoved(const GestureEvent& e)
{
	printf("zoomMoved: %d\n", e.distance);
}

