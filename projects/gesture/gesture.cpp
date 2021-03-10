#include "p5.hpp"


using namespace p5;


void setup()
{
	createCanvas(1920, 1080);
	//fullscreen();

	//frameRate(30);

	// make sure we're not doing touch
	auto isTouchy = noTouch();
	std::cout << "isTouchy: " << isTouchy << std::endl;
}

void panStarted(const GestureEvent& e)
{
	printf("panStarted: (%d, %d)\n", e.x, e.y);

}

void panMoved(const GestureEvent& e)
{
	printf("panMoved: (%d, %d) - (%d, %d)\n", e.x, e.y, panVecX, panVecY);
}

void panEnded(const GestureEvent& e)
{
	printf("panEnded: (%d, %d)\n", e.x, e.y);

}
