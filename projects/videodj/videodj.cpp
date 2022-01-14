#include <memory>

#include "p5.hpp"
#include "screensnapshot.hpp"


using namespace maths;

using namespace p5;

ScreenSnapshot* ss = nullptr;

void keyReleased(const KeyboardEvent& e)
{
	if (e.keyCode == VK_ESCAPE)
		halt();

	//if (e.keyCode == VK_SPACE)
	//	reco->toggleRecording();
}

void drawPoints()
{
	noFill();
	noStroke();
	strokeWeight(0.5);

	// Draw some random sampled points
	for (int i = 0; i < 25000; i++)
	{
		auto x = random(canvasWidth - 1);
		auto y = random(canvasHeight - 1);
		double u = Map(x, 0, canvasWidth - 1, 0, 1);
		double v = Map(y, 0, canvasHeight - 1, 0, 1);

		// get a random sample from the screen capture
		auto c = ss->pixelValue(u, v, { x,y,0 });

		// turn that into a couple of styled lines
		int stemSize = 10;
		int stemHalf = stemSize / 2;

		fill(c);
		//stroke(c);
		//circle(x, y, stemSize);
		BLEllipse params(x,y,stemHalf);
		gAppSurface->getBlend2dContext().fillEllipse(params);
		//gAppSurface->getBlend2dContext().strokeEllipse(params);

		//triangle(x, y - stemHalf, x + stemHalf, y + stemHalf, x - stemHalf, y + stemHalf);

	}
}

// some other effects that can be done above
//gAppSurface->set(x, y, c);

//fillTriangle(*gAppSurface, x, y - stemHalf, x + stemHalf, y + stemHalf, x - stemHalf, y + stemHalf, c, { 0,0,canvasWidth,canvasHeight });

//fillRectangle(*gAppSurface, x - stemHalf, y, stemSize, 1, c);
//fillRectangle(*gAppSurface, x, y - stemHalf, 1, stemSize, c);

//ctx->point(x, y, *screenSamp);
//ctx->circle(x, y, 2, *screenSamp);

//fillCircle(*gAppSurface, x, y, 5, c);


void draw()
{
	ss->next();

	// start with blank slate
	background(0);

	drawPoints();

	// Draw a rectangle or two
	//ctx->rectangle(100, 100, 400, 300, *screenSamp);
	//ctx->rectangle(300, 400, 640, 480, *screenSamp);

	//reco->saveFrame();
}

void setup()
{
	frameRate(20);
	createCanvas(800, 600);

	ss = new ScreenSnapshot(100, 400, canvasWidth, canvasHeight);

	noStroke();

	// setup the recorder
	//reco = std::make_shared<Recorder>(&(*gAppSurface), "frame-", 0);
}