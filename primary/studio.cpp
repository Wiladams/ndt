#include "studio.hpp"
#include "desktopmanager.h"
#include "Surface.h"

#include <memory>

using std::shared_ptr;
using std::unique_ptr;

static unique_ptr<Surface> gAppSurface{};
static shared_ptr<GraphicGroup> gGroup{};
static unique_ptr<DesktopBehavior> gDesktopBehavior{};
static std::function<void(IGraphics& ctx) > gDesktopDrawer{};
static std::function<void(IGraphics& ctx, std::shared_ptr<GraphicGroup> gs) > gWindowDrawer{};

static VOIDROUTINE gSetupHandler = nullptr;


//
// ======== User Facing Routines ========= 
//
void addGraphic(std::shared_ptr<GraphicElement> g)
{
	gGroup->addChild(g);
}

void setDesktopDrawing(std::function<void(IGraphics& ctx)> func)
{
	gDesktopDrawer = func;
}

void setWindowDrawing(std::function<void(IGraphics& ctx, std::shared_ptr<GraphicGroup> gs)> func)
{
	gWindowDrawer = func;
}

//
// ========= Internal routines ============
//
static void drawBackground()
{
	// Need to flush if we're doing multi-threaded
	gAppSurface->flush();

	if (gDesktopDrawer != nullptr) {
		gDesktopDrawer(*gAppSurface);
	}
	else {
		if (isLayered())
			gAppSurface->clear();
		else
			gAppSurface->background(Pixel(255, 255, 0, 255));
		
		// Need to flush if we're doing multi-threaded
		gAppSurface->flush();
	}


}

static void drawChildren()
{
	if (gWindowDrawer != nullptr)
	{
		gWindowDrawer(*gAppSurface, gGroup);
	}
	else {
		gGroup->draw(*gAppSurface);
	}

	// Need to flush again just before going to the screen
	gAppSurface->flush();
}

static void frameTick(const FrameCountEvent& fce)
{
	drawBackground();

	drawChildren();

	// ensure drawing gets to the actual screen
	screenRefresh();
}

void defaultWindowDrawing(IGraphics& ctx, std::shared_ptr<GraphicGroup> gs)
{
	gs->draw(ctx);
}



void setupDynamicRoutines()
{
	HMODULE hInst = ::GetModuleHandleA(NULL);

	gSetupHandler = (VOIDROUTINE)GetProcAddress(hInst, "setup");
}

void setupSubscriptions()
{
	subscribe(frameTick);
}

void fullscreen(const char* title, uint32_t threadCount) noexcept
{
	//createCanvas(displayWidth, displayHeight, title, threadCount);
	createAppWindow(displayWidth, displayHeight, "studio");
	gAppSurface->attachPixelArray(appFrameBuffer(), threadCount);

	setCanvasPosition(0, 0);
	layered();
	//gIsFullscreen = true;
}

void onLoad()
{
	setupDynamicRoutines();
	setupSubscriptions();
	setWindowDrawing(defaultWindowDrawing);

	frameRate(15);

	createAppWindow(displayWidth/2, displayHeight/2, "studio");
	setCanvasPosition(0, 0);
	//layered();
	
	// Create the surface we'll be using for drawing
	gAppSurface = std::make_unique<Surface>();
	gAppSurface->attachPixelArray(appFrameBuffer(), 2);
	gAppSurface->textFont("Consolas");


	// Create the container that will hold all the graphics
	// that appear on the desktop
	gGroup = std::make_shared<GraphicGroup>(maths::bbox2f{ {0,0},{(float)canvasWidth, (float)canvasHeight} });
	
	// Create the behavior that will control UI actions in the 
	// desktop
	gDesktopBehavior = std::make_unique<DesktopBehavior>(*gGroup);

	if (gSetupHandler)
		gSetupHandler();
}
