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

		setFrame({ 0,0,3200,3200 });
	}
};

Decade dcade(2020);
std::shared_ptr<Recorder> recorder;

double gScaleFactor = 0.1;
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
	dcade.translateBy(deltaX, deltaY);
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
	dcade.draw(gAppSurface);

	recorder->saveFrame();
	
	gAppSurface->pop();

}

void setup()
{
	createCanvas(800, 600);

	
	recorder = std::make_shared<Recorder>(gAppSurface, "infinical-");
	//recorder->record();
}