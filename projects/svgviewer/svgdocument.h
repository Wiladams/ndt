#pragma once


#include "graphic.hpp"


using namespace tinyxml2;

//  SVGDocument
//  The SVGDocument represents the graphic that we can
//  draw on the canvas.  It is the result of the parsing.
//
//  We retain the original content, in the fDoc object, but 
//  we create a number of graphic elements that will be used
//  when it comes time to render.
//

struct SVGDocument
{
	XMLDocument fDoc{};
	bool fIsValid = false;

	SVGDocument()
	{
		fIsValid = false;
	}

	bool reset(const char* filename)
	{
		fIsValid = false;

		auto err = fDoc.LoadFile(filename);

		if (err != 0)
		{
			return false;
		}

		fIsValid = true;

		return fIsValid;
	}

	constexpr bool isValid() const { return fIsValid; }
};





};