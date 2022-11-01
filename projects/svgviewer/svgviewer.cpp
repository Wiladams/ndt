#include "p5.hpp"
#include "svgbuilder.hpp"
#include "svgdocument.h"

using namespace p5;

SVGDocument sdoc;

void setup()
{
	createCanvas(800, 800);
	
	sdoc.reset("./resources/tango.svg");

	if (sdoc.isValid())
	{
		SvgVisitor v(*gAppSurface);
		sdoc.fDoc.Accept(&v);
	}

	noLoop();
}