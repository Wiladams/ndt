#include "apphost.h"

#include "rangen.h"
#include "graphic.hpp"
#include "Surface.h"
#include "desktopmanager.h"
#include "elements/framestats.h"

#include "shapes.h"


FrameStats _stats;

using namespace shapes;


constexpr size_t NUMCIRCLES = 1000;
constexpr float RADIUS = 20;
LONG WIDTH = 800;
LONG HEIGHT = 800;

std::shared_ptr<Surface> gAppSurface;
static std::shared_ptr<DesktopManager> gDesktopManager;



void frameTick(const FrameCountEvent& fce)
{
	if (isLayered())
		gAppSurface->clear();
	//printf("Frame:[%d]\n", frameCount());

	//gAppSurface->background(Pixel(255, 255, 255, 20));
	gDesktopManager->draw(*gAppSurface);
	gAppSurface->flush();

	screenRefresh();
}

void onLoad()
{
	frameRate(30);

	//createAppWindow(WIDTH, HEIGHT, "mousetrack");
	createAppWindow(displayWidth, displayHeight, "mousetrack");
	setCanvasPosition(0, 0);
	layered();

	gAppSurface = std::make_shared<Surface>();
	gAppSurface->attachPixelArray(appFrameBuffer(), 8);
	gAppSurface->textFont("Consolas");

	gDesktopManager = std::make_shared<DesktopManager>(maths::bbox2f{ {0,0},{(float)WIDTH, (float)HEIGHT} });

	// Create some random circles
	for (size_t i = 1; i <= NUMCIRCLES; i++)
	{
		auto acircle = std::make_shared<GCircle>(maths::vec2f{random(RADIUS,canvasWidth- RADIUS),random(RADIUS,canvasWidth - RADIUS) }, RADIUS);

		gDesktopManager->addChild(acircle);
	}
	auto gstats = std::make_shared<FrameStats>();
	gDesktopManager->addChild(gstats);

	subscribe(frameTick);

}