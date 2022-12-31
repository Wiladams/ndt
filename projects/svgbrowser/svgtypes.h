#pragma once

#include "chunkutil.h"
#include "shaper.h"
#include "maths.hpp"
#include "coloring.h"
#include "svgcolors.h"

// https://www.w3.org/TR/css3-values/#numbers
// SVGDimension
// SVGColor
// SVGViewbox
//

namespace svg {
    using namespace ndt;
    using namespace maths;
}

namespace svg {
    // SVG Element Attributes are of fixed types
// The SVGAttributeKind enum defines the types
// https://www.w3.org/TR/SVG2/attindex.html#PresentationAttributes
    enum SVGAttributeKind
    {
        SVG_ATTR_KIND_INVALID = 0
        , SVG_ATTR_KIND_CHUNK               // If there is no better representation

        , SVG_ATTR_KIND_NUMBER              // floating point number
		, SVG_ATTR_KIND_NUMBERORPERCENT     // floating point number or percentage, range [0..1]
        , SVG_ATTR_KIND_DIMENSION           // value/units
		, SVG_ATTR_KIND_COLOR               // color
        , SVG_ATTR_KIND_PAINT               // color, gradient, pattern
        , SVG_ATTR_KIND_TRANSFORM           // matrix
		, SVG_ATTR_KIND_ENUM 			    // enumeration of known (typically string) values
        
        , SVG_ATTR_KIND_BOOL                // bool
        , SVG_ATTR_KIND_INT                 // int
        , SVG_ATTR_KIND_STRING              // string
        , SVG_ATTR_KIND_POINTS              // points for a poly
        , SVG_ATTR_KIND_PATH                // path data
    };


    // Enums that represent choices for individual attributes
    // 
    enum SVGClipRule
    {
        SVG_CLIP_RULE_NONZERO = 0
        , SVG_CLIP_RULE_EVENODD
    };

    enum SVGColorInterpolation {
		SVG_COLOR_INTERPOLATION_AUTO
		, SVG_COLOR_INTERPOLATION_SRGB
		, SVG_COLOR_INTERPOLATION_LINEARRGB
    };
    
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



    enum SVGflags {
        SVG_FLAGS_VISIBLE = 0x01
    };



