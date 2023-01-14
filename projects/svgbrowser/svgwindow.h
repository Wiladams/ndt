#pragma once

#include "maths.hpp"
#include "gwindow.h"
#include "svgdocument.h"

#include <memory>

struct SVGIcon : public GWindow, public Topic<std::shared_ptr<svg::SVGDocument>>
{
	std::shared_ptr<svg::SVGDocument> fDocument{};
	bool fScaleToFit = false;


	SVGIcon(float x, float y, float w, float h, std::shared_ptr<svg::SVGDocument> doc) 
		: GWindow(x, y, w, h)
	{
		document(doc, true);
		smartCache(true);
	}

	std::shared_ptr<svg::SVGDocument> document() const { return fDocument; }
	void document(std::shared_ptr<svg::SVGDocument> doc, bool scaleToFit = true)
	{
		fDocument = doc;

		if (nullptr == doc)
			return;

		if (scaleToFit)
		{
			float scalex = frameWidth() / doc->width();
			float scaley = frameHeight() / doc->height();
			float scale = scalex < scaley ? scalex : scaley;

			scaleBoundsTo(scale, scale);
		}

		// calculate the bounds of the document
		// so we can adjust the scaling for drawing
		// clear all drawables
		clearDrawables();

		// add the document as a drawable
		addDrawable(doc);
	}


	void mouseEvent(const MouseEvent& e) override
	{
		switch (e.activity) {

		case MOUSERELEASED:
			notify(document());
			break;

		}

		//GWindow::mouseEvent(e);
	}

};

struct SVGWindow : public GWindow
{
	maths::vec2f fLastDragLoc{};
	std::shared_ptr<svg::SVGDocument> fDocument{};
	bool fScaleToFit = false;

	SVGWindow(std::shared_ptr<svg::SVGDocument> doc)
		: GWindow(0,0,doc->width()<100?100: (float)doc->width(), (float)doc->height()<100?100: (float)doc->height())
	{
		document(doc, false);
		smartCache(true);
	}

	SVGWindow(float x, float y, float w, float h, std::shared_ptr<svg::SVGDocument> doc) : GWindow(x, y, w, h)
	{	
		document(doc, true);
		smartCache(true);
	}

	std::shared_ptr<svg::SVGDocument> document() const { return fDocument; }
	void document(std::shared_ptr<svg::SVGDocument> doc, bool scaleToFit=true)
	{
		fDocument = doc;
		
		if (nullptr == doc)
			return;
		
		resetBoundsTransform();
		
		if (scaleToFit)
		{
			float scalex = frameWidth() / doc->width();
			float scaley = frameHeight() / doc->height();
			float scale = scalex < scaley ? scalex : scaley;

			scaleBoundsTo(scale, scale);
		}

		
		// calculate the bounds of the document
		// so we can adjust the scaling for drawing
		// clear all drawables
		clearDrawables();
		
		// add the document as a drawable
		addDrawable(doc);
		needsRedraw(true);
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
