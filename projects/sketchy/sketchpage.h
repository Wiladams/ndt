#pragma once

#include "gpath.h"
#include "drawingtool.h"

// A single graphic responsible for 
// handling tools drawing on its surface
const Pixel canvasColor = Pixel(255, 255, 255);
const Pixel canvasShadowColor = Pixel(187, 199, 215);

struct SketchPage : public GraphicElement
{
	Pixel fCanvasColor{ 225, 225, 225,255 };
	GPath fPagePath;
	std::shared_ptr<DrawingTool> fCurrentTool = nullptr;

	SketchPage(float x, float y, float w, float h)
		:GraphicElement(x, y, w, h)
	{}

	void setTool(std::shared_ptr<DrawingTool> tool)
	{
		fCurrentTool = tool;
	}

	void drawSelf(IGraphics& ctx)
	{
		// draw actual canvas
		//noStroke();
		ctx.stroke(0);
		ctx.fill(fCanvasColor);
		ctx.rect(boundsX(), boundsY(), boundsWidth(), boundsHeight());

		// Draw the current path
		ctx.stroke(Pixel(0, 0, 0));
		ctx.path(fPagePath);

		// Draw the current tool
		fCurrentTool->draw(ctx);
	}

	void mousePressed(const MouseEvent& e)
	{
		//printf("SketchPage::mousePressed()\n");
		auto& f = frame();
		auto& b = bounds();

		if (fCurrentTool != nullptr)
			fCurrentTool->mousePressed(e);
	}

	void mouseMoved(const MouseEvent& e)
	{
		//printf("SketchPage::mouseMoved: %d, %d\n", e.x, e.y);
		if (fCurrentTool != nullptr)
			fCurrentTool->mouseMoved(e);
	}

	void mouseReleased(const MouseEvent& e)
	{
		//printf("SketchPage::mouseReleased: %d, %d\n", e.x, e.y);
		if (fCurrentTool != nullptr)
		{
			fCurrentTool->mouseReleased(e);
			fPagePath.addPath(fCurrentTool->figure());
		}
	}

};