    enum SVGDimensionUnits
    {
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
}

namespace svg {
    // A map between a name and a presentation attribute kind
    // Functions can use this mapping to determine how to parse the data
    // https://www.w3.org/TR/SVG2/attindex.html#PresentationAttributes
    // 
    // It's questionable whether this mapping is needed.  
    // It might be useful when we're doing generic parsing of attributes
    // but not so useful when we're doing highly directed parsing, where we
    // explicitly already know the types we're parsing.
    // 
    // It might be useful to help the programmer to determine which basic
    // type parser to use.  So, we'll keep it as informational for now
    std::map<std::string, int> SVGPresentationAttributeMap = {
        {"alignment-baseline", SVG_ATTR_KIND_CHUNK}
        ,{"baseline-shift", SVG_ATTR_KIND_CHUNK}
        ,{"clip", SVG_ATTR_KIND_ENUM}
        ,{"clip-path", SVG_ATTR_KIND_CHUNK}
        ,{"clip-rule", SVG_ATTR_KIND_CHUNK}
        ,{"color", SVG_ATTR_KIND_CHUNK}
        ,{"color-interpolation", SVG_ATTR_KIND_CHUNK}
        ,{"color-interpolation-filters", SVG_ATTR_KIND_CHUNK}
        ,{"color-rendering", SVG_ATTR_KIND_CHUNK}
        ,{"cursor", SVG_ATTR_KIND_CHUNK}
        ,{"direction", SVG_ATTR_KIND_CHUNK}
        ,{"display", SVG_ATTR_KIND_CHUNK}
        ,{"dominant-baseline", SVG_ATTR_KIND_CHUNK}
        ,{"fill", SVG_ATTR_KIND_PAINT}
        ,{"fill-opacity", SVG_ATTR_KIND_NUMBERORPERCENT}
        ,{"fill-rule", SVG_ATTR_KIND_ENUM}
        ,{"filter", SVG_ATTR_KIND_CHUNK}
        ,{"flood-color", SVG_ATTR_KIND_CHUNK}
        ,{"flood-opacity", SVG_ATTR_KIND_NUMBERORPERCENT}
        ,{"font-family", SVG_ATTR_KIND_CHUNK}
        ,{"font-size", SVG_ATTR_KIND_DIMENSION}
        ,{"font-size-adjust", SVG_ATTR_KIND_CHUNK}
        ,{"font-stretch", SVG_ATTR_KIND_CHUNK}
        ,{"font-style", SVG_ATTR_KIND_CHUNK}
        ,{"font-variant", SVG_ATTR_KIND_CHUNK}
        ,{"font-weight", SVG_ATTR_KIND_CHUNK}
        ,{"glyph-orientation-horizontal", SVG_ATTR_KIND_CHUNK}
        ,{"glyph-orientation-vertical", SVG_ATTR_KIND_CHUNK}
        ,{"image-rendering", SVG_ATTR_KIND_CHUNK}
        ,{"lighting-color", SVG_ATTR_KIND_COLOR}
        ,{"marker-end", SVG_ATTR_KIND_CHUNK}
        ,{"marker-mid", SVG_ATTR_KIND_CHUNK}
        ,{"marker-start", SVG_ATTR_KIND_CHUNK}
        ,{"mask", SVG_ATTR_KIND_CHUNK}
        ,{"opacity", SVG_ATTR_KIND_NUMBERORPERCENT}
        ,{"overflow", SVG_ATTR_KIND_CHUNK}
        ,{"paint-order", SVG_ATTR_KIND_ENUM}                        // normal | [fill || stroke || markers]
        ,{"pointer-events", SVG_ATTR_KIND_CHUNK}
        ,{"shape-rendering", SVG_ATTR_KIND_CHUNK}
        ,{"stop-color", SVG_ATTR_KIND_CHUNK}
        ,{"stop-opacity", SVG_ATTR_KIND_NUMBERORPERCENT}
        ,{"stroke", SVG_ATTR_KIND_PAINT}
        ,{"stroke-dasharray", SVG_ATTR_KIND_CHUNK}
        ,{"stroke-dashoffset", SVG_ATTR_KIND_DIMENSION}
        ,{"stroke-linecap", SVG_ATTR_KIND_ENUM}                     // butt, round, square
        ,{"stroke-linejoin", SVG_ATTR_KIND_ENUM}                    // miter, miter-clip, round, bevel, arcs
        ,{"stroke-miterlimit", SVG_ATTR_KIND_NUMBER}
        ,{"stroke-opacity", SVG_ATTR_KIND_NUMBERORPERCENT}
        ,{"stroke-width", SVG_ATTR_KIND_DIMENSION}
        ,{"text-anchor", SVG_ATTR_KIND_ENUM}                       // start, middle, end
        ,{"text-decoration", SVG_ATTR_KIND_CHUNK}
        ,{"text-rendering", SVG_ATTR_KIND_CHUNK}
        ,{"transform", SVG_ATTR_KIND_TRANSFORM}
        ,{"unicode-bidi", SVG_ATTR_KIND_CHUNK}
        ,{"vector-effect", SVG_ATTR_KIND_CHUNK}
        ,{"vertical-align", SVG_ATTR_KIND_DIMENSION}               // SVG 2.0
        ,{"visibility", SVG_ATTR_KIND_CHUNK}
        ,{"word-spacing", SVG_ATTR_KIND_DIMENSION}
        ,{"letter-spacing", SVG_ATTR_KIND_CHUNK}
    };
}

namespace svg {
    // simple type parsing
    static INLINE maths::vec4b toColor(const ndt::DataChunk& inChunk);
    static INLINE int64_t toInteger(const DataChunk& inChunk);
    static INLINE float toNumber(const DataChunk& inChunk);
    static INLINE std::string toString(const DataChunk& inChunk);
	
    // compound types
    // toDimension, can't forward declare this one
    // 	static INLINE SVGViewbox toViewbox(const DataChunk& inChunk)
    
	// Utility, for viewbox, points, etc
    static INLINE float nextNumber(DataChunk& inChunk, const charset& delims);
}

/// Some utility routines
namespace svg {

    // return a number next in a list of numbers
    static INLINE float nextNumber(DataChunk& inChunk, const charset& delims)
    {
        DataChunk numChunk;
        numChunk = chunk_token(inChunk, delims);

        return (float)chunk_to_double(numChunk);
    }

    static INLINE int64_t toInteger(const DataChunk& inChunk)
    {
        DataChunk s = inChunk;
        return chunk_to_i64(s);
    }

