#pragma once


#include "blend2d.h"

#include "maths.hpp"
#include "shaper.h"
#include "geometry.h"

#include "chunkutil.h"
#include "svgxform.h"
#include "svgcolors.h"
#include "graphic.hpp"
#include "mmap.hpp"
#include "xmlscan.h"

#include <functional>
#include <charconv>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <stack>
#include <memory>


namespace svg
{
    using namespace ndt;


    //static charset whitespaceChars(",\t\n\f\r\v ");
    static charset digitChars("0123456789");
    static charset numberChars("0123456789.-+eE");


    constexpr auto SVG_MAX_ATTR = 128;
    constexpr auto SVG_MAX_DASHES = 8;
}


// Core enumerations and types
namespace svg
{
    using namespace maths;
    using namespace ndt;

    // Various Attributes to be found in an SVG image
    enum SVGAlignType {
        SVG_ALIGN_NONE = 0,
        SVG_ALIGN_MIN = 0,
        SVG_ALIGN_MID = 1,
        SVG_ALIGN_MAX = 2,
        SVG_ALIGN_MEET = 1,
        SVG_ALIGN_SLICE = 2,
    };

    enum SVGspreadType {
        SVG_SPREAD_PAD = 0,
        SVG_SPREAD_REFLECT = 1,
        SVG_SPREAD_REPEAT = 2
    };

    enum SVGGradientUnits {
        SVG_USER_SPACE = 0,
        SVG_OBJECT_SPACE = 1
    };

    enum SVGCoordinateUnits {
        SVG_UNITS_USER,
        SVG_UNITS_PX,
        SVG_UNITS_PT,
        SVG_UNITS_PC,
        SVG_UNITS_MM,
        SVG_UNITS_CM,
        SVG_UNITS_IN,
        SVG_UNITS_PERCENT,
        SVG_UNITS_EM,
        SVG_UNITS_EX
    };

    enum SVGflags {
        SVG_FLAGS_VISIBLE = 0x01
    };


    struct SVGCoordinate {
        float value;
        int units;
    };
}



namespace svg {
    // Represents the actual kind of data in the Attribute
    // union data section
    enum SVGAttributeKind
    {
          SVG_ATTR_KIND_INVALID = 0
        , SVG_ATTR_KIND_CHUNK
        , SVG_ATTR_KIND_BOOL                // bool
        , SVG_ATTR_KIND_INT                 // int
        , SVG_ATTR_KIND_NUMBER              // floating point number
        , SVG_ATTR_KIND_COORDINATE          // value/units
        , SVG_ATTR_KIND_TRANSFORM           // matrix
        , SVG_ATTR_KIND_PAINT               // color, gradient, pattern
//		, SVG_ATTR_KIND_STRING              // string
        , SVG_ATTR_KIND_POINTS              // points for a poly
		, SVG_ATTR_KIND_PATH                // path data
    };

    // A single structure to represent all possible kinds of attributes
    // for an SVG element
    struct SVGAttribute
    {
        std::string fName;                              // Name of the attribute
        int fKind;                                      // The kind of data

        union {
            DataChunk       fChunk;
            SVGCoordinate   fCoordinate;
            BLMatrix2D      fTransform;
            BLVarCore       fVar;                     // Cath all for multiple types
            BLArray <BLPoint> fPoints;                  // Poly points
            std::vector<ndt::PathSegment> fSegments;    // Path segments


        };

        SVGAttribute() :fKind(SVG_ATTR_KIND_INVALID) {}
        SVGAttribute(int kind):fKind(kind){}
        SVGAttribute(const SVGAttribute& other)
        {
            fName = other.fName;
            fKind = other.fKind;
            switch (fKind) {

            case SVG_ATTR_KIND_CHUNK:
                fChunk = other.fChunk;
                break;

            case SVG_ATTR_KIND_BOOL:
            case SVG_ATTR_KIND_INT:                // int
            case SVG_ATTR_KIND_NUMBER:              // floating point number
                blVarAssignWeak(&fVar, &other.fVar);
                break;

            case SVG_ATTR_KIND_COORDINATE:          // value/units
                fCoordinate = other.fCoordinate;
                break;

            case SVG_ATTR_KIND_TRANSFORM:
				fTransform = other.fTransform;
				break;

			case SVG_ATTR_KIND_POINTS:
				fPoints = other.fPoints;
				break;
                
            case SVG_ATTR_KIND_PATH:
				fSegments = other.fSegments;
				break;
            }
        }
       
