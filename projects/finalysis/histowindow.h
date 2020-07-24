#pragma once

#include "gwindow.h"
#include "filehisto.h"

class FileHistoWindow : public GWindow
{
	std::string fFilename;
	BLRect fTitleBar;
	Pixel fTitleBarColor;
	BLPoint fLastMouse;
	bool fIsMoving;

public:
	FileHistoWindow(std::string filename, int x, int y, int w = 256, int h = 256)
		: GWindow(x, y, w, h),
		fFilename(filename),
		fTitleBar(2, 2, 254, 32),
		fTitleBarColor(225, 220, 220, 127),
		fLastMouse(0, 0),
		fIsMoving(false)
	{
		setPage(FileHistogram::fromFile(filename));
	}

	void drawBackground(IGraphics* ctx)
	{
		ctx->push();

		// Fill in background
		ctx->fill(250, 250, 220);
		ctx->rect(fClientArea.x, fClientArea.y, fClientArea.w, fClientArea.h);

		// draw a little frame
		ctx->stroke(0);
		ctx->noFill();
		ctx->rect(0, 0, 256, 256);

		ctx->pop();
	}

	void drawTitleBar(IGraphics* ctx)
	{
		// A little rectangle to back filename
		ctx->noStroke();
		ctx->fill(fTitleBarColor);
		ctx->rect(fTitleBar.x, fTitleBar.y, fTitleBar.w, fTitleBar.h);

		// Draw filename
		ctx->fill(0);
		ctx->text(fFilename.c_str(), 4, 16);
		ctx->flush();
	}

	void drawForeground(IGraphics* ctx)
	{
		drawTitleBar(ctx);
	}

	bool inTitleBar(int x, int y)
	{
		return ((x >= fTitleBar.x) && (y >= fTitleBar.y) &&
			(x - fTitleBar.x <= fTitleBar.w) &&
			(y - fTitleBar.y <= fTitleBar.h));
	}

	virtual void mousePressed(const MouseEvent& e)
	{
		auto x = (double)e.x - fFrame.x;
		auto y = (double)e.y - fFrame.y;
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
			int dx = e.x - fLastMouse.x;
			int dy = e.y - fLastMouse.y;

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
