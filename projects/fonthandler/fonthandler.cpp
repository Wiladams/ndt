#include "p5.hpp"

void setup()
{
	BLFontFace face;
	gFontHandler->queryFontFace("Tahoma", face);

	printf("isvalid: %d\n", face.isValid());
}