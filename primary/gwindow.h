#pragma once

#include "graphic.hpp"
#include "Surface.h"

class GWindow : public Graphic
{
protected:
	BLRect fClientArea;
	BLPoint fLastMouse;
	bool fIsMoving;
	Pixel fBackgroundColor;
	BLRect fTitleBar;
	Pixel fTitleBarColor;
	std::string fTitle;

	std::shared_ptr<Surface> fSurface;	// backing store for drawing

public:

	GWindow(double x, double y, double w, double h)
		: Graphic(x, y, w, h),
		fClientArea(0, 0, w, h),
		fTitleBar(2, 2, w, 32),
		fTitleBarColor(0x7f, 0x7f, 0x7f, 200),
		fBackgroundColor(245, 246, 247),
		fLastMouse(0, 0),
		fIsMoving(false)
	{
		fSurface = std::make_shared<Surface>(w, h);
	}

	BLImage& getImage() { return fSurface->getImage(); }


	void drawTitleBar(IGraphics & ctx)
	{
		// If there's no title, don't draw anything
		if (fTitle.empty())
			return;

		// A little rectangle to back title
		ctx.noStroke();
		ctx.fill(fTitleBarColor);
		ctx.rect(fTitleBar.x, fTitleBar.y, fTitleBar.w, fTitleBar.h);

		// Draw title
		ctx.fill(0);
		ctx.textAlign(ALIGNMENT::CENTER, ALIGNMENT::BASELINE);
		ctx.textSize(12);
		ctx.text(fTitle.c_str(), fTitleBar.x + fTitleBar.w / 2, 16);
		ctx.flush();
	}

	void setBackgroundColor(const Pixel& c)
	{
		fBackgroundColor = c;
	}

	void drawBackground(IGraphics & ctx)
	{
		ctx.push();

		ctx.clear();

		if (fBackgroundColor.value == 0) {

		} else {
			// Fill in background
			ctx.noStroke();
			ctx.fill(fBackgroundColor);
			ctx.rect(fClientArea.x, fClientArea.y, fClientArea.w, fClientArea.h);
		}

		ctx.pop();
	}

	void drawForeground(IGraphics& ctx)
	{
		drawTitleBar(ctx);
		
		// Draw a frame
		auto frame = getFrame();

		ctx.strokeWeight(2.0f);
		ctx.stroke(0);
		ctx.noFill();
		ctx.rect(0, 0, frame.w, frame.h);

	}

	void draw(IGraphics & ctx)
	{
		//fSurface->push();
		//ctx.clip(fFrame.x, fFrame.y, fFrame.w, fFrame.h);


		// BUGBUG - maybe perform arbitrary transform?
		//auto pt = fTransform.mapPoint(fFrame.x, fFrame.y);
		//ctx->translate(pt.x, pt.y);

		drawBackground(*fSurface);
		drawChildren(*fSurface);
		drawSelf(*fSurface);
		drawForeground(*fSurface);

		//ctx->noClip();
		//ctx.pop();

		compose(ctx);
	}

	void compose(IGraphics & ctx)
	{
		ctx.image(fSurface->getImage(), (int)fFrame.x, (int)fFrame.y);
	}

	void setTitle(const char* title)
	{
		fTitle.clear();
		fTitle.append(title);
	}

	void setTitle(const std::string& title)
	{
		fTitle.clear();
		fTitle.append(title);
	}

	bool inTitleBar(int x, int y)
	{
		return ((x >= fTitleBar.x) && (y >= fTitleBar.y) &&
			(x - fTitleBar.x <= fTitleBar.w) &&
			(y - fTitleBar.y <= fTitleBar.h));
	}
	




	virtual void mouseEvent(const MouseEvent& e)
	{
		//printf("GWindow.mouseEvent: %d (%d,%d)\n", e.activity, e.x, e.y);


		switch (e.activity)
		{
		case MOUSEPRESSED:
			if (inTitleBar(e.x, e.y))
			{
				fIsMoving = true;
				fLastMouse = { (double)e.x, (double)e.y };

				return;
			}
			break;

		case MOUSERELEASED:
			if (fIsMoving) {
				fIsMoving = false;
			}
			break;

		case MOUSEMOVED:
			if (fIsMoving) {
				// move
				auto dx = (e.x - fLastMouse.x);
				auto dy = (e.y - fLastMouse.y);
				//printf("GWindow.mouseEvent(moved): %3.2f %3.2f\n", dx, dy);

				moveBy(dx, dy);

				fLastMouse = { (double)e.x-dx, (double)e.y-dy };
				return;
			}
			break;

		case MOUSEWHEEL:
			//printf("GWindow.MouseEvent: WHEEL\n");
			break;
		}

		// if we are here, the window itself did not 
		// handle the event, so allow sub-graphics to deal with it
		auto win = graphicAt(e.x, e.y);

		if (win != nullptr) {
			win->mouseEvent(e);
		}

	}


};