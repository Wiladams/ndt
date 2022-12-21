#pragma once

#include "graphic.hpp"
#include "datachunk.h"
#include "svgparser.h"


namespace svg
{

	struct SVGGraphic : public GraphicGroup
	{
		
		SVGGraphic(const maths::rectf& fr)
			:GraphicGroup(fr)
		{
			setBounds(frame());
		}


		bool initFromChunk(const DataChunk& srcChunk)
		{
			SVGParser p;
			svg::parseSVGDocument(srcChunk, p);

			// Now we have a parsed svg
			// use the svgp.shapes to create a graphic
			maths::rectf bnds{ 0,0,p.imageWidth, p.imageHeight };
			//setBounds(bnds);

			// Go through the shapes, turning each one into an SVGGraphicShape
			for (auto& shape : p.fShapes)
			{
				addGraphic(shape);
			}

			return true;
		}

		void draw(IGraphics& ctx) override
		{
			for (auto& g : fChildren)
			{
				g->draw(ctx);
			}

		}
	};
}

