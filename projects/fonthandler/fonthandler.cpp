#include "p5.hpp"

void setup()
{
	BLFontFace face;
	face = gFontHandler->queryFontFace("Tahoma");

	printf("isvalid: %d\n", face.isValid());
}