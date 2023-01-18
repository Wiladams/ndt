#pragma once

#include "gwindow.h"
#include "svgdocument.h"
#include "bufferedview.h"



struct SVGIcon : public BufferedView, public Topic<std::shared_ptr<svg::SVGDocument>>
{
	std::shared_ptr<svg::SVGDocument> fDocument{};
	bool fScaleToFit = false;


	SVGIcon(float x, float y, float w, float h, std::shared_ptr<svg::SVGDocument> doc)
		: BufferedView(x, y, w, h)
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

		// scale to fit
		if (scaleToFit)
		{
			float scalex = frameWidth() / doc->width();
			float scaley = frameHeight() / doc->height();
			float scale = scalex < scaley ? scalex : scaley;

			scaleBoundsTo(scale, scale);
		}

		// use the document's viewbox as the default viewport
		fTranslation.x = -doc->x();
		fTranslation.y = -doc->y();
		
		// calculate the bounds of the document
		// so we can adjust the scaling for drawing
		// clear all drawables
		clearDrawables();

		// add the document as a drawable
		addDrawable(doc);
	}

	void drawBackground(IGraphics& ctx)
	{
		ctx.background(Pixel(255, 255, 255, 255));
	}
	
	void mouseEvent(const MouseEvent& e) override
	{
		switch (e.activity) {

		case MOUSERELEASED:
			notify(document());
			break;

		}

	}

};
