#pragma once

#include "graphics.h"

#include "tinyxml/tinyxml2.h"


using namespace tinyxml2;

//  SVGDocument
//  The purpose of the SVGDocument is to parse the thing
//  and turn it into pieces we can use in rendering.
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

struct Element_Svg : public XMLVisitor
{

};

struct SvgVisitor : public tinyxml2::XMLVisitor
{
	IGraphics & fCtx;

	SvgVisitor(IGraphics& ctx)
		: fCtx(ctx)
	{

	}

	/// Visit an element.
	bool VisitEnter(const XMLDocument& doc) override
	{
		printf("XMLDocument ENTER: %s\n", doc.Value());
		
		return true;
	}

	/// Visit a document.
	virtual bool VisitExit(const XMLDocument& doc) 
	{
		printf("XMLDocument EXIT");
		return true;
	}

	/// Visit an element.
	virtual bool VisitEnter(const XMLElement& elem, const XMLAttribute* attr) override
	{
		
		printf("XMLElement ENTER: <%s>\n", elem.Value());

		if (std::string("rect") == elem.Value())
		{
			//printf("XMLElement ENTER: <%s>\n", elem.Value());

			const XMLAttribute * cAttr = attr;
			float x;
			float y;
			float w;
			float h;

			while (cAttr != nullptr)
			{
				const char* name = cAttr->Name();
				const char* value = cAttr->Value();
				char* endPtr;

				if (name == std::string("width"))
					w = strtof(value, &endPtr);
				if (name == std::string("height"))
					h = strtof(value, &endPtr);
				if (name == std::string("x"))
					x = strtof(value, &endPtr);
				if (name == std::string("y"))
					y = strtof(value, &endPtr);

				//printf("%s: %s\n", name, value);

				cAttr = cAttr->Next();
			}
			printf("rect(%3.2f,%3.2f, %3.2f,%3.2f);\n", x,y,w,h);
			fCtx.stroke(225);
			fCtx.fill(255, 0, 0);
			fCtx.rect(x, y, w, h);
		}

		return true;
	}

	/// Visit an element.
	virtual bool VisitExit(const XMLElement& elem) override
	{
		//printf("XMLElement EXIT: </%s>\n", elem.Value());

		return true;
	}

};