        ~SVGAttribute() {
            switch (fKind)
            {

            case SVG_ATTR_KIND_POINTS:
                fPoints.~BLArray();
                break;
                
            case SVG_ATTR_KIND_PATH:
                fSegments.~vector();
                break;
                
            case SVG_ATTR_KIND_BOOL:
            case SVG_ATTR_KIND_INT:
			case SVG_ATTR_KIND_NUMBER:
				blVarDestroy(&fVar);
                break;
                
            default:
                break;
            }
        }

        void setKind(int kind) { fKind = kind; }
    };

}



namespace svg {
    using std::string; 
	using ndt::DataChunk;
    
    static SVGAttribute toEnum(const ndt::DataChunk& s);
	static SVGAttribute toNumber(const ndt::DataChunk& s);
    static SVGAttribute toCoordinate(const ndt::DataChunk& s);
    static SVGAttribute toTransform(const ndt::DataChunk& s);
    static SVGAttribute toDefault(const ndt::DataChunk& s);
    static SVGAttribute toColor(const ndt::DataChunk& s);
    static SVGAttribute toOpacity(const ndt::DataChunk& s);
    static SVGAttribute toStyle(const ndt::DataChunk& s);
    static SVGAttribute toPath(const DataChunk& s);

    
	// Create a map that takes a string as the key, and the value is a function that takes a const DataChunk& and returns a SVGAttribute
    
