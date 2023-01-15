#pragma once

#include "gwindow.h"
#include "svgdocument.h"

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
