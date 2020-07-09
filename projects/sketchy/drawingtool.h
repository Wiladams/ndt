#pragma once

#include "p5.hpp"

class DrawingTool
{
protected:
	BLPath &fFigure;

public:
	DrawingTool(BLPath &fig)
		:fFigure(fig)
	{

	}

	virtual void draw(IGraphics* ctx) {}

	virtual void mousePressed(const MouseEvent &e){}
	virtual void mouseReleased(const MouseEvent& e) {}
	virtual void mouseMoved(const MouseEvent& e) {}
	virtual void mouseClicked(const MouseEvent& e) {}
};

class StraightLineTool : public DrawingTool
{
	bool fIsDragging = false;
	BLPoint fStartDrag{};
	BLPoint fEndDrag{};


public:
	StraightLineTool(BLPath &fig)
		:DrawingTool(fig),
		fStartDrag (),
		fEndDrag()
	{
	}

	void draw(IGraphics *ctx)
	{
		if (fIsDragging) {
			ctx->stroke(Pixel(255, 0, 0));
			ctx->line(fStartDrag.x, fStartDrag.y, fEndDrag.x, fEndDrag.y);
		}
	}

	void mousePressed(const MouseEvent& e)
	{
		printf("lineTool - mousePressed: %d, %d\n", e.x, e.y);

		fStartDrag = { (double)mouseX, (double)mouseY };
		fIsDragging = true;
	}

	void mouseMoved(const MouseEvent& e)
	{
		printf("lineTool - mouseMoved: %d, %d\n", e.x, e.y);

		if (fIsDragging) {
			fEndDrag = BLPoint((double)mouseX, mouseY);
		}
	}

	void mouseReleased(const MouseEvent& e)
	{
		printf("lineTool - mouseReleased: %d, %d\n", e.x, e.y);

		fIsDragging = false;

		fFigure.addLine(BLLine(fStartDrag.x, fStartDrag.y, fEndDrag.x, fEndDrag.y));
	}
};

class RectangleTool : public DrawingTool
{
	bool fIsDragging = false;
	BLPoint fStartDrag{};
	BLPoint fEndDrag{};


public:
	RectangleTool(BLPath &fig)
		:DrawingTool(fig),
		fStartDrag(),
		fEndDrag()
	{
	}

	void draw(IGraphics* ctx)
	{
		if (fIsDragging) {
			ctx->push();
			ctx->stroke(Pixel(255, 0, 0));
			ctx->noFill();
			//ctx->fill(0, 0, 0, 1);
			//ctx->blendMode(BL_COMP_OP_SRC_COPY);
			double x = maths::Min(fStartDrag.x, fEndDrag.x);
			double y = maths::Min(fStartDrag.y, fEndDrag.y);
			double w = abs(fEndDrag.x - fStartDrag.x);
			double h = abs(fEndDrag.y - fStartDrag.y);

			ctx->rect(x, y, w, h);
			ctx->pop();
		}
	}

	void mousePressed(const MouseEvent& e)
	{
		printf("lineTool - mousePressed: %d, %d\n", e.x, e.y);

		fStartDrag = { (double)mouseX, (double)mouseY };
		fEndDrag = { (double)mouseX, (double)mouseY };

		fIsDragging = true;
	}

	void mouseMoved(const MouseEvent& e)
	{
		printf("lineTool - mouseMoved: %d, %d\n", e.x, e.y);

		if (fIsDragging) {
			fEndDrag = BLPoint((double)mouseX, mouseY);
		}
	}

	void mouseReleased(const MouseEvent& e)
	{
		printf("lineTool - mouseReleased: %d, %d\n", e.x, e.y);

		fIsDragging = false;

		// should do whatever we're to do 
		// with the canvas
		// typically adding a piece of path
		//mainPath.addLine(BLLine(fStartDrag.x, fStartDrag.y, fEndDrag.x, fEndDrag.y));
	}
};


class PencilTool : public DrawingTool
{
	bool fIsDragging = false;
	GPath fPath;

public:
	PencilTool(BLPath &fig)
		:DrawingTool(fig),
		fPath()
	{
	}

	void draw(IGraphics* ctx)
	{
		if (fIsDragging) {
			ctx->push();
			ctx->stroke(Pixel(255, 0, 0));
			ctx->noFill();
			ctx->path(fPath);
			ctx->pop();
		}
	}

	void mousePressed(const MouseEvent& e)
	{
		//printf("PencilTool - mousePressed: %d, %d\n", e.x, e.y);

		fPath.clear();
		fPath.moveTo(e.x, e.y);

		fIsDragging = true;
	}

	void mouseMoved(const MouseEvent& e)
	{
		//printf("PencilTool - mouseMoved: %d, %d\n", e.x, e.y);

		if (fIsDragging) {
			fPath.lineTo(e.x, e.y);
		}
	}

	void mouseReleased(const MouseEvent& e)
	{
		//printf("PencilTool - mouseReleased: %d, %d\n", e.x, e.y);

		fIsDragging = false;
		fFigure.addPath(fPath);
	}
};