    std::map<string, std::function<SVGAttribute (const DataChunk& inChunk) > > gParseAttribute = {
        {"accent-height", toNumber}
        ,{"accumulate", toDefault}
        ,{"additive", toDefault}
        ,{"alignment-baseline", toDefault}
        ,{"allowReorder", toDefault}
        ,{"alphabetic", toNumber}
        ,{"amplitude", toDefault}
        ,{"arabic-form", toDefault}
        ,{"ascent", toNumber}
        ,{"attributeName", toDefault}
        ,{"attributeType", toDefault}
        ,{"autoReverse", toDefault}
        ,{"azimuth", toDefault}

        ,{"baseFrequency", toDefault}
        ,{"baseline-shift", toDefault}
        ,{"baseProfile", toDefault}
        ,{"bbox", toDefault}
        ,{"begin", toDefault}
        ,{"bias", toDefault}
        ,{"by", toDefault}

        ,{"calcMode", toDefault}
        ,{"cap-height", toNumber}
        ,{"class", toDefault}
        ,{"clip", toDefault}
        ,{"clip-path", toDefault}
        ,{"clip-rule", toDefault}
        ,{"clipPathUnits", toDefault}
        ,{"color", toColor}
        ,{"color-interpolation", toDefault}
        ,{"color-interpolation-filters", toDefault}
        ,{"color-profile", toDefault}
        ,{"color-rendering", toDefault}
        ,{"contentScriptType", toDefault}
        ,{"contentStyleType", toDefault}
        ,{"cursor", toDefault}
        ,{"cx", toCoordinate}
        ,{"cy", toCoordinate}

        ,{"d", toPath}
        ,{"decelerate", toDefault}
        ,{"descent", toNumber}
        ,{"diffuseConstant", toDefault}
        ,{"direction", toDefault}
        ,{"display", toDefault}
        ,{"divisor", toDefault}
        ,{"dominant-baseline", toDefault}
        ,{"dur", toDefault}
        ,{"dx", toNumber}
        ,{"dy", toNumber}

        ,{"edgeMode", toDefault}
        ,{"elevation", toDefault}
        ,{"enable-background", toDefault}
        ,{"end", toDefault}
        ,{"exponent", toNumber}
        ,{"externalResourcesRequired", toDefault}

        ,{"fill", toPaint}
        ,{"fill-opacity", toOpacity}
        ,{"fill-rule", toDefault}
        ,{"filter", toDefault}
        ,{"filterRes", toDefault}
        ,{"filterUnits", toDefault}
        ,{"flood-color", toColor}
        ,{"flood-opacity", toOpacity}
        ,{"font-family", toDefault}
        ,{"font-size", toDefault}
        ,{"font-size-adjust", toDefault}
        ,{"font-stretch", toDefault}
        ,{"font-style", toDefault}
        ,{"font-variant", toDefault}
        ,{"font-weight", toDefault}
        ,{"format", toDefault}
        ,{"from", toDefault}
        ,{"fx", toCoordinate}
        ,{"fy", toCoordinate}

        ,{"g1", toDefault}
        ,{"g2", toDefault}
        ,{"glyph-name", toDefault}
        ,{"glyph-orientation-horizontal", toDefault}
        ,{"glyph-orientation-vertical", toDefault}
        ,{"glyphRef", toDefault}
        ,{"gradientTransform", toTransform}
        ,{"gradientUnits", toDefault}

        ,{"hanging", toNumber}
        ,{"height", toCoordinate}
        ,{"horiz-adv-x", toNumber}
        ,{"horiz-origin-x", toNumber}

        ,{"id", toDefault}
        ,{"ideographic", toNumber}
        ,{"image-rendering", toDefault}
        ,{"in", toDefault}
        ,{"in2", toDefault}
        ,{"intercept", toDefault}

        ,{"k", toNumber }
        ,{"k1", toNumber }
        ,{"k2", toNumber }
        ,{"k3", toNumber}
        ,{"k4", toNumber }
        ,{"kernelMatrix", toTransform }
        ,{"kernelUnitLength", toNumber }
        ,{"kerning", toDefault}
        ,{"keyPoints", toDefault}
        ,{"keySplines", toDefault}
        ,{"keyTimes", toDefault}

        ,{"lang", toDefault}
        ,{"lengthAdjust", toDefault}
        ,{"letter-spacing", toDefault }
        ,{"lighting-color", toColor }
        ,{"limitingConeAngle", toDefault }
        ,{"local", toDefault }

        ,{"marker-end", toDefault }
        ,{"marker-mid", toDefault }
        ,{"marker-start", toDefault }
        ,{"markerHeight", toCoordinate }
        ,{"mask", toDefault}
        ,{"maskContentUnits", toDefault }
        ,{"maskUnits", toDefault }
        ,{"mathematical", toNumber }
        ,{"max", toDefault}
        ,{"media",toDefault}
        ,{"method",toDefault}
        ,{"min", toDefault}
        ,{"mode", toDefault}

        ,{"name", toDefault}
        ,{"numOctaves", toDefault}

        ,{"offset", toDefault}
        ,{"onabort", toDefault }
        ,{"onactivate", toDefault }
        ,{"onbegin", toDefault }
        ,{"onclick", toDefault }
        ,{"onend", toDefault }
        ,{"onerror", toDefault }
        ,{"onfocusin", toDefault }
        ,{"onfocusout", toDefault }
        ,{"onload", toDefault }
        ,{"onmousedown", toDefault }
        ,{"onmousemove", toDefault }
        ,{"onmouseout", toDefault }
        ,{"onmouseover", toDefault }
        ,{"onmouseup", toDefault }
        ,{"onrepeat", toDefault }
        ,{"onresize", toDefault }
        ,{"onscroll", toDefault }
        ,{"onunload", toDefault }
        ,{"onzoom", toDefault }
        ,{"opacity", toDefault }
        ,{"operator", toDefault }
        ,{"order", toDefault }
        ,{"orient", toDefault }
        ,{"orientation", toDefault }
        ,{"origin", toDefault }
        ,{"overflow", toDefault }
        ,{"overline-position", toDefault }
        ,{"overline-thickness", toDefault }

        ,{"panose-1", toDefault }
        ,{"paint-order", toDefault }
        ,{"pathLength", toDefault }
        ,{"patternContentUnits", toDefault }
        ,{"patternTransform", toDefault }
        ,{"patternUnits", toDefault }
        ,{"pointer-events", toDefault }
        ,{"points", toDefault }
        ,{"pointsAtX", toDefault }
        ,{"pointsAtY", toDefault }
        ,{"pointsAtZ", toDefault }
        ,{"preserveAlpha", toDefault }
        ,{"preserveAspectRatio", toDefault }
        ,{"primitiveUnits", toDefault }


        ,{"r", toDefault }
        ,{"radius", toDefault }
        ,{"refX", toDefault }
        ,{"refY", toDefault }
        ,{"rendering-intent", toDefault }
        ,{"repeatCount", toDefault }
        ,{"repeatDur", toDefault }
        ,{"requiredExtensions", toDefault }
        ,{"requiredFeatures", toDefault }
        ,{"restart", toDefault }
        ,{"result", toDefault }
        ,{"rotate", toDefault }
        ,{"rx", toDefault }
        ,{"ry", toDefault }

        ,{"scale", toDefault }
        ,{"seed", toDefault }
        ,{"shape-rendering", toDefault }
        ,{"slope", toDefault }
        ,{"spacing", toDefault }
        ,{"specularConstant", toDefault }
        ,{"specularExponent", toDefault }
        ,{"speed", toDefault }
        ,{"spreadMethod", toDefault }
        ,{"startOffset", toDefault }
        ,{"stdDeviation", toDefault }
        ,{"stemh", toDefault }
        ,{"stemv", toDefault }
        ,{"stitchTiles", toDefault }
        ,{"stop-color", toDefault }
        ,{"stop-opacity", toDefault }
        ,{"strikethrough-position", toDefault }
        ,{"strikethrough-thickness", toDefault }
        ,{"string", toDefault }
        ,{"stroke", toPaint }
        ,{"stroke-dasharray", toDefault }
        ,{"stroke-dashoffset", toDefault }
        ,{"stroke-linecap", toDefault }
        ,{"stroke-miterlimit", toDefault }
        ,{"stroke-opacity", toDefault }
        ,{"stroke-width", toDefault }
        ,{"style", toStyle }
        ,{"surfaceScale", toDefault }
        ,{"systemLanguage", toDefault }


        ,{"tableValues", toDefault }
        ,{"target", toDefault }
        ,{"targetX", toDefault }
        ,{"targetY", toDefault }

        ,{"text-anchor", toDefault }
        ,{"text-decoration", toDefault }
        ,{"text-rendering", toDefault }
        ,{"textLength", toDefault }

        ,{"to", toDefault }
        ,{"transform", toDefault }
        ,{"type", toDefault }


        ,{"underline-position", toDefault }
        ,{"underline-thickness", toDefault }
        ,{"unicode", toDefault }
        ,{"unicode-bidi", toDefault }
        ,{"unicode-range", toDefault }
        ,{"units-per-em", toDefault }


        ,{"v-alphabetic", toDefault }
        ,{"v-hanging", toDefault }
        ,{"v-ideographic", toDefault }
        ,{"v-mathematical", toDefault }
        ,{"values", toDefault }
        ,{"version", toDefault }
        ,{"v-adv-y", toDefault }
        ,{"v-origin-x", toDefault }
        ,{"v-origin-y", toDefault }
        ,{"viewBox", toDefault }
        ,{"visibility", toDefault }

        ,{"width", toCoordinate }
        ,{"widths", toDefault }
        ,{"word-spacing", toDefault }
        ,{"writing-mode", toDefault }


        ,{"x", toCoordinate }
        ,{"x-height", toDefault }
        ,{"x1", toCoordinate }
        ,{"x2", toCoordinate }
        ,{"xChannelSelector", toDefault }
        ,{"xlink:actuate", toDefault }
        ,{"xlink:arcrole", toDefault }
        ,{"xlink:href", toDefault }
        ,{"xlink:role", toDefault }
        ,{"xlink:show", toDefault }
        ,{"xlink:title", toDefault }
        ,{"xlink:type", toDefault }
        ,{"xml:base", toDefault }
        ,{"xml:lang", toDefault }
        ,{"xml:space", toDefault }

        ,{"y", toCoordinate }
        ,{"y1", toCoordinate }
        ,{"y2", toCoordinate }
        ,{"yChannelSelector", toDefault }
        ,{"z", toCoordinate }
        ,{"zoomAndPan", toDefault }

    };
    

