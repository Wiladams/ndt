#include "apphost.h"

#include "GraphicsDecoder.hpp"
#include "GraphicsEncoder.hpp"
#include "binstream.hpp"
#include "blend2d.h"
#include "gpath.h"
#include "drawingtool.h"
#include "tabbedview.h"
#include "gview.h"

using namespace p5;


uint8_t encBuff[1024*64];
size_t encBuffLen = 1024 * 64;

BinStream bs(encBuff, encBuffLen);
//GraphicsEncoder enc(bs);


//BinStream subrange = bs.range(0, bs.tell());
//GraphicsDecoder dec(subrange, gAppSurface);
//dec.run();

GPath mainPath;
std::shared_ptr<DrawingTool> currentTool = nullptr;

BLRect appArea;
Pixel appTabColor(245, 246, 247);
Pixel appViewColor(203, 212, 228);

BLRect canvasArea;
BLRect canvasShadow;
Pixel canvasColor(255,255,255);
Pixel canvasShadowColor(187, 199, 215);

// Tabs for the toolbar
TabViewSet tabSet(800,100);

void draw()
{
	background(appTabColor);

	push();
	tabSet.draw(gAppSurface);

	// Draw App View
	noStroke();
	fill(appViewColor);
	rect(appArea.x, appArea.y, appArea.w, appArea.h);

	// Draw Canvas area
	// put a drop shadow behind it
	translate(appArea.x+8, appArea.y+8);
	// Draw canvas shadow
	fill(canvasShadowColor);
	rect(canvasShadow.x, canvasShadow.y, canvasShadow.w, canvasShadow.h);

	// draw actual canvas
	noStroke();
	fill(canvasColor);
	rect(canvasArea.x, canvasArea.y, canvasArea.w, canvasArea.h);
	


	// Draw the current path
	stroke(Pixel(0, 0, 0));
	path(mainPath);

	// Draw the current tool
	currentTool->draw(gAppSurface);
	pop();
}

void setup()
{
	createCanvas(800, 600);

	tabSet.addChild(std::make_shared<TabbedView>(BLRect(0, 4, (double)width, 96), BLRoundRect(68, 0, 48, 16, 1), "View", appTabColor, appTabColor));
	tabSet.addChild(std::make_shared<TabbedView>(BLRect(0, 4, (double)width, 96), BLRoundRect(16, 0, 48, 16, 1), "Home", appTabColor, appTabColor));

	appArea.x = 0;
	appArea.y = 96;
	appArea.w = width;
	appArea.h = height - appArea.y;

	canvasArea.x = 0;
	canvasArea.y = 0;
	canvasArea.w = 640;
	canvasArea.h = 480;

	canvasShadow.x = 8;
	canvasShadow.y = 8;
	canvasShadow.w = 640;
	canvasShadow.h = 480;

	//currentTool = std::make_shared<StraightLineTool>(mainPath);
	//currentTool = std::make_shared<RectangleTool>(mainPath);
	currentTool = std::make_shared<PencilTool>(mainPath);

}

MouseEvent adjustMouse(const MouseEvent& e)
{
	MouseEvent newEvent(e);
	newEvent.x -= appArea.x + 8;
	newEvent.y -= appArea.y + 8;

	return newEvent;
}

void mousePressed(const MouseEvent& e)
{
	// hand even to current tool
	// we should only pass along mouse events
	// that land on the canvas area
	currentTool->mousePressed(adjustMouse(e));
}

void mouseMoved(const MouseEvent& e)
{
	//printf("mouseMoved: %d, %d\n", e.x, e.y);
	currentTool->mouseMoved(adjustMouse(e));
}

void mouseReleased(const MouseEvent& e)
{
	currentTool->mouseReleased(adjustMouse(e));
}