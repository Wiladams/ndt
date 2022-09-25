#include "p5.hpp"
#include "angleindicator.hpp"

using namespace p5;
std::shared_ptr<AngleIndicator> indic;
float ang = 0;

void draw()
{
	background(225);

	indic->setAngle(maths::Radians(ang));

	ang++;
}

void setup()
{
	createCanvas(600, 600, "angler");
	frameRate(1);

	auto win1 = window(10,10,400,400);

	indic = std::make_shared<AngleIndicator>(ang);
	win1->addChild(indic);
}