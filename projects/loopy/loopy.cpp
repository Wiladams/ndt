#include "apphost.h"

static uint64_t counter = 0;

void fullScreen() noexcept
{

    setCanvasSize(displayWidth, displayHeight);
    setWindowPosition(0, 0);
    layered();
}

void appWindow(int w, int h)
{
	setCanvasSize(w, h);
	layered();
}

void onLoad()
{
	//fullScreen();
	appWindow(800, 800);

	gAppSurface->translate(canvasWidth / 2, canvasHeight / 2);
	showAppWindow();
}

/*
	Test the quality of the 'onLoop()' function.
	This comes from apphost, and is dynamically loaded from the 
	appmain file.

	The timing here is nothing regular.  onLoop() is really only
	useful to tell you how often we're going through the main
	application loop (getting Windows Messages).  

	You can do other things with this knowledge other than trying
	to do drawing.
*/
constexpr double PI_EXP = 3.14159;

void onLoop()
{
	gAppSurface->clear();
	//gAppSurface->clearRect(0, 0, canvasWidth, canvasHeight);
	gAppSurface->flush();

	gAppSurface->rotate(2.0 * PI_EXP / 360.0 / 1.0);

	// fat yellow line
	gAppSurface->strokeWeight(8.0);
	gAppSurface->stroke(Pixel(0xffffff00));
	gAppSurface->line(0, 0, canvasWidth / 2, 0);
	
	// Then thin red line
	gAppSurface->strokeWeight(4.0);
	gAppSurface->stroke(Pixel(0xffff0000));
	gAppSurface->line(0, 0, canvasWidth / 2, 0);
	
	// Make sure all drawing is actually comitted
	gAppSurface->flush();

	screenRefresh();

}