    // Routines to convert from a plain chunk to an SVGAttribute
   // All known attributes are here, even if we don't have any specific
   // routine to parse the value
      // The easiest conversion
   // just transfer the chunk value
    SVGAttribute toDefault(const ndt::DataChunk& s)
    {
        SVGAttribute attr(SVG_ATTR_KIND_CHUNK);
        attr.fChunk = s;

        return attr;
    }

    SVGAttribute toInt(const DataChunk& inChunk)
    {
        auto s = inChunk;
        auto anint = chunk_to_u64(s);
        
        SVGAttribute attr(SVG_ATTR_KIND_INT);
		blVarAssignUInt64(&attr.fVar, anint);
        

		// compare big list of strings to their enum values
		// put the enum value in the attribute and return it
        //
        
        return attr;
    }

    
    // toNumber()
    // converts attribute to a floating point number
    // without any units, just a plain number
    SVGAttribute toNumber(const ndt::DataChunk& s)
    {
        SVGAttribute attr(SVG_ATTR_KIND_NUMBER);
        DataChunk numChunk = s;
        double anum = chunk_to_double(numChunk);

		blVarAssignDouble(&attr.fVar, anum);


        return attr;
    }

    //
    // Coordinate parsing
    // Turn a units indicator into an enum
    static int parseCoordUnits(const DataChunk& units)
    {
        // if the chunk is blank, then return user units
        if (!units)
            return SVG_UNITS_USER;

        if (units[0] == 'p' && units[1] == 'x')
            return SVG_UNITS_PX;
        else if (units[0] == 'p' && units[1] == 't')
            return SVG_UNITS_PT;
        else if (units[0] == 'p' && units[1] == 'c')
            return SVG_UNITS_PC;
        else if (units[0] == 'm' && units[1] == 'm')
            return SVG_UNITS_MM;
        else if (units[0] == 'c' && units[1] == 'm')
            return SVG_UNITS_CM;
        else if (units[0] == 'i' && units[1] == 'n')
            return SVG_UNITS_IN;
        else if (units[0] == '%')
            return SVG_UNITS_PERCENT;
        else if (units[0] == 'e' && units[1] == 'm')
            return SVG_UNITS_EM;
        else if (units[0] == 'e' && units[1] == 'x')
            return SVG_UNITS_EX;

        return SVG_UNITS_USER;
    }

