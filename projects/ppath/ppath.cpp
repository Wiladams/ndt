#include "apphost.h"

// Create a simple application window
void appWindow(int w, int h)
{
	setCanvasSize(w, h);
	gAppWindow->setCanvasSize(w, h);
	showAppWindow();
}

// onLoad() will be called when the runtime has started,
// and just before it gets into the endless application loop
void onLoad()
{
	appWindow(800, 600);
}


// This function is called every time through the runtime's 
// application loop
void onLoop()
{
	// make sure the canvas is clear
	gAppSurface->clear();
	gAppSurface->flush();

	// fat yellow line
	gAppSurface->strokeWeight(40.0);
	gAppSurface->stroke(Pixel(0xffffff00));
	gAppSurface->line(20, 40, canvasWidth / 2, 40);

	// Then thin red line
	gAppSurface->strokeWeight(20.0);
	gAppSurface->stroke(Pixel(0xffff0000));
	gAppSurface->line(20, 40, canvasWidth / 2, 40);

	// Make sure all drawing is actually comitted
	gAppSurface->flush();

	screenRefresh();
}
