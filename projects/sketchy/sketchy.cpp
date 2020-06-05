#include "apphost.h"
//#include "p5.hpp"
#include "GraphicsDecoder.hpp"
#include "GraphicsEncoder.hpp"
#include "binstream.hpp"

using namespace p5;

GraphicsEncoder* enc;
GraphicsDecoder* dec;


void setup()
{
	setCanvasSize(800, 600);
	gAppWindow->setCanvasSize(canvasWidth, canvasHeight);

	show();
}