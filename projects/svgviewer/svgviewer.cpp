#include "p5.hpp"
#include "svgbuilder.hpp"

using namespace p5;
using namespace alib;

void drawShapes()
{
	// Create some shapes to draw
	
	SVGPathBuilder sb;


	// Random concave polygon
	//sb.moveTo(10, 10);
	//sb.lineTo(20, 10);
	//sb.lineTo(20, 20);
	//sb.lineTo(10, 20);
	//sb.close();


	// Create a tabbed panel using quadratic bezier
	// and lineTo
	sb.moveTo(10, 440);
	sb.lineTo(100, 440);

	sb.quadTo(100, 420, 108, 420);
	//sb.lineTo(100, 420);
	//sb.lineTo(120, 420);

	sb.lineTo(200, 420);

	sb.quadTo(220, 420, 220, 440);
	//sb.lineTo(220, 420);
	//sb.lineTo(220, 440);

	sb.lineTo(300, 440);
	sb.lineTo(300, 480);
	sb.lineTo(10, 480);
	sb.close();

	fill(0xff, 0xff, 0x00);

	auto p = sb.getPath();
	path(p);

}


void setup()
{
	createCanvas(800, 800);
	
	drawShapes();
}