#pragma once

#include "graphic.hpp"
#include "blend2d.h"
#include <string>

class GPath {
	BLPath fPath;
	BLPoint fLastPoint;

	BLPoint getLastVertex()
	{
		BLPoint vtxOut;
		BLResult bResult = fPath.getLastVertex(&vtxOut);
		return vtxOut;
	}

public:
	operator BLPath &() { return fPath; }

	void close()
	{
		fPath.close();
	}

	void cubicTo(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		fPath.cubicTo(x1, y1, x2, y2, x3, y3);
		fLastPoint = { x3,y3 };
	}
	
	void cubicBy(double x1, double y1, double x2, double y2, double x3, double y3)
	{
		cubicTo(fLastPoint.x + x1, fLastPoint.y + y1,
			fLastPoint.x + x1 + x2, fLastPoint.y + y1 + y2,
			fLastPoint.x + x1 + x2 + x3, fLastPoint.y + y1 + y2 + y3);
	}

	void lineTo(double x, double y)
	{
		fPath.lineTo(x, y);
		fLastPoint = { x,y };
	}

	void lineBy(double x, double y)
	{
		lineTo(fLastPoint.x + x, fLastPoint.y + y);
	}

	void moveTo(double x, double y)
	{
		fPath.moveTo(x, y);
		fLastPoint = { x,y };
	}

	void moveBy(double x, double y)
	{
		moveTo(fLastPoint.x + x, fLastPoint.y + y);
	}
};

class TabbedView : public Graphic {

	BLRect fFrame;		// frame for the whole graphic
	
	// Content Area details
	GPath fWholePath;
	BLRgba32 fBackgroundColor = { 255, 234, 167 };
	BLRect fContentArea;

	// Tab details
	BLRoundRect fTabParam;
	GPath fTabPath;
	BLRgba32 fTabColor = { 255,255,255 };
	BLRect fTabContentArea;
	std::string fTabTitle;

public:
	TabbedView(const BLRect& frame, const BLRoundRect &tParam, const std::string& title, const Pixel& tColor = {255,255,255})
		: fFrame(frame),
		fTabParam(tParam),
		fTabColor(tColor),
		fTabTitle(title)
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
		
	}

	void setBackgroundColor(const Pixel& c) { fBackgroundColor = c; }
	void setTabColor(const Pixel& c) { fTabColor = c; }

	void drawBackground(IGraphics* ctx)
	{
		ctx->clear();

		ctx->stroke(0);
		ctx->blendMode(BL_COMP_OP_SRC_OVER);
		ctx->fill(fBackgroundColor);
		ctx->path(fWholePath);

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
		ctx->noStroke();
		ctx->fill(0);
		ctx->textSize(fTabParam.h * .8);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx->text(fTabTitle.c_str(), titleX, titleY);
	}
};
