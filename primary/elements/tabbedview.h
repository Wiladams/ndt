#pragma once

#include "blend2d.h"
#include "graphic.hpp"
#include "gpath.h"

#include <string>


class TabbedView : public Graphic 
{
	static constexpr int ContentMargin = 4;

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



public:
	TabbedView(const BLRect& f, const BLRoundRect &tParam, const std::string& title, const Pixel& tColor = { 245, 246, 247 }, const Pixel& bColor = { 245, 246, 247 })
		: Graphic(f.x,f.y,f.w,f.h),
		fTabParam(tParam),
		fTabColor(tColor),
		fTabTitle(title),
		fBackgroundColor(bColor)
	{
		maths::bbox2f b = bounds();
		maths::vec2f bsz = maths::size(b);

		// Create the tab that sits on top
		fTabPath.moveTo(b.min.x + fTabParam.x, b.min.y + fTabParam.h);
		fTabPath.cubicBy(fTabParam.rx, 0, 0, -fTabParam.h, fTabParam.rx, 0);
		fTabPath.lineBy(fTabParam.w, 0);
		fTabPath.cubicBy(fTabParam.rx, 0, 0, fTabParam.h, fTabParam.rx, 0);
		fTabPath.close();
		fTabContentArea = { b.min.x + fTabParam.x + fTabParam.rx * 2, b.min.y + 2, fTabParam.w, fTabParam.h - 4};
		
		// Create the path including the tab
		fWholePath.moveTo(b.min.x, b.min.y + fTabParam.h);
		fWholePath.lineBy(fTabParam.x, 0);
		fWholePath.cubicBy(fTabParam.rx, 0, 0, -fTabParam.h, fTabParam.rx, 0);
		fWholePath.lineBy(fTabParam.w, 0);
		fWholePath.cubicBy(fTabParam.rx, 0,  0, fTabParam.h, fTabParam.rx, 0);
		fWholePath.lineTo(b.max.x, fTabParam.h);
		fWholePath.lineTo(b.max.x, fTabParam.h + bsz.y - fTabParam.h);
		fWholePath.lineTo(b.min.x, fTabParam.h + bsz.y - fTabParam.h);
		fWholePath.close();
		fContentArea = { b.min.x + ContentMargin, b.min.y + fTabParam.h + ContentMargin, bsz.x - ContentMargin * 2, bsz.y - fTabParam.h - ContentMargin * 2};
	}

	virtual bool contains(float x, float y) override
	{
		return fWholePath.contains(x, y);
	}

	void setBackgroundColor(const Pixel& c) { fBackgroundColor = c; }
	void setTabColor(const Pixel& c) { fTabColor = c; }

	void drawBackground(IGraphics &ctx) override
	{
		// Stroke the overall outline
		// Stroke this first, and allow the filling to
		// cover some of the stroke
		//ctx.blendMode(BL_COMP_OP_SRC_OVER);
		ctx.stroke(0);
		ctx.noFill();
		ctx.path(fWholePath);

		// Fill the overall background
		//ctx.blendMode(BL_COMP_OP_SRC_OVER);
		ctx.noStroke();
		ctx.fill(fBackgroundColor);
		ctx.path(fWholePath);

		// Fill the tab area
		//ctx.blendMode(BL_COMP_OP_SRC_OVER);
		ctx.noStroke();
		ctx.fill(fTabColor);
		ctx.path(fTabPath);

		// draw title centered on tab
		double titleX = fTabContentArea.x + fTabContentArea.w / 2;
		double titleY = fTabContentArea.y + fTabContentArea.h - 4;

		// Just to check on the size of the tabContentArea
		//ctx->stroke(255, 0, 0);
		//ctx->rect(fTabContentArea.x, fTabContentArea.y, fTabContentArea.w, fTabContentArea.h);
		
		// Put the text on the label
		ctx.noStroke();
		ctx.fill(0);
		ctx.textSize(fTabParam.h * .8);
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx.textFont("Consolas");
		ctx.text(fTabTitle.c_str(), titleX, titleY);


		// check size of content area
		//ctx.stroke(255, 0, 0);
		//ctx.noFill();
		//ctx.rect(fContentArea.x, fContentArea.y, fContentArea.w, fContentArea.h);

	}
};

class TabViewSet : public Graphic 
{
public:
	TabViewSet(const float w, const float h)
		:Graphic(0, 0, w, h)
	{}

	void mouseReleased(const MouseEvent& e) override
	{
		// Figure out which window is being 
		// clicked
		auto g = graphicAt(e.x, e.y);

		// if not clicked on a view, then simply return
		if (nullptr == g) {
			//fActiveGraphic = nullptr;
			return;
		}

		// bring it to the front
		moveToFront(g);
		g->mousePressed(e);
	}

	// Factory method to create a set
	static std::shared_ptr<TabViewSet> create(const float w, const float h)
	{
		auto tset = std::make_shared<TabViewSet>(w, h);
		return tset;
	}
};