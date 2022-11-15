#pragma once

#include "graphic.hpp"
#include "Surface.h"

class GWindow : public Graphic
{
protected:
	BLRect fClientArea;
	maths::vec2f fLastMouse;
	bool fIsMoveable;
	bool fIsMoving;
	Pixel fBackgroundColor;
	BLRect fTitleBar;
	Pixel fTitleBarColor;
	std::string fTitle;

	User32PixelMap fPixelMap;
	Surface fSurface;

public:

	GWindow(float x, float y, float w, float h)
		: Graphic(x, y, w, h)
		,fClientArea(0, 0, w, h)
		,fTitleBar(2, 2, w, 32)
		,fTitleBarColor(0x7f, 0x7f, 0x7f, 200)
		,fBackgroundColor(245, 246, 247)
		,fLastMouse{ 0, 0 }
		,fIsMoveable(true)
		,fIsMoving(false)
	{
		fPixelMap.init(w, h);
		fSurface.attachPixelArray(fPixelMap);
	}

	void setMoveable(const bool moveable)
	{
		fIsMoveable = moveable;
	}

	BLImage& getImage() { return fSurface.getImage(); }


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
		ctx.textFont("Consolas");
		ctx.text(fTitle.c_str(), fTitleBar.x + fTitleBar.w / 2, 16);
		ctx.flush();
	}

	void setBackgroundColor(const Pixel& c)
	{
		fBackgroundColor = c;
	}

	void drawBackground(IGraphics & ctx) override
	{
		ctx.push();

		ctx.clear();

		if (fBackgroundColor.value == 0) {
			ctx.clearRect(fClientArea.x, fClientArea.y, fClientArea.w, fClientArea.h);
		} else {
			// Fill in background
			ctx.noStroke();
			ctx.fill(fBackgroundColor);
			ctx.rect(fClientArea.x, fClientArea.y, fClientArea.w, fClientArea.h);
		}

		ctx.pop();
	}

	void drawForeground(IGraphics& ctx) override
	{
		drawTitleBar(ctx);
		
		// Draw a frame
		auto sz = maths::size(frame());
		ctx.strokeWeight(2.0f);
		ctx.stroke(0);
		ctx.noFill();
		ctx.rect(0, 0, sz.x, sz.y);
	}

	void compose(IGraphics& ctx)
	{
		fSurface.flush();

		ctx.image(fSurface.getImage(), (int)fFrame.min.x, (int)fFrame.min.y);
	}

	void draw(IGraphics & ctx) override
	{
		drawBackground(fSurface);
		drawChildren(fSurface);
		drawSelf(fSurface);
		drawForeground(fSurface);

		compose(ctx);
	}

	void setTitle(const char* title)
	{
		setTitle(std::string(title));
	}

	void setTitle(const std::string& title)
	{
		fTitle.clear();
		fTitle.append(title);
		setMoveable(true);
	}

	bool inTitleBar(float x, float y)
	{
		return ((x >= fTitleBar.x) && (y >= fTitleBar.y) &&
			(x - fTitleBar.x <= fTitleBar.w) &&
			(y - fTitleBar.y <= fTitleBar.h));
	}
	
	bool isMoving()
	{
		return fIsMoveable && fIsMoving;
	}

	void mouseEvent(const MouseEvent& e) override
	{
		//printf("GWindow.mouseEvent: %d (%d,%d)\n", e.activity, e.x, e.y);

		// First check to see if we're moving our window
		// around, from dragging in the titleBar area
		switch (e.activity)
		{
		case MOUSEPRESSED:
			if (inTitleBar(e.x, e.y) && fIsMoveable)
			{
				fIsMoving = true;
				fLastMouse = { e.x, e.y };

				return;
			}
			break;

		case MOUSERELEASED:
			if (isMoving()) {
				fIsMoving = false;

				return;
			}
			break;

		case MOUSEMOVED:
			if (isMoving()) {
				// move
				auto dx = (e.x - fLastMouse.x);
				auto dy = (e.y - fLastMouse.y);
				//printf("GWindow.mouseEvent(moved): %3.2f %3.2f\n", dx, dy);

				moveBy(dx, dy);

				fLastMouse = { e.x-dx, e.y-dy };
				return;
			}
			break;

		}

		// if we are here, we're not moving around
		// figure out if there was a graphic under the mouse
		// and if there is, forward the event to that graphic
		auto g = graphicAt(e.x, e.y);

		if (g != fActiveGraphic)
		{
			// tell the active graphic it's no longer
		}

		if (g != nullptr)
		{
			setActiveGraphic(g);

			// right here, need to adjust the event to account
			// for the frame of the underlying graphic
			MouseEvent newEvent(e);
			newEvent.x = (e.x - g->frameX());
			newEvent.y = (e.y - g->frameY());

			g->mouseEvent(newEvent);
		}

		// BUGBUG
		// If there was no underlying graphic
		// then give the window a chance to do something?
	}

	virtual void keyEvent(const KeyboardEvent& e) override
	{
		//printf("GWindow:keyEvent\n");

		if (fActiveGraphic != nullptr)
			fActiveGraphic->keyEvent(e);
	}
};