    static SVGCoordinate parseCoordinateRaw(const DataChunk& s)
    {
        SVGCoordinate coord{ 0, SVG_UNITS_USER };
        char buf[64]{};
        DataChunk numChunk = make_chunk_size(buf, 64);
        auto nextPart = scanNumber(s, numChunk);
        coord.value = chunk_to_double(numChunk);
        coord.units = parseCoordUnits(nextPart);

        return coord;
    }
    
    SVGAttribute toCoordinate(const ndt::DataChunk& s)
    {
        SVGCoordinate coord = parseCoordinateRaw(s);

        SVGAttribute attr(SVG_ATTR_KIND_COORDINATE);
        attr.fCoordinate = coord;

        return attr;
    }

    //
    // path parsing
    //
    SVGAttribute toPath(const ndt::DataChunk& s)
    {
        return toDefault(s);
    }
    
    //
    // Transform parsing
    //
    SVGAttribute toTransform(const ndt::DataChunk& s)
    {
        return toDefault(s);
    }

//===========================================
// Color parsing
//===========================================
    static vec4b parseColorHex(const DataChunk& chunk)
    {
        // BUGBUG - making a copy of the chunk to use sscanf
        char str[64];
        copy_to_cstr(str, 63, chunk);

        unsigned int r = 0, g = 0, b = 0;
        if (sscanf_s(str, "#%2x%2x%2x", &r, &g, &b) == 3)		// 2 digit hex
            return rgb(r, g, b);
        if (sscanf_s(str, "#%1x%1x%1x", &r, &g, &b) == 3)		// 1 digit hex, e.g. #abc -> 0xccbbaa
            return rgb(r * 17, g * 17, b * 17);			    // same effect as (r<<4|r), (g<<4|g), ..

        // if not one of those cases, just return a mid-gray
        return rgb(128, 128, 128);
    }