    // toNumber
    // a floating point number
	static INLINE float toNumber(const DataChunk& inChunk)
	{
        DataChunk s = inChunk;
		return (float)chunk_to_double(s);
	}

    static INLINE std::string toString(const DataChunk& inChunk)
    {
        std::string str(inChunk.fStart, inChunk.fEnd);
        return str;
    }
}


// Specific types of attributes
//==============================================================================
// SVGDimension
// used for length, time, frequency, resolution, location
//==============================================================================
namespace svg
{



    // Turn a units indicator into an enum
    static SVGDimensionUnits parseDimensionUnits(const DataChunk& units)
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
    
    struct SVGDimension {
        float fValue;
        SVGDimensionUnits fUnits;
        
        float fDeviceValue;
        
        SVGDimension()
			: fValue(0.0f)
			, fUnits(SVGDimensionUnits::SVG_UNITS_USER)
			, fDeviceValue(0.0f)
		{
		}
	    

        SVGDimension(float value, SVGDimensionUnits units)
		: fValue(value)
		, fUnits(units)
		, fDeviceValue(0.0f)
	    {
	    }

        float value() const { return fValue; }
        SVGDimensionUnits units() const { return fUnits; }
        float calculatePixels(float length=1.0, float orig=0, float dpi = 96)
        {
            //static float svg_convertToPixels(float orig, float length)


            switch (fUnits) {
            case SVG_UNITS_USER:		return fValue;
            case SVG_UNITS_PX:			return fValue;
            case SVG_UNITS_PT:			return fValue / 72.0f * dpi;
            case SVG_UNITS_PC:			return fValue / 6.0f * dpi;
            case SVG_UNITS_MM:			return fValue / 25.4f * dpi;
            case SVG_UNITS_CM:			return fValue / 2.54f * dpi;
            case SVG_UNITS_IN:			return fValue * dpi;
            //case SVG_UNITS_EM:			return fValue * attr.fontSize;
            //case SVG_UNITS_EX:			return fValue * attr.fontSize * 0.52f; // x-height of Helvetica.
            case SVG_UNITS_PERCENT:	return orig + fValue / 100.0f * length;

            }

            return fValue;

        }
        
        SVGDimension& reset(float value, SVGDimensionUnits units)
        {
			fValue = value;
			fUnits = units;
			fDeviceValue = 0.0f;
			return *this;
        }
        
        static SVGDimension fromChunk(const DataChunk &inChunk)
		{
            SVGDimension coord{ };

            DataChunk s = inChunk;
            DataChunk numChunk;
            auto nextPart = scanNumber(s, numChunk);
            float value = (float)chunk_to_double(numChunk);
            SVGDimensionUnits units = parseDimensionUnits(nextPart);
            coord.reset(value, units);
            
            return coord;
		}
    };


    
    static INLINE SVGDimension toDimension(const DataChunk& inChunk)
    {
        return SVGDimension::fromChunk(inChunk);
    }

    
    struct SVGPoint {
        float fX;
        float fY;

        SVGPoint(float x, float y) : fX(x), fY(y) {}
        SVGPoint() : fX(0), fY(0) {}

        float x() const { return fX; }
        float y() const { return fY; }

        static SVGPoint fromChunk(const DataChunk& inChunk)
        {
            SVGPoint point{};

            DataChunk s = inChunk;
            DataChunk numChunk;
            charset numDelims = wspChars + ',';

            point.fX = nextNumber(s, numDelims);
            point.fY = nextNumber(s, numDelims);

            return point;
        }


    };
}

//======================================================
// Definition of SVG Paint
//======================================================
namespace svg {
    // Representation of color according to CSS specification
	// https://www.w3.org/TR/css-color-4/#typedef-color
    // Over time, this structure could represent the full specification
    // but for practical purposes, we'll focus on rgb, rgba for now
    /*
    <color> = <absolute-color-base> | currentcolor | <system-color> 

    <absolute-color-base> = <hex-color> | <absolute-color-function> | <named-color> | transparent
    <absolute-color-function> = <rgb()> | <rgba()> |
                            <hsl()> | <hsla()> | <hwb()> |
                            <lab()> | <lch()> | <oklab()> | <oklch()> |
                            <color()>
    */

 

 
    
}

namespace svg {

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
            auto cv = SVGDimension::fromChunk(num);
            //writeChunk(num);
            if (chunk_find_char(num, '%'))
            {
                // it's a percentage
                // BUGBUG - we're assuming it's a range of [0..255]

                rgbi[i] = (uint8_t)(cv.value() / 100.0f * 255.0f);
            }
            else
            {
                // it's a regular number
                rgbi[i] = (uint8_t)cv.value();
            }
            i++;
            num = chunk_token(nums, ",");
        }

