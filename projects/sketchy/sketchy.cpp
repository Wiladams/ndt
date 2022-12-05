
#include "p5.hpp"
#include "binstream.hpp"
#include "gpath.h"

#include "elements/tabbedview.h"

#include "drawingtool.h"
#include "sketchpage.h"

#include <memory>

using namespace p5;
using std::shared_ptr;
using std::make_shared;


Pixel appTabColor(245, 246, 247);
Pixel appViewColor(200, 210, 226);


shared_ptr<SketchPage> page = nullptr;
shared_ptr<TabViewSet> tabSet = nullptr;
shared_ptr<DrawingTool> currentTool = nullptr;

void draw()
{
	background(appViewColor);
}

void setup()
{
	createCanvas(800, 600);

	auto mainWin = window(0, 0, canvasWidth, canvasHeight);
	mainWin->setLayout(std::make_shared<ColumnLayout>((float)canvasWidth));
	mainWin->setBackgroundColor(appViewColor);

	//currentTool = std::make_shared<StraightLineTool>();
	//currentTool = make_shared<RectangleTool>();
	currentTool = make_shared<PencilTool>();

	// Tabs for the toolbar
	tabSet = make_shared<TabViewSet>(800, 100);

	tabSet->addGraphic(make_shared<TabbedView>(BLRect(0, 4, (double)canvasWidth, 96), BLRoundRect(68, 0, 48, 16, 1), "File", appTabColor, appTabColor));
	tabSet->addGraphic(make_shared<TabbedView>(BLRect(0, 4, (double)canvasWidth, 96), BLRoundRect(68, 0, 48, 16, 1), "Home", appTabColor, appTabColor));
	tabSet->addGraphic(make_shared<TabbedView>(BLRect(0, 4, (double)canvasWidth, 96), BLRoundRect(16, 0, 48, 16, 1), "View", appTabColor, appTabColor));

	page = std::make_shared<SketchPage>(0,0,640,480);
	page->setTool(currentTool);

	mainWin->addGraphic(tabSet);
	mainWin->addGraphic(page);
}


