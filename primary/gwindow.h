#pragma once

#include "gview.h"

class GWindow : public GView
{
protected:
	BLRect fClientArea;
	BLPoint fLastMouse;
	bool fIsMoving;
	Pixel fBackgroundColor;
	BLRect fTitleBar;
	Pixel fTitleBarColor;
	std::string fTitle;

public:
	GWindow(int x, int y, int w, int h)
		: GView(BLRect(x,y,w,h)),
		fClientArea(0,0,w,h),
		fTitleBar(2, 2, w, 32),
		fTitleBarColor(225, 220, 220, 127),
		fBackgroundColor(245, 246, 247),
		fLastMouse(0, 0),
		fIsMoving(false)
	{

	}

	void setBackgroundColor(const Pixel& c)
	{
		fBackgroundColor = c;
	}

	void drawBackground(IGraphics* ctx)
	{
		//std::cout << "GWindow::drawBackground" << std::endl;

		ctx->push();



		// Fill in background
		ctx->noStroke();
		ctx->fill(fBackgroundColor);
		ctx->rect(fClientArea.x, fClientArea.y, fClientArea.w, fClientArea.h);

		// Draw a frame
		auto frame = getFrame();

		ctx->strokeWeight(2.0);
		ctx->stroke(0);
		ctx->noFill();
		ctx->rect(0, 0, frame.w, frame.h);

		ctx->pop();
	}

	void setTitle(const std::string& title)
	{
		fTitle = title;
	}

	bool inTitleBar(int x, int y)
	{
		return ((x >= fTitleBar.x) && (y >= fTitleBar.y) &&
			(x - fTitleBar.x <= fTitleBar.w) &&
			(y - fTitleBar.y <= fTitleBar.h));
	}
	
	void drawTitleBar(IGraphics* ctx)
	{
		// If there's no title, don't draw anything
		if (fTitle.empty())
			return;

		// A little rectangle to back title
		ctx->noStroke();
		ctx->fill(fTitleBarColor);
		ctx->rect(fTitleBar.x, fTitleBar.y, fTitleBar.w, fTitleBar.h);

		// Draw title
		ctx->fill(0);
		ctx->textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx->textSize(12);
		ctx->text(fTitle.c_str(), fTitleBar.x+fTitleBar.w/2, 16);
		ctx->flush();
	}

	void drawForeground(IGraphics* ctx)
	{
		drawTitleBar(ctx);
	}


	virtual void mousePressed(const MouseEvent& e)
	{
	auto x = (int)((double)e.x - fFrame.x);
	auto y = (int)((double)e.y - fFrame.y);
	if (inTitleBar(x, y)) {
		fIsMoving = true;
		fLastMouse = { (double)e.x, (double)e.y };
		}
	}

virtual void mouseMoved(const MouseEvent& e)
{
	//std::cout << "FileHistoWindow.mouseDragged()" << std::endl;
	// adjust coordinates accounting for our frame
	auto x = e.x - fFrame.x;
	auto y = e.y - fFrame.y;

	if (fIsMoving)
	{
		// move
		auto dx = (e.x - fLastMouse.x);
		auto dy = (e.y - fLastMouse.y);

		moveBy(dx, dy);

		fLastMouse = { (double)e.x, (double)e.y };
	}
}

virtual void mouseReleased(const MouseEvent& e)
{
	auto x = e.x - fFrame.x;
	auto y = e.y - fFrame.y;

	fIsMoving = false;
}

};