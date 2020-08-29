#pragma once

#include "blend2d.h"
#include "graphic.hpp"
#include "gpath.h"

#include <string>




class TabbedView : public Graphic {

	//BLRect fFrame;		// frame for the whole graphic
	
	// Content Area details
	GPath fWholePath;
	BLRgba32 fBackgroundColor;
	BLRect fContentArea;

	// Tab details
	BLRoundRect fTabParam;
	GPath fTabPath;
	BLRgba32 fTabColor;
	BLRect fTabContentArea;
	std::string fTabTitle;

	static const int ContentMargin=4;

public:
	TabbedView(const BLRect& frame, const BLRoundRect &tParam, const std::string& title, const Pixel& tColor = { 245, 246, 247 }, const Pixel& bColor = { 245, 246, 247 })
		: Graphic(frame),
		fTabParam(tParam),
		fTabColor(tColor),
		fTabTitle(title),
		fBackgroundColor(bColor)
	{
		fTabPath.moveTo(fFrame.x+fTabParam.x, fFrame.y + fTabParam.h);
		fTabPath.cubicBy(fTabParam.rx, 0, 0, -fTabParam.h, fTabParam.rx, 0);
		fTabPath.lineBy(fTabParam.w, 0);
		fTabPath.cubicBy(fTabParam.rx, 0, 0, fTabParam.h, fTabParam.rx, 0);
		fTabPath.close();
		fTabContentArea = { fFrame.x + fTabParam.x + fTabParam.rx*2, fFrame.y + 2, fTabParam.w, fTabParam.h - 4 };
		
		fWholePath.moveTo(fFrame.x, fFrame.y + fTabParam.h);
		fWholePath.lineBy(fTabParam.x, 0);
		fWholePath.cubicBy(fTabParam.rx, 0, 0, -fTabParam.h, fTabParam.rx, 0);
		fWholePath.lineBy(fTabParam.w, 0);
		fWholePath.cubicBy(fTabParam.rx, 0,  0, fTabParam.h, fTabParam.rx, 0);
		fWholePath.lineTo(fFrame.x + fFrame.w, fTabParam.h);
		fWholePath.lineTo(fFrame.x + fFrame.w, fTabParam.h + fFrame.h - fTabParam.h);
		fWholePath.lineTo(fFrame.x, fTabParam.h + fFrame.h - fTabParam.h);
		fWholePath.close();
		fContentArea = { fFrame.x + ContentMargin, fFrame.y + fTabParam.h + ContentMargin, fFrame.w - ContentMargin*2, fFrame.h - fTabParam.h-ContentMargin*2 };
	}

	void setBackgroundColor(const Pixel& c) { fBackgroundColor = c; }
	void setTabColor(const Pixel& c) { fTabColor = c; }

	void drawBackground(IGraphics* ctx)
	//virtual void draw(IGraphics* ctx) 
	{
		// Stroke the overall outline
		// Stroke this first, and allow the filling to
		// cover some of the stroke
		ctx->blendMode(BL_COMP_OP_SRC_OVER);
		ctx->stroke(0);
		ctx->noFill();
		ctx->path(fWholePath);

		// Fill the overall background
		ctx->blendMode(BL_COMP_OP_SRC_OVER);
		ctx->noStroke();
		ctx->fill(fBackgroundColor);
		ctx->path(fWholePath);

		// Fill the tab area
		ctx->blendMode(BL_COMP_OP_SRC_OVER);
		ctx->noStroke();
		ctx->fill(fTabColor);
		ctx->path(fTabPath);




		// draw title centered on tab
		double titleX = fTabContentArea.x + fTabContentArea.w / 2;
		double titleY = fTabContentArea.y + fTabContentArea.h - 4;

		// Just to check on the size of the tabContentArea
		//ctx->stroke(255, 0, 0);
		//ctx->rect(fTabContentArea.x, fTabContentArea.y, fTabContentArea.w, fTabContentArea.h);
		
		// Put the text on the label
		ctx->noStroke();
		ctx->fill(0);
		ctx->textSize(fTabParam.h * .8);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx->text(fTabTitle.c_str(), titleX, titleY);


		// check size of content area
		//ctx->stroke(255, 0, 0);
		//ctx->noFill();
		//ctx->rect(fContentArea.x, fContentArea.y, fContentArea.w, fContentArea.h);

	}
};

class TabViewSet : public Graphic {
public:
	TabViewSet(const double w, const double h)
		:Graphic(0, 0, w, h)
	{}

	static std::shared_ptr<TabViewSet> create(const double w, const double h)
	{
		auto tset = std::make_shared<TabViewSet>(w, h);
		return tset;
	}

	void mousePressed(const MouseEvent& e)
	{
		// Figure out which window is being 
		// clicked
		auto g = graphicAt(e.x, e.y);

		// if not clicked on a view, then simply return
		if (nullptr == g) {
			//fActiveGraphic = nullptr;
			return;
		}

		// deactivate old active window

		// Activate new active window
		//setActiveGraphic(win);

		// bring it to the front
		moveToFront(g);
		g->mousePressed(e);
	}


};