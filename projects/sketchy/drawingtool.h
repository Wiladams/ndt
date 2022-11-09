#pragma once

#include "Graphics.h"

class DrawingTool : public IDrawable
{
protected:
	BLPath fFigure;		// The little piece we're drawing

public:
	DrawingTool() {}

	virtual const BLPath& figure() const
	{
		return fFigure;
	}

	virtual void draw(IGraphics &ctx) {}

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
	StraightLineTool()
		:DrawingTool(),
		fStartDrag (),
		fEndDrag()
	{
	}

	void draw(IGraphics &ctx) override
	{
		if (fIsDragging) {
			ctx.stroke(Pixel(255, 0, 0));
			ctx.line(fStartDrag.x, fStartDrag.y, fEndDrag.x, fEndDrag.y);
		}
	}

	void mousePressed(const MouseEvent& e) override
	{
		//printf("lineTool - mousePressed: %d, %d\n", e.x, e.y);
		fStartDrag = { (double)e.x, (double)e.y };
		fEndDrag = { (double)e.x, (double)e.y };
		fIsDragging = true;
	}

	void mouseMoved(const MouseEvent& e) override
	{
		//printf("lineTool - mouseMoved: %d, %d\n", e.x, e.y);

		if (fIsDragging) {
			fEndDrag = BLPoint((double)e.x, e.y);
		}
	}

	void mouseReleased(const MouseEvent& e) override
	{
		//printf("lineTool - mouseReleased: %d, %d\n", e.x, e.y);
		fFigure.addLine(BLLine(fStartDrag.x, fStartDrag.y, fEndDrag.x, fEndDrag.y));

		fIsDragging = false;
	}
};

class RectangleTool : public DrawingTool
{
	bool fIsDragging = false;
	BLPoint fStartDrag{};
	BLPoint fEndDrag{};


public:
	RectangleTool()
		:DrawingTool()
		, fStartDrag{}
		, fEndDrag{}
	{
	}

	void draw(IGraphics & ctx) override
	{
		if (fIsDragging) 
		{
			ctx.stroke(Pixel(255, 0, 0));
			ctx.noFill();

			
			float x = maths::min((float)fStartDrag.x, (float)fEndDrag.x);
			float y = maths::min((float)fStartDrag.y, (float)fEndDrag.y);
			float w = maths::abs((float)fEndDrag.x - (float)fStartDrag.x);
			float h = maths::abs((float)fEndDrag.y - (float)fStartDrag.y);

			// The simplest thing to draw is an outline
			// of the rectangle, as we're dragging around
			ctx.rect(x, y, w, h);
		}
	}

	void mousePressed(const MouseEvent& e) override
	{
		//printf("lineTool - mousePressed: %d, %d\n", e.x, e.y);
		fFigure.clear();
		fFigure.moveTo(e.x, e.y);

		fStartDrag = { (double)e.x, (double)e.y };
		fEndDrag = { (double)e.x, (double)e.y };

		fIsDragging = true;
	}

	void mouseMoved(const MouseEvent& e) override
	{
		//printf("lineTool - mouseMoved: %d, %d\n", e.x, e.y);

		if (fIsDragging) {
			fEndDrag = BLPoint(e.x, e.y);
		}
	}

	void mouseReleased(const MouseEvent& e) override
	{
		//printf("lineTool - mouseReleased: %d, %d\n", e.x, e.y);

		fIsDragging = false;

		float x = maths::min((float)fStartDrag.x, (float)fEndDrag.x);
		float y = maths::min((float)fStartDrag.y, (float)fEndDrag.y);
		float w = maths::abs((float)fEndDrag.x - (float)fStartDrag.x);
		float h = maths::abs((float)fEndDrag.y - (float)fStartDrag.y);
		
		fFigure.addRect(BLRect(x, y, w, h));
	}
};


class PencilTool : public DrawingTool
{
	bool fIsDragging = false;
	BLPoint fStartDrag{};
	BLPoint fEndDrag{};

public:
	PencilTool()
		:DrawingTool()
	{
	}

	void draw(IGraphics & ctx)
	{
		if (fIsDragging) 
		{
			ctx.stroke(Pixel(255, 0, 0));
			ctx.noFill();
			ctx.path(fFigure);
		}
	}

	void mousePressed(const MouseEvent& e)
	{
		//printf("PencilTool - mousePressed: %d, %d\n", e.x, e.y);

		fFigure.clear();
		fFigure.moveTo((double)e.x, (double)e.y);

		fIsDragging = true;
	}

	void mouseMoved(const MouseEvent& e)
	{
		//printf("PencilTool - mouseMoved: %d, %d\n", e.x, e.y);

		if (fIsDragging) {
			fFigure.lineTo((double)e.x, (double)e.y);
		}
	}

	void mouseReleased(const MouseEvent& e)
	{
		//printf("PencilTool - mouseReleased: %d, %d\n", e.x, e.y);

		fIsDragging = false;
	}
};
