#pragma once

#include "graphic.hpp"
#include "Surface.h"

//
// GWindow
// Things that differentiate a GWindow from 
// an ordinary Graphic
// Has-A Backing Buffer
// Has-A Title Bar
// Can - Move based on mouse commands in title bar
// 
// GWindow is kind of heavy weight, and assumes you're going to do
// a lot of drawing, and you might want to retain it, rather than
// having to draw from scratch each time.  So, you draw into the 
// backing store, and that get's blt'd into the context at the right
// time.
//

class GWindow : public GraphicGroup
{
protected:
	maths::bbox2f fClientArea;

	// If window has a title bar
	BLRect fTitleBar;
	Pixel fTitleBarColor;
	std::string fTitle;

	// Window movement
	maths::vec2f fLastMouse;
	bool fIsMoveable{ true };
	bool fIsMoving{ false };
	bool fIsDragging{ false };
	
	// For drawing and composing
	Pixel fBackgroundColor;
	User32PixelMap fPixelMap;
	Surface fSurface;
	bool fNeedsRedraw{ true };
	bool fSmartCache{ false };

public:

	GWindow(float x, float y, float w, float h)
		: GraphicGroup(x, y, w, h)
		, fClientArea({ {0, 0}, {w, h} })
		,fTitleBar(2, 2, w, 32)
		,fTitleBarColor(0x7f, 0x7f, 0x7f, 200)
		,fBackgroundColor(245, 246, 247)
		,fLastMouse{ 0, 0 }
		,fIsMoveable(true)
		,fIsMoving(false)
	{
		fPixelMap.init(w, h);
		fSurface.attachPixelArray(fPixelMap);

		// Set default drawing attributes
		fSurface.fill(Pixel(0, 0, 0));
		fSurface.stroke(Pixel(0, 0, 0));
		fSurface.strokeWeight(1.0);
	}

	const bool smartCache() const { return fSmartCache; }
	void smartCache(bool b) { fSmartCache = b; }
	
	const bool needsRedraw() const { return fNeedsRedraw; }
	void needsRedraw(bool needsIt) { fNeedsRedraw = needsIt; }

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

		ctx.push();
		
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

		ctx.pop();
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
			auto sz = maths::size(fClientArea);
			ctx.clearRect(fClientArea.min.x, fClientArea.min.y, sz.x, sz.y);
		} else {
			// Fill in background
			auto sz = maths::size(fClientArea);
			ctx.noStroke();
			ctx.fill(fBackgroundColor);
			ctx.rect(fClientArea.min.x, fClientArea.min.y, sz.x, sz.y);
		}
		
		ctx.pop();
	}

	void drawForeground(IGraphics& ctx) override
	{
		drawTitleBar(ctx);
		
		ctx.push();
		
		// Draw a frame
		auto sz = maths::size(frame());
		ctx.strokeWeight(2.0f);
		ctx.stroke(0);
		ctx.noFill();
		ctx.rect(0, 0, sz.x, sz.y);
		ctx.pop();
	}

	void compose(IGraphics& ctx)
	{
		fSurface.flush();

		ctx.image(fSurface.getImage(), (int)frameX(), (int)frameY());
	}

	void draw(IGraphics & ctx) override
	{

		if (!smartCache() || needsRedraw())
		{
			drawBackground(fSurface);

			fSurface.push();

			// Apply user specified transform
			fSurface.scale(fScale.x, fScale.y);
			fSurface.translate(fTranslation.x, fTranslation.y);


			drawSelf(fSurface);

			fSurface.pop();

			drawForeground(fSurface);

			needsRedraw(false);
		}

		compose(ctx);
	}


	void setTitle(const std::string& title)
	{
		fTitle.clear();
		fTitle.append(title);

		// adjust the client area accordingly
		//fClientArea.min.y = fTitleBar.x + fTitleBar.h;
		//fClientArea.min.h -= fClientArea.y;

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
		//printf("GWindow.mouseEvent: %d (%3.0f,%3.0f)\n", e.activity, e.x, e.y);

		// Mouse events are given in the coordinate space of the 
// parent frame, so we want to convert to our local coordinate
// space before doing anything else.
// First subtract off the frame origin
		MouseEvent lev(e);
		lev.x = e.x - frameX();
		lev.y = e.y - frameY();

		// First check to see if we're moving our window
		// around, from dragging in the titleBar area
		switch (lev.activity)
		{
		case MOUSEPRESSED:
			fIsDragging = true;
			fLastMouse = { lev.x, lev.y };

			if (inTitleBar(lev.x, lev.y) && fIsMoveable)
			{
				fIsMoving = true;


				return ;
			}
			break;

		case MOUSERELEASED:
			fIsDragging = false;
			if (isMoving()) {
				fIsMoving = false;

				return ;
			}
			break;

		case MOUSEMOVED: {
			auto dx = (lev.x - fLastMouse.x);
			auto dy = (lev.y - fLastMouse.y);

			fLastMouse = { lev.x-dx, lev.y-dy };
			
			if (isMoving()) {
				//printf("GWindow.mouseEvent(moved): %3.2f %3.2f\n", dx, dy);
				moveBy(dx, dy);

				return;
			}
			}
			break;

		}

		// if we are here, we're not moving around
		// figure out if there was a graphic under the mouse
		// and if there is, forward the event to that graphic
		auto g = graphicAt(lev.x, lev.y);

		if (g != fActiveGraphic)
		{
			// tell the active graphic it's no longer
		}

		if (g != nullptr)
		{
			fActiveGraphic = g;

			g->mouseEvent(lev);
		}

	}

	virtual void keyEvent(const KeyboardEvent& e) override
	{
		//printf("GWindow:keyEvent\n");

		if (fActiveGraphic != nullptr)
			fActiveGraphic->keyEvent(e);
	}
};