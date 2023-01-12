#pragma once

#include "maths.hpp"
#include "gwindow.h"
#include "svgdocument.h"

struct SVGWindow : public GWindow
{
	maths::vec2f fLastDragLoc{};


	SVGWindow(std::shared_ptr<svg::SVGDocument> doc)
		: GWindow(0,0,doc->width()<100?100: (float)doc->width(), (float)doc->height()<100?100: (float)doc->height())
	{
		setDocument(doc);
	}

	SVGWindow(float x, float y, float w, float h) : GWindow(x, y, w, h)
	{
		//setFrame(frame);
	}

	void setDocument(std::shared_ptr<svg::SVGDocument> doc)
	{
		// calculate the bounds of the document
		// so we can adjust the scaling for drawing
		addDrawable(doc);
	}

	virtual void keyEvent(const KeyboardEvent& e) override
	{
		switch (e.activity)
		{
		case KEYPRESSED:
			if (e.keyCode == VK_RIGHT)
			{
				translateBoundsBy(10, 0);

			}
			else if (e.keyCode == VK_LEFT)
			{
				translateBoundsBy(-10, 0);
			}
			break;
		}

		if (fActiveGraphic != nullptr)
			fActiveGraphic->keyEvent(e);
	}

	void mouseEvent(const MouseEvent& e) override
	{
		//printf("MOUSE MOVING: %d (%3.0f,%3.0f)\n", e.activity, e.x, e.y);


		switch (e.activity) {
		case MOUSEWHEEL:
		{
			if (e.delta < 0)
				scaleBoundsBy(0.9f, 0.9f);
			else
				scaleBoundsBy(1.1f, 1.1f);

			needsRedraw(true);
		}
		break;

		case MOUSEPRESSED:
		{
			fLastDragLoc = { e.x, e.y };
		}
		break;

		case MOUSEMOVED:
		{
			if (!isMoving() && fIsDragging)
			{

				float diffx = e.x - fLastDragLoc.x;
				float diffy = e.y - fLastDragLoc.y;
				//printf("MOUSE DRAGGING: %d (%3.0f,%3.0f)\n", e.activity, diffx, diffy);

				translateBoundsBy(diffx, diffy);
				fLastDragLoc = { e.x, e.y };

				needsRedraw(true);
			}
		}
		break;
		}

		GWindow::mouseEvent(e);

	}

};
