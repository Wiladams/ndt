#pragma once

#include "graphic.hpp"
#include "datachunk.h"
#include "svgparser.h"


namespace svg
{
	struct SVGGraphic : public GraphicGroup
	{
		SVGGraphic(maths::rectf& fr)
		{

		}

		bool initFromChunk(const DataChunk& srcChunk, SVGGraphic & destGraphic);
	};
}


namespace svg
{
	INLINE bool SVGGraphic::initFromChunk(const DataChunk& srcChunk, SVGGraphic& destGraphic)
	{
		SVGParser parser;
		svg::parseSVGDocument(srcChunk, p);
	}
	
		
	
}
