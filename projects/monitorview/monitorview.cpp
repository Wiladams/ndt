#include "p5.hpp"
#include "elements/screensnapshot.hpp"

#include <memory>

using namespace p5;

ScreenSnapper snapper;

void setup()
{
	BLVar style;

	BLRgba32 c(255, 0, 0, 255);

	blVarAssignRgba32(&style, c.value);

	printf("isStyle: %d\n", style.isStyle());
}