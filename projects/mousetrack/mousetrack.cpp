#include "studio.hpp"

#include "elements/framestats.h"

#include "shapes.h"

using namespace shapes;


constexpr size_t NUMCIRCLES = 1000;
constexpr float RADIUS = 20;

void setup()
{
	// Create some random circles
	for (size_t i = 1; i <= NUMCIRCLES; i++)
	{
		auto acircle = std::make_shared<GCircle>(maths::vec2f{random(RADIUS,canvasWidth- RADIUS),random(RADIUS,canvasWidth - RADIUS) }, RADIUS);

		addGraphic(acircle);
	}
}