        return rgb(rgbi[0], rgbi[1], rgbi[2]);
    }

    static vec4b parseColorName(const DataChunk& inChunk)
    {
        std::string cName = std::string(inChunk.fStart, inChunk.fEnd);

        // If named color not found
        // or name == "none"
        // return fully transparent black
        // BUGBUG - this is different than not having a color attribute
        // in which case, we might want to eliminate color, and allow ancestor's color to come through
        if (!svg::colors.contains(cName))
            return rgba(0, 0, 0, 0);

        return svg::colors[cName];
    }

    struct SVGColor
    {
        maths::vec4b fColor;
        bool fIsNone;

        SVGColor(const maths::vec4b c)
        {
            fColor = c;
            fIsNone = false;
        }

        static SVGColor fromChunk(const DataChunk& inChunk)
        {
            DataChunk str = inChunk;
            DataChunk rgbStr = make_chunk_cstr("rgb(");
            DataChunk rgbaStr = make_chunk_cstr("rgba(");

            size_t len = 0;

            len = size(str);
            if (len >= 1 && *str == '#')
            {
                return SVGColor(parseColorHex(str));
            }
            else if (chunk_starts_with(str, rgbStr) || chunk_starts_with(str, rgbaStr))
            {
                return SVGColor(parseColorRGB(str));
            }

            return SVGColor(parseColorName(str));
        }
    };
 

    static INLINE maths::vec4b toColor(const DataChunk& inChunk)
    {
        DataChunk str = inChunk;
        DataChunk rgbStr = make_chunk_cstr("rgb(");
        DataChunk rgbaStr = make_chunk_cstr("rgba(");

        size_t len = 0;

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
}

namespace svg {
    
    //======================================================
    // SVGViewbox
    //======================================================
    
    struct SVGViewbox {
        float fX{};
        float fY{};
        float fWidth{};
        float fHeight{};


        float x() const { return fX; }
        float y() const { return fY; }
        float width() const { return fWidth; }
        float height() const {return fHeight;}
        
        static SVGViewbox fromChunk(const DataChunk& inChunk)
        {

			SVGViewbox viewbox{ };

            if (!inChunk)
                return viewbox;

			DataChunk s = inChunk;
			DataChunk numChunk;
            charset numDelims = wspChars+',';
                
            viewbox.fX = nextNumber(s, numDelims);
			viewbox.fY = nextNumber(s, numDelims);
			viewbox.fWidth = nextNumber(s, numDelims);
			viewbox.fHeight = nextNumber(s, numDelims);

			return viewbox;
        }
    };

	static INLINE SVGViewbox toViewbox(const DataChunk& inChunk)
	{
		return SVGViewbox::fromChunk(inChunk);
	}
    
}

namespace svg
{


    static std::vector<SVGPoint> toPoints(const DataChunk &inChunk)
	{
		std::vector<SVGPoint> points;

		DataChunk s = inChunk;
		charset numDelims = wspChars + ',';

		while (s)
		{
			SVGPoint p;
			p.fX = nextNumber(s, numDelims);
			p.fY = nextNumber(s, numDelims);
			points.push_back(p);
		}

		return points;
	}
}

namespace svg
{
    //
    // parsing transforms
    //
    static DataChunk parseTransformArgs(const DataChunk& inChunk, double* args, int maxNa, int& na)
    {
        na = 0;

        DataChunk s = inChunk;

        // Skip past everything until we see a '('
		s = chunk_find_char(s, '(');

        // If we got to the end of the chunk, and did not see the '('
        // then just return
        if (!s)
            return s;

        // by the time we're here, we're sitting right on top of the 
        //'(', so skip past that to get to what should be the numbers
        s++;

        // We want to create a  chunk that contains all the numbers
        // without the closing ')', so create a chunk that just
        // represents that range.
        // Start the chunk at the current position
        // and expand it after we find the ')'
        DataChunk item = s;
        item.fEnd = item.fStart;

        // scan until the closing ')'
        s = chunk_find_char(s, ')');


        // At this point, we're either sitting at the ')' or at the end
        // of the chunk.  If we're at the end of the chunk, then we
        // didn't find the closing ')', so just return
        if (!s)
            return s;

        // We found the closing ')', so if we use the current position
        // as the end (sitting on top of the ')', the item chunk will
        // perfectly represent the numbers we want to parse.
        item.fEnd = s.fStart;

        // Create a chunk that will represent a specific number to be parsed.
        DataChunk numChunk{};

        // Move the source chunk cursor past the ')', so that whatever
        // needs to use it next is ready to go.
        s++;

        // Now we're ready to parse the individual numbers
		auto numDelims = wspChars + ',';
        
        while (item) {
            if (na >= maxNa)
                break;
            
			args[na++] = nextNumber(item, numDelims);
        }

        return s;
    }

