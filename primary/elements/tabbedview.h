#pragma once

#include "blend2d.h"
#include "graphic.hpp"
#include "gpath.h"

#include <string>


class TabbedView : public GraphicGroup
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
		: GraphicGroup(f.x,f.y,f.w,f.h),
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

	void mouseEvent(const MouseEvent& e) override
	{
		printf("TabbedView::mouseEvent [%d] %3.0f, %3.0f\n", e.activity, e.x, e.y);
	}
};

class TabViewSet : public GraphicGroup 
{
	std::shared_ptr<GraphicElement> fSelectedGraphic{ nullptr };
	bool fIsDragging{ false };

public:
	TabViewSet(const float w, const float h)
		:GraphicGroup(0, 0, w, h)
	{}

	void mouseEvent(const MouseEvent& e) override
	{
		printf("TabViewSet::mouseEvent - [%d] %3.0f,%3.0f\n", e.activity, e.x, e.y);

		MouseEvent lev(e);
		lev.x = e.x - frameX();
		lev.y = e.y - frameY();

		auto hovered = graphicAt(lev.x, lev.y);

		switch (e.activity)
		{
		case MOUSEPRESSED:
			fIsDragging = true;
			fSelectedGraphic = hovered;
			if (fSelectedGraphic != nullptr)
			{
				moveToFront(fSelectedGraphic);
				fSelectedGraphic->mouseEvent(lev);
			}
			break;

		case MOUSERELEASED:
			fIsDragging = false;
			if (fSelectedGraphic != nullptr)
			{
				fSelectedGraphic->mouseEvent(lev);
			}
			break;

		case MOUSEMOVED:
			// If dragging, keep sending to the original 
			// graphic, even if we're beyond its bounds
			if (fIsDragging)
			{
				if (fSelectedGraphic != nullptr)
					fSelectedGraphic->mouseEvent(lev);
			}
			else {
				if (hovered)
					hovered->mouseEvent(lev);
			}
			break;

		default:
			if (fSelectedGraphic != nullptr)
				fSelectedGraphic->mouseEvent(lev);
			break;
		}

	}


	// Factory method to create a set
	static std::shared_ptr<TabViewSet> create(const float w, const float h)
	{
		auto tset = std::make_shared<TabViewSet>(w, h);
		return tset;
	}
};