    // Parse rgb color. The pointer 'str' must point at "rgb(" (4+ characters).
    // This function returns gray (rgb(128, 128, 128) == '#808080') on parse errors
    // for backwards compatibility. Note: other image viewers return black instead.

    static vec4b parseColorRGB(const DataChunk& inChunk)
    {
        // skip past the leading "rgb("
        DataChunk s = inChunk;
        auto leading = chunk_token(s, "(");

        // s should now point to the first number
        // and 'leading' should contain 'rgb'
        // BUGBUG - we can check 'leading' to see if it's actually 'rgb'
        // but we'll just assume it is for now

        // get the numbers by separating at the ')'
        auto nums = chunk_token(s, ")");

        // So, now nums contains the individual numeric values, separated by ','
        // The individual numeric values are either
        // 50%
        // 50

        int i = 0;
        uint8_t rgbi[4]{};

        // Get the first token, which is red
        // if it's not there, then return gray
        auto num = chunk_token(nums, ",");
        if (size(num) < 1)
            return rgba(128, 128, 128, 0);

        while (num)
        {
            writeChunk(num);
            if (chunk_find_char(num, '%'))
            {
                // it's a percentage
                // BUGBUG - we're assuming it's a range of [0..255]
                auto cv = parseCoordinateRaw(num);
                rgbi[i] = (uint8_t)(cv.value / 100.0f * 255.0f);
            }
            else
            {
                // it's a regular number
                rgbi[i] = (uint8_t)parseCoordinateRaw(num).value;
            }
            i++;
            num = chunk_token(nums, ",");
        }

        return rgb(rgbi[0], rgbi[1], rgbi[2]);
    }

    static vec4b parseColorName(const DataChunk& inChunk)
    {
        std::string cName = std::string(inChunk.fStart, inChunk.fEnd);

        if (!svg::colors.contains(cName))
            return rgb(0, 0, 0);

        return svg::colors[cName];
    }

    static vec4b parseColor(const DataChunk& inChunk)
    {
        DataChunk str = inChunk;
        DataChunk rgbStr = make_chunk_cstr("rgb(");
        DataChunk rgbaStr = make_chunk_cstr("rgba(");

        size_t len = 0;
        str = chunk_ltrim(str, wspChars);

        len = size(str);
        if (len >= 1 && *str == '#')
        {
            return parseColorHex(str);
        }
        else if (chunk_starts_with(str, rgbStr) || chunk_starts_with(str, rgbaStr))
        {
            return parseColorRGB(str);
        }

        return parseColorName(str);
    }
    
    SVGAttribute toColor(const ndt::DataChunk& s)
    {
		SVGAttribute attr(SVG_ATTR_KIND_PAINT);
        auto c = parseColor(s);
		blVarAssignRgba32(&attr.fVar, c.value);
		
        return attr;
    }

    SVGAttribute toOpacity(const ndt::DataChunk& s)
    {
        return toNumber(s);
    }

    SVGAttribute toStyle(const ndt::DataChunk& s)
    {
        return toDefault(s);
    }

    