    static BLMatrix2D toMatrix(const DataChunk& inChunk)
    {
		BLMatrix2D res = BLMatrix2D::makeIdentity();
        
        double t[6];
        int na = 0;
        DataChunk s = inChunk;
        s = parseTransformArgs(s, t, 6, na);

        if (na != 6)
            return res;

		res.reset(t[0], t[1], t[2], t[3], t[4], t[5]);

        return res;
    }
    
    /*
    static DataChunk svg_parseTranslate(BLMatrix2D& xform, const DataChunk& inChunk)
    {

        float args[2];
        int na = 0;
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, args, 2, na);
        if (na == 1)
            args[1] = 0.0;

        xform.translate(args[0], args[1]);

        return s;
    }

    static DataChunk svg_parseScale(BLMatrix2D& xform, const DataChunk& inChunk)
    {
        float args[2]{ 0 };
        int na = 0;
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, args, 2, na);

        if (na == 1)
            args[1] = args[0];

        xform.scale(args[0], args[1]);

        return s;
    }

    static DataChunk svg_parseSkewX(BLMatrix2D& xform, const DataChunk& inChunk)
    {
        float args[1];
        int na = 0;
        float t[6];
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, args, 1, na);

        xform.resetToSkewing(args[0], 0.0f);

        return s;
    }

    static DataChunk svg_parseSkewY(BLMatrix2D& xform, const DataChunk& inChunk)
    {
        float args[1]{ 0 };
        int na = 0;
        float t[6]{ 0 };
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, args, 1, na);

        xform.resetToSkewing(0.0f, args[0]);

        return s;
    }

    static DataChunk svg_parseRotate(BLMatrix2D& xform, const DataChunk& inChunk)
    {
        float args[3]{ 0 };
        int na = 0;
        float m[6]{ 0 };
        float t[6]{ 0 };
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, args, 3, na);
        if (na == 1)
            args[1] = args[2] = 0.0f;

        svg_xformIdentity(m);

        if (na > 1) {
            svg_xformSetTranslation(t, -args[1], -args[2]);
            svg_xformMultiply(m, t);
        }

        svg_xformSetRotation(t, args[0] / 180.0f * maths::pi);
        svg_xformMultiply(m, t);

        if (na > 1) {
            svg_xformSetTranslation(t, args[1], args[2]);
            svg_xformMultiply(m, t);
        }

        xform.reset(m[0], m[1], m[2], m[3], m[4], m[5]);

        return  s;
    }
    */

    static BLMatrix2D toTransform(const DataChunk& inChunk)
    {

        DataChunk s = inChunk;
        BLMatrix2D tm{};

        //while (s)
        //{
            // Set out temp transform to the identity to start
            // so that if parsing goes wrong, we can still do
            // the multiply without worrying about messing things up
            // That means, the individula parsing functions need to not
            // partially mess up the transform if they fail.
            //svg_xformIdentity(t);

            tm.reset();

            if (comparen_cstr(s, "matrix", 6) == 0)
            {
                tm = toMatrix(s);
            }
            else if (comparen_cstr(s, "translate", 9) == 0)
            {
                //s = svg_parseTranslate(tm, s);
            }
            else if (comparen_cstr(s, "scale", 5) == 0)
            {
                //s = svg_parseScale(tm, s);
            }
            else if (comparen_cstr(s, "rotate", 6) == 0)
            {
                //s = svg_parseRotate(tm, s);
            }
            else if (comparen_cstr(s, "skewX", 5) == 0)
            {
                //s = svg_parseSkewX(tm, s);
            }
            else if (comparen_cstr(s, "skewY", 5) == 0)
            {
                //s = svg_parseSkewY(tm, s);
            }
            else {
                s++;
                //continue;
            }


        //}
        
        return tm;
    }
}