    // convert a name and chunk into an SVGAttribute
    static SVGAttribute parseAttribute(std::string name, const DataChunk& inChunk)
    {
        if (gParseAttribute.contains(name)) {
            return gParseAttribute[name](inChunk);
        }

        return SVGAttribute();
    }
}

namespace svg {

    
    class SVGBaseElement
    {
    private:

        std::shared_ptr<SVGBaseElement> fRoot;

        std::string fName{};    // The tag name of the element

        ndt::DataChunk fContent;
        std::map<std::string, svg::SVGAttribute> fAttributes;
        std::vector<std::shared_ptr<SVGBaseElement>> fChildren;
        std::map<std::string, SVGBaseElement&> fDefinitions;

    public:
        SVGBaseElement(std::shared_ptr<SVGBaseElement> root);

        void setRoot(std::shared_ptr<SVGBaseElement> root);
        void setName(std::string name);

        void addAttribute(const svg::SVGAttribute& attr);
        void addXmlAttribute(const std::string name, const DataChunk& value);
        void addChild(std::shared_ptr<SVGBaseElement> child);
        void addContent(ndt::DataChunk& content);

        void parseContent(ndt::DataChunk& content);
        void scanElement(ndt::DataChunk& s);
        void scanChunk(ndt::DataChunk& inChunk);
    };


    INLINE SVGBaseElement::SVGBaseElement(std::shared_ptr<SVGBaseElement> root)
        : fRoot(root)
    {}

    INLINE void SVGBaseElement::setRoot(std::shared_ptr<SVGBaseElement> root)
    {
        fRoot = root;
    }

    INLINE void SVGBaseElement::setName(std::string name) { fName = name; }
    
    void SVGBaseElement::addAttribute(const svg::SVGAttribute& attr) {
        auto apair = std::make_pair(attr.fName, attr);
        fAttributes.insert(apair);
    }

    void SVGBaseElement::addXmlAttribute(const std::string name, const DataChunk& value)
    {
		auto attr = svg::parseAttribute(name, value);
		addAttribute(attr);
    }
    
    INLINE void SVGBaseElement::addChild(std::shared_ptr<SVGBaseElement> node)
    {
        fChildren.push_back(node);
    }

    INLINE void SVGBaseElement::addContent(ndt::DataChunk& content)
    {
        auto node = std::make_shared<SVGBaseElement>(fRoot);
        node->fContent = content;
        addChild(node);
    }

    INLINE void SVGBaseElement::parseContent(ndt::DataChunk& content)
    {
        auto s = content;
        s = chunk_trim(s, wspChars);

        // If the content is just blank, then don't add it
        if (!s)
            return;

        // Add a child element that is the content
        addContent(content);
    }


    // Scan the input chunk
    INLINE void SVGBaseElement::scanIterator(DataChunk& s)
    {


		// If the content is just blank, then don't add it
		if (!s)
			return;



        do
        {
            elem = iter.next();

            if (!elem)
                break;
            
            // use the contents of the XmlElement 
            // to initialize ourself
            // or if it's a new child, create that new
			// child and add it to ourself
			if (elem.isStart())
			{
				// it's a start tag aborb the contents ourself
                setName(elem.name());
				for (auto& attr : elem.attributes())
				{
					addXmlAttribute(attr.first, attr.second);
				}

                
				auto node = std::make_shared<SVGBaseElement>(fRoot);
				node->setName(elem.fName);
				node->scanElement(elem.fContent);
				addChild(node);
			}
			else
			{
				// it's content
				parseContent(elem.fContent);
			}
		} while (elem);

    }

}

namespace svg {
    struct SVGDocument : public SVGBaseElement
    {

        static std::shared_ptr<SVGBaseElement> createFromChunk(const DataChunk& inChunk)
        {
            DataChunk s = inChunk;
            s = chunk_trim(s, wspChars);

            XmlElementIterator iter(s);
            XmlElement elem{};
            
            auto doc = std::make_shared<SVGBaseElement>(nullptr);
            doc->setRoot(doc);
            doc->scanChunk(s);

            return doc;
        }
    };
}
