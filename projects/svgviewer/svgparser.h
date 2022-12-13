#pragma once



#include "bstream.h"


#include "maths.hpp"
#include "coloring.h"
#include "shaper.h"
#include "blend2d.h"
#include "geometry.h"
#include "svg.h"

#include "elements/textscan.h"

#include "svgxform.h"

#include <functional>
#include <charconv>
#include <string>
#include <vector>
#include <map>
#include <stack>


namespace svg
{
    using namespace maths;
    using namespace ndt;
    
    static charset whitespaceChars(",\t\n\f\r\v ");
    static charset digitChars("0123456789");
	static charset numberChars("0123456789.-+eE");
    
    constexpr auto NSVG_XML_TAG = 1;
    constexpr auto NSVG_XML_CONTENT = 2;
    constexpr auto NSVG_XML_MAX_ATTRIBS = 256;


    constexpr auto SVG_MAX_ATTR = 128;
    constexpr auto SVG_MAX_DASHES = 8;


    enum SVGgradientUnits {
        SVG_USER_SPACE = 0,
        SVG_OBJECT_SPACE = 1
    };
    


    enum SVGunits {
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

    typedef struct SVGcoordinate {
        float value;
        int units;
    } SVGcoordinate;

    typedef struct SVGlinearData {
        SVGcoordinate x1, y1, x2, y2;
    } SVGlinearData;

    typedef struct SVGradialData {
        SVGcoordinate cx, cy, r, fx, fy;
    } SVGradialData;

    typedef struct SVGgradientData
    {
        char id[64];
        char ref[64];
        char type;
        union {
            SVGlinearData linear;
            SVGradialData radial;
        };
        char spread;
        char units;
        float xform[6];
        int nstops;
        SVGgradientStop* stops;
        struct SVGgradientData* next;
    } SVGgradientData;

    typedef struct SVGattrib
    {
        char id[64];
        float xform[6];
        vec4b fillColor;
        vec4b strokeColor;
        float opacity;
        float fillOpacity;
        float strokeOpacity;
        char fillGradient[64];
        char strokeGradient[64];
        float strokeWidth;
        float strokeDashOffset;
        float strokeDashArray[SVG_MAX_DASHES];
        int strokeDashCount;
        char strokeLineJoin;
        char strokeLineCap;
        float miterLimit;
        char fillRule;
        float fontSize;
        vec4b stopColor;
        float stopOpacity;
        float stopOffset;
        char hasFill;
        char hasStroke;
        char visible;
    } SVGattrib;





    
    struct SVGParser
    {
		std::stack<SVGattrib> attribStack;
        
        float* pts;
        int npts;
        int cpts;
        SVGpath* plist;
        SVGimage* image;
        SVGgradientData* gradients;
        SVGshape* shapesTail;
        float viewMinx, viewMiny, viewWidth, viewHeight;
        int alignX, alignY, alignType;



        
        
        float fDpi = 96;
        const char * fUnits = "px";
        bool fDefsFlag = false;
        bool fPathFlag = false;
    };
    
    struct SVGDocument_t
    {
        float fDpi;         // dots per inch for document
        rectf fFrame;
		rectf   fViewBox;
    };


    typedef std::function<void(SVGParser& p, const DataChunk& elName, std::map<DataChunk, DataChunk>& attrs)> StartElementCallback;
    typedef std::function<void(SVGParser& p, const DataChunk& elName)> EndElementCallback;
    typedef std::function<void(SVGParser& p, DataCursor& dc)> ContentCallback;

    
    static INLINE void skipWhitespace(DataCursor &dc)
    {

		while (!isEOF(dc) && whitespaceChars[peek_u8(dc)])
		{
            skip(dc, 1);
		}
    }




    static SVGattrib & svg_getAttr(SVGParser &p)
    {
		return p.attribStack.top();
    }
    
    static void svg_pushAttr(SVGParser &p)
    {
		p.attribStack.push(p.attribStack.top());
    }
    
    static void svg_popAttr(SVGParser &p)
    {
		p.attribStack.pop();
    }
    
    static float svg_actualOrigX(SVGParser& p)
    {
        return p.viewMinx;
    }

    static float svg_actualOrigY(SVGParser& p)
    {
        return p.viewMiny;
    }

    static float svg_actualWidth(SVGParser& p)
    {
        return p.viewWidth;
    }

    static float svg_actualHeight(SVGParser& p)
    {
        return p.viewHeight;
    }

    static float svg_actualLength(SVGParser& p)
    {
        float w = svg_actualWidth(p), h = svg_actualHeight(p);
        return sqrtf(w * w + h * h) / sqrtf(2.0f);
    }
    

    static float svg_convertToPixels(SVGParser &p, SVGcoordinate &c, float orig, float length)
    {
        const SVGattrib & attr = svg_getAttr(p);
        switch (c.units) {
        case SVG_UNITS_USER:		return c.value;
        case SVG_UNITS_PX:			return c.value;
        case SVG_UNITS_PT:			return c.value / 72.0f * p.fDpi;
        case SVG_UNITS_PC:			return c.value / 6.0f * p.fDpi;
        case SVG_UNITS_MM:			return c.value / 25.4f * p.fDpi;
        case SVG_UNITS_CM:			return c.value / 2.54f * p.fDpi;
        case SVG_UNITS_IN:			return c.value * p.fDpi;
        case SVG_UNITS_EM:			return c.value * attr.fontSize;
        case SVG_UNITS_EX:			return c.value * attr.fontSize * 0.52f; // x-height of Helvetica.
        case SVG_UNITS_PERCENT:	return orig + c.value / 100.0f * length;
        default:					return c.value;
        }
        return c.value;
    }
    

    //==========================================================
    // Parser utilities
    // The primary motivation of these routines is to use
    // chunks, as much as possible.  we don't want to allocate
    // any memory, and we don't want to use any system routines
    //==========================================================

    // Convert a string to an unsigned 64-bit value
    // this is only the digits part, not the sign or exponent
    // if you want to handle those, use this only to parse the 
    // digit parts
   	static INLINE uint64_t svg_strtoull(DataCursor& dc)
    {
        uint64_t v = 0;

        while (!isEOF(dc) && digitChars(*dc))
        {
            v = v * 10 + (uint64_t)(*dc - '0');
            dc++;
        }
        return v;
    }
    
	// parse to integer, including sign
	static INLINE int64_t svg_strtoll(DataCursor& dc)
	{
        uint64_t v = 0;
        int sign = 1;
        
        // Parse optional sign
        if (*dc == '+') {
            dc++;
        }
        else if (*dc == '-') {
            sign = -1;
            dc++;
        }
        
		return sign * svg_strtoull(dc);
	}
    
    // parse floating point number
	// includes sign, exponent, and decimal point
    static double svg_strtof(const DataChunk &chunk, int *len = nullptr)
    {
		DataCursor s = make_cursor_chunk(chunk);
        DataCursor cur = make_cursor_chunk(chunk);
        
        char* end = NULL;
		double sign = 1.0;

        double res = 0.0;
        long long intPart = 0, fracPart = 0;
        bool hasIntPart = false;
        bool hasFracPart = false;

        // Parse optional sign
        if (*cur == '+') {
            cur++;
        }
        else if (*cur == '-') {
            sign = -1;
            cur++;
        }
        // BUGBUG - assuming NOT EOF
       
        // Parse integer part
        if (digitChars[*cur]) {
            // Parse digit sequence
            auto sentinel = tell(cur);
            intPart = svg_strtoull(cur);
            
            if (tell(cur) != sentinel) {
                res = (double)intPart;
                hasIntPart = true;
            }
        }

        // Parse fractional part.
        if (*cur == '.') {
            cur++; // Skip '.'
            if (digitChars(*cur)) {
                // Parse digit sequence
                auto sentinel = tell(cur);
                fracPart = svg_strtoull(cur);
                if (tell(cur) != sentinel) {
                    res += (double)fracPart / pow(10.0, (double)(tell(cur) - sentinel));
                    hasFracPart = true;
                }
            }
        }

        // A valid number should have integer or fractional part.
        if (!hasIntPart && !hasFracPart)
            return 0.0;

        
        // Parse optional exponent
        if (*cur == 'e' || *cur == 'E') {
            long long expPart = 0;
            cur++; // skip 'E'
            auto sentinel = tell(cur);
            expPart = svg_strtoll(cur); // Parse digit sequence with sign
            if (tell(cur) != sentinel) {
                res *= pow(10.0, (double)expPart);
            }
        }
        
		if (len) {
			*len = tell(cur) - tell(s);
		}
        
        return res * sign;
    }
    
    // Parse a number which may have units after it
    // By the end of this routine, we want to return a data chunk that
    // represents the units, and put the characters for the number inside the numChunk
    // a better way would be to use a cursor for the numeric part
    static DataChunk svg_parseNumber(const DataChunk &inChunk, DataChunk & numchunk)
    {
        DataCursor s = make_cursor_chunk(inChunk);
        DataCursor numdc = make_cursor_chunk(numchunk);
        
		//numchunk.fStart = s.fStart;
        
        // sign
        if (*s == '-' || *s == '+') {
            //if (remaining(numdc) > 0)
                put_u8(numdc, *s);
			s++;
        }
        
        // integer part
        while (!isEOF(s) && digitChars[*s]) {
            //if (remaining(numdc) >0) 
                put_u8(numdc, *s);
            s++;
        }
        
        if (*s == '.') {
            // decimal point
            //if (remaining(numdc) > 0)
                put_u8(numdc, *s);
            s++;
            
            // fraction part
            while (!isEOF(s) && digitChars[*s]) {
                //if (remaining(numdc) > 0)
                    put_u8(numdc, *s);
                s++;
            }
        }
        
        // exponent
        if ((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x')) {
            //if (remaining(numdc) > 0)
                put_u8(numdc, *s);
            s++;
            if (*s == '-' || *s == '+') {
                //if (remaining(numdc) > 0)
                    put_u8(numdc, *s);
                s++;
            }
            while (!isEOF(s) && digitChars[*s]) {
                //if (remaining(numdc) > 0)
                    put_u8(numdc, *s);
                s++;
            }
        }

        return make_chunk_size(tell_pointer(s), remaining(s));
    }

    
    //=====================================================
    // Parsing attributes
    //=====================================================
    
    static int svg_parseUnits(const DataChunk &units)
    {
        
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
    
    static bool svg_isCoordinate(DataCursor &s)
    {
        // optional sign
        if (*s == '-' || *s == '+')
            s++;
        // must have at least one digit, or start by a dot
        return (digitChars[*s] || *s == '.');
    }
    
    static SVGcoordinate svg_parseCoordinateRaw(const DataChunk & strChunk)
    {
        SVGcoordinate coord{ 0, SVG_UNITS_USER };
        char buf[64]{};
		DataChunk numChunk = make_chunk_size(buf, 64);
        coord.units = svg_parseUnits(svg_parseNumber(strChunk, numChunk));
        coord.value = (float)svg_strtof(numChunk);
        
        return coord;
    }
    
    static SVGcoordinate svg_coord(float v, int units)
    {
        return { v, units };
    }

    static float svg_parseCoordinate(SVGParser &p, const DataChunk &str, float orig, float length)
    {
        SVGcoordinate coord = svg_parseCoordinateRaw(str);
        return svg_convertToPixels(p, coord, orig, length);
    }
    
    //===========================================
    // Color parsing
    //===========================================
    static vec4b svg_parseColorHex(const DataChunk & chunk)
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

    static vec4b svg_parseColorRGB(const DataChunk & inChunk)
    {
		DataCursor str = make_cursor_chunk(inChunk);
        char strbuff[64];
        
        int i;
        unsigned int rgbi[3];
        float rgbf[3];
        
        // try decimal integers first
        copy_to_cstr(strbuff, 63, inChunk);
        if (sscanf_s(strbuff, "rgb(%u, %u, %u)", &rgbi[0], &rgbi[1], &rgbi[2]) != 3) {
            // integers failed, try percent values (float, locale independent)
            const char delimiter[3] = { ',', ',', ')' };
            skip(str, 4); // skip "rgb("
            for (i = 0; i < 3; i++) {
                while (*str && (whitespaceChars(*str))) 
                    str++; 	// skip leading spaces
                
                if (*str == '+') 
                    str++;				// skip '+' (don't allow '-')
                
                if (isEOF(str)) 
                    break;
                
                int len = 0;
				DataChunk strChunk = make_chunk_size(tell_pointer(str), remaining(str));
                rgbf[i] = svg_strtof(strChunk, &len);


                
                // Note 1: it would be great if nsvg__atof() returned how many
                // bytes it consumed but it doesn't. We need to skip the number,
                // the '%' character, spaces, and the delimiter ',' or ')'.
                skip(str, len);

                if (*str == '%')
                    str++;

                if (*str == delimiter[i]) 
                    str++;
                else 
                    break;
            }

            if (i == 3) {
                rgbi[0] = roundf(rgbf[0] * 2.55f);
                rgbi[1] = roundf(rgbf[1] * 2.55f);
                rgbi[2] = roundf(rgbf[2] * 2.55f);
            }
            else {
                rgbi[0] = rgbi[1] = rgbi[2] = 128;
            }
        }
        // clip values as the CSS spec requires
        for (i = 0; i < 3; i++) {
            if (rgbi[i] > 255) 
                rgbi[i] = 255;
        }
        return rgb(rgbi[0], rgbi[1], rgbi[2]);
    }
    
    static vec4b svg_parseColorName(const DataChunk& inChunk)
    {   
        std::string cName = std::string(inChunk.fStart, inChunk.fEnd);

        
        if (!colors.contains(cName))
            return rgb(0, 0, 0);

        return colors[cName];
    }
    
    static vec4b svg_parseColor(const DataChunk & inChunk)
    {
		DataCursor str = make_cursor_chunk(inChunk);
        
        size_t len = 0;
        while (*str == ' ') 
            str++;
        
        len = size(str);
        if (len >= 1 && *str == '#')
            return svg_parseColorHex(str);
        else if (len >= 4 && str[0] == 'r' && str[1] == 'g' && str[2] == 'b' && str[3] == '(')
            return svg_parseColorRGB(str);

        return svg_parseColorName(str);
    }
    
    static float svg_parseOpacity(const DataChunk &str)
    {
        float val = svg_strtof(str);
        return maths::clamp(val, 0.0f, 1.0f);
    }
    
    static float svg_parseMiterLimit(const DataChunk &str)
    {
        float val = svg_strtof(str);
        if (val < 0.0f) val = 0.0f;
        return val;
    }

    static void svg_parseUrl(char* id, const DataChunk &chunk)
    {
        DataCursor str = make_cursor_chunk(chunk);
		// skp past 'url('
        int i = 0;
		skip(str, 4);

        if (!isEOF(str) && *str == '#')
            str++;
		
        //while (i < 63 && *str && *str != ')')
        while (!isEOF(str) && *str != ')')
        {
            id[i++] = *str;
            str++;
        }
        
        id[i] = '\0';
    }
    
    static char svg_parseLineCap(const DataChunk & str)
    {
        if (str == "butt")
            return SVG_CAP_BUTT;
        else if (str == "round")
            return SVG_CAP_ROUND;
        else if (str == "square")
            return SVG_CAP_SQUARE;
        
        // BUGBUG: handle inherit.
        
        return SVG_CAP_BUTT;
    }

    static char svg_parseLineJoin(const DataChunk & str)
    {
        if (str == "miter")
            return SVG_JOIN_MITER;
        else if (str == "round")
            return SVG_JOIN_ROUND;
        else if (str == "bevel")
            return SVG_JOIN_BEVEL;
        
        // BUGBUG: handle inherit.
        return SVG_JOIN_MITER;
    }
    
    static char svg_parseFillRule(const DataChunk & str)
    {
        if (str == "nonzero")
            return SVG_FILLRULE_NONZERO;
        else if (str == "evenodd")
            return SVG_FILLRULE_EVENODD;
        
        // BUGBUG: handle inherit.
        return SVG_FILLRULE_NONZERO;
    }
    
    static const char* svg_getNextDashItem(const DataChunk &inChunk, DataChunk &it)
    {
        int sz = size(it);
        int n = 0;
        it[0] = '\0';
        
		DataCursor s = make_cursor_chunk(inChunk);
        
        // Skip white spaces and commas
        while (!isEOF(s) && (whitespaceChars(*s) || *s == ',')) 
            s++;
        
        // Advance until whitespace, comma or end.
        while (!isEOF(s) && (!whitespaceChars(*s) && *s != ',')) {
            if (n < sz-1)
                it[n++] = *s;
            s++;
        }
        it[n++] = '\0';
        return (const char *)s.fCurrent;
    }
    
    static int svg_parseStrokeDashArray(SVGParser & p, const DataChunk& inChunk, float* strokeDashArray)
    {
        char item[64];
        int count = 0, i;
        float sum = 0.0f;

		DataChunk itemChunk = make_chunk_size(item, 64);
		DataCursor str = make_cursor_chunk(inChunk);
        
        // Handle "none"
        if (str[0] == 'n')
            return 0;

        // Parse dashes
        while (!isEOF(str)) {
            str.fCurrent = (uint8_t *)svg_getNextDashItem(str, itemChunk);
            if (!*item) 
                break;
            if (count < SVG_MAX_DASHES)
                strokeDashArray[count++] = fabsf(svg_parseCoordinate(p, itemChunk, 0.0f, svg_actualLength(p)));
        }

        for (i = 0; i < count; i++)
            sum += strokeDashArray[i];
        if (sum <= 1e-6f)
            count = 0;

        return count;
    }

    //
    // parsing transforms
    //
    static void nsvg__parseTransform(float* xform, const DataChunk &inChunk)
    {
        float t[6];
        int len;
        svg_xformIdentity(xform);
        
		DataCursor str = make_cursor_chunk(inChunk);
        while (!isEOF(str))
        {
            if (comparen_cstr(str, "matrix", 6) == 0)
                len = svg_parseMatrix(t, str);
            else if (comparen_cstr(str, "translate", 9) == 0)
                len = svg_parseTranslate(t, str);
            else if (comparen_cstr(str, "scale", 5) == 0)
                len = svg_parseScale(t, str);
            else if (comparen_cstr(str, "rotate", 6) == 0)
                len = svg_parseRotate(t, str);
            else if (comparen_cstr(str, "skewX", 5) == 0)
                len = svg_parseSkewX(t, str);
            else if (comparen_cstr(str, "skewY", 5) == 0)
                len = svg_parseSkewY(t, str);
            else {
                str++;
                continue;
            }
            if (len != 0) {
                skip(str, len);
            }
            else {
                str++;
                continue;
            }

            svg_xformPremultiply(xform, t);
        }
    }
    
    
    // Parse specific kinds of attributes
    static void svg_parseStyle(SVGParser& p, const DataChunk& chunk);
    
    static int svg_parseAttr(SVGParser & p, const DataChunk &name, const DataChunk & value)
    {
        float xform[6]{ 0 };

        SVGattrib & attr = svg_getAttr(p);

        if (name == "style") {
            svg_parseStyle(p, value);
        }
        else if (name == "display") {
            if (value == "none")
                attr.visible = 0;
            // Don't reset ->visible on display:inline, one display:none hides the whole subtree

        }
        else if (name == "fill") {
            if (value == "none") {
                attr.hasFill = 0;
            }
            else if (comparen_cstr(value, "url(", 4) == 0) {
                attr.hasFill = 2;
                svg_parseUrl(attr.fillGradient, value);
            }
            else {
                attr.hasFill = 1;
                attr.fillColor = svg_parseColor(value);
            }
        }
        else if (name == "opacity") {
            attr.opacity = svg_parseOpacity(value);
        }
        else if (name == "fill-opacity") {
            attr.fillOpacity = svg_parseOpacity(value);
        }
        else if (name == "stroke") {
            if (value == "none") {
                attr.hasStroke = 0;
            }
            else if (comparen_cstr(value, "url(", 4) == 0) {
                attr.hasStroke = 2;
                svg_parseUrl(attr.strokeGradient, value);
            }
            else {
                attr.hasStroke = 1;
                attr.strokeColor = svg_parseColor(value);
            }
        }
        else if (name == "stroke-width")  {
            attr.strokeWidth = svg_parseCoordinate(p, value, 0.0f, svg_actualLength(p));
        }
        else if (name == "stroke-dasharray")  {
            attr.strokeDashCount = svg_parseStrokeDashArray(p, value, attr.strokeDashArray);
        }
        else if (name == "stroke-dashoffset")  {
            attr.strokeDashOffset = svg_parseCoordinate(p, value, 0.0f, svg_actualLength(p));
        }
        else if (name == "stroke-opacity")  {
            attr.strokeOpacity = svg_parseOpacity(value);
        }
        else if (name == "stroke-linecap")  {
            attr.strokeLineCap = svg_parseLineCap(value);
        }
        else if (name == "stroke-linejoin")  {
            attr.strokeLineJoin = svg_parseLineJoin(value);
        }
        else if (name == "stroke-miterlimit")  {
            attr.miterLimit = svg_parseMiterLimit(value);
        }
        else if (name == "fill-rule")  {
            attr.fillRule = svg_parseFillRule(value);
        }
        else if (name == "font-size")  {
            attr.fontSize = svg_parseCoordinate(p, value, 0.0f, svg_actualLength(p));
        }
        else if (name == "transform")  {
            //nsvg__parseTransform(xform, value);
            svg_xformPremultiply(attr.xform, xform);
        }
        else if (name == "stop-color")  {
            attr.stopColor = svg_parseColor(value);
        }
        else if (name == "stop-opacity")  {
            attr.stopOpacity = svg_parseOpacity(value);
        }
        else if (name == "offset")  {
            attr.stopOffset = svg_parseCoordinate(p, value, 0.0f, 1.0f);
        }
        else if (name == "id")  {
			copy_to_cstr(attr.id, 63, value);
        }
        else {
            return 0;
        }
        
		return 1;
    }
    
    
    //
    // These are name/value pairs related to a style attribute
    //
    static int svg_parseNameValue(SVGParser &p, DataChunk &nameValueChunk)
    {
        char name[512];
        char value[512];
        int n;

		const char* start = (const char *)starting(nameValueChunk);
        const char *str = (const char *)starting(nameValueChunk);
		const char* end = (const char *)ending(nameValueChunk);
        const char* val;
        
        while (str < end && *str != ':') 
            str++;

        // at the beginning of the value
        val = str;

        // Right Trim whitespaces
        while (str > start && (*str == ':' || whitespaceChars(*str))) 
            --str;
        ++str;

        n = (int)(str - start);
        if (n > 511) n = 511;
        if (n) memcpy(name, start, n);
        name[n] = 0;

        while (val < end && (*val == ':' || whitespaceChars(*val))) 
            val++;

        n = (int)(end - val);
        if (n > 511) n = 511;
        if (n) memcpy(value, val, n);
        value[n] = 0;
        DataChunk nameChunk = make_chunk_size(name, strlen(name));
		DataChunk valueChunk = make_chunk_size(value, strlen(value));
		return svg_parseAttr(p, nameChunk, valueChunk);
    }

    
    static void svg_parseStyle(SVGParser &p, const DataChunk & chunk)
    {
        const char* start;
        const char* end;
        DataCursor str = make_cursor_chunk(chunk);
        
        while (!isEOF(str)) {
            // Left Trim
            while (*str && whitespaceChars(*str)) 
                str++;
            
            start = (const char *)tell_pointer(str);
            while (*str && (*str != ';')) 
                str++;
            end = (const char *)tell_pointer(str);

            // Right Trim
            while (end > start && (*end == ';' || whitespaceChars(*end))) 
                --end;
            ++end;
            
            DataChunk nameChunk = make_chunk(start, end);
            svg_parseNameValue(p, nameChunk);
            
            if (!isEOF(str)) 
                str++;
        }
    }

    
    static void svg_parseAttribs(SVGParser &p, std::map<DataChunk, DataChunk>& attr)
    {
        int i;
        
        for (auto& a : attr) {
            const DataChunk& name = a.first;
            DataChunk& value = a.second;
            
            if (name == "style")
                ;//svg_parseStyle();
            else
                svg_parseAttr(p, name, value);
        }
        /*
        for (i = 0; attr[i]; i += 2)
        {
            if (strcmp(attr[i], "style") == 0)
                svg__parseStyle(p, attr[i + 1]);
            else
                svg__parseAttr(p, attr[i], attr[i + 1]);
        }
        */
    }
    
    //=====================================================
    // Parsing specific elements
    //=====================================================
    static void svg_parseLine(SVGParser & p, std::map<std::string, DataChunk>& attrs)
    {
        float x1 = 0.0;
        float y1 = 0.0;
        float x2 = 0.0;
        float y2 = 0.0;
        int i;
        /*
        for (auto &attribute : attr )
		{
			if (attribute.first == "x1") {
				x1 = svg_convertToPixels(p, svg_parseCoordinate(attribute.second), svg_actualOrigX(p), svg_actualWidth(p));
			}
			else if (attribute.first == "y1") {
				y1 = svg_convertToPixels(p, svg_parseCoordinate(attribute.second), svg_actualOrigY(p), svg_actualHeight(p));
			}
			else if (attribute.first == "x2") {
				x2 = svg_convertToPixels(p, svg_parseCoordinate(attribute.second), svg_actualOrigX(p), svg_actualWidth(p));
			}
			else if (attribute.first == "y2") {
				y2 = svg_convertToPixels(p, svg_parseCoordinate(attribute.second), svg_actualOrigY(p), svg_actualHeight(p));
			}
		}
        */
        /*
        for (auto& keyvalue : attrs) 
        {
            if (!nsvg__parseAttr(p, attr[i], attr[i + 1])) 
            {
                if (strcmp(attr[i], "x1") == 0) x1 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
                if (strcmp(attr[i], "y1") == 0) y1 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
                if (strcmp(attr[i], "x2") == 0) x2 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigX(p), nsvg__actualWidth(p));
                if (strcmp(attr[i], "y2") == 0) y2 = nsvg__parseCoordinate(p, attr[i + 1], nsvg__actualOrigY(p), nsvg__actualHeight(p));
            }
        }
        */
        //nsvg__resetPath(p);

        //nsvg__moveTo(p, x1, y1);
       // nsvg__lineTo(p, x2, y2);

        //nsvg__addPath(p, 0);

        //nsvg__addShape(p);
    }

    
    //
    // SVG specific callback routines
    // These are used to actually turn text values
    // into programmatic values
    //
    static void svg_startElement(SVGParser &p, const DataChunk &el, std::map<DataChunk, DataChunk> & attr)
    {
        /*
        if (p.fDefsFlag) {
            // Skip everything but gradients in defs
            if (el == "linearGradient") {
                svg_parseGradient(p, attr, SVG_PAINT_LINEAR_GRADIENT);
            }
            else if (el == "radialGradient") {
                svg_parseGradient(p, attr, SVG_PAINT_RADIAL_GRADIENT);
            }
            else if (el == "stop") {
                svg_parseGradientStop(p, attr);
            }
            return;
        }

        if (el == "g") {
            svg_pushAttr(p);
            svg_parseAttribs(p, attr);
        }
        else if (el == "path") {
            if (p.fPathFlag)	// Do not allow nested paths.
                return;
            svg_pushAttr(p);
            svg_parsePath(p, attr);
            svg_popAttr(p);
        }
        else if (el == "rect") {
            svg_pushAttr(p);
            svg_parseRect(p, attr);
            svg_popAttr(p);
        }
        else if (el == "circle") {
            svg_pushAttr(p);
            svg_parseCircle(p, attr);
            svg_popAttr(p);
        }
        else if (el == "ellipse") {
            svg_pushAttr(p);
            svg_parseEllipse(p, attr);
            svg_popAttr(p);
        }
        else if (el == "line") {
            svg_pushAttr(p);
            svg_parseLine(p, attr);
            svg_popAttr(p);
        }
        else if (el == "polyline") {
            svg_pushAttr(p);
            svg_parsePoly(p, attr, 0);
            svg_popAttr(p);
        }
        else if (el == "polygon") {
            svg_pushAttr(p);
            svg_parsePoly(p, attr, 1);
            svg_popAttr(p);
        }
        else  if (el == "linearGradient") {
            svg_parseGradient(p, attr, SVG_PAINT_LINEAR_GRADIENT);
        }
        else if (el == "radialGradient") {
            svg_parseGradient(p, attr, SVG_PAINT_RADIAL_GRADIENT);
        }
        else if (el == "stop") {
            svg_parseGradientStop(p, attr);
        }
        else if (el == "defs") {
            p.fDefsFlag = true;
        }
        else if (el == "svg") {
            svg_parseSVG(p, attr);
        }
        */
    }

    static void svg_endElement(SVGParser &p, const DataChunk &el)
    {
        
        if (el == "g") {
            svg_popAttr(p);
        }
        else if (el == "path") {
            p.fPathFlag = false;
        }
        else if (el == "defs") {
            p.fDefsFlag = false;
        }
    }
    

    static void svg_content(SVGParser &p, DataCursor &dc)
    {
        // Don't do anything with content
        //NSVG_NOTUSED(ud);
        //NSVG_NOTUSED(s);
        // empty
    }
    


    //===================================================================================
    //
    // Lowest level simple XML parsing engine
    // start at 'parseXML()'
    //
    // BUGBUG : This should be separable.  The only dependence is on the SVGParser
    //===================================================================================
    static void xml_parseContent(SVGParser& p, DataChunk &content, ContentCallback contentCb)
    {
		DataCursor dc = make_cursor_chunk(content);
        
        // Trim front white space
        skipWhitespace(dc);
        
        if (isEOF(dc)) 
            return;
   
        
        if (contentCb != nullptr)
            contentCb(p,dc);
    }
    
    static void xml_parseElement(SVGParser& p, DataChunk &chunk, StartElementCallback startelCb,EndElementCallback endelCb)
    {
		DataCursor dc = make_cursor_chunk(chunk);
        
        // storage for raw attribute key/value pairs
		std::map<DataChunk, DataChunk> attr{};
        
        int nattr = 0;
        uint8_t* beginName{};
        uint8_t* endName{};
        int start = 0;
        int end = 0;
        uint8_t quote{};

        // Skip any white space after the '<'
		skipWhitespace(dc);
        
		if (isEOF(dc))
			return;
        
        // Check if the tag is end tag
        if (peek_u8(dc) == '/') 
        {
		    // End tag
			skip(dc, 1);
            end = 1;
        }
        else {
            start = 1;
        }

        // Skip comments, data and preprocessor stuff.
        if (isEOF(dc) || peek_u8(dc) == '?' || peek_u8(dc) == '!')
            return;
        

        // Get tag name
        beginName = tell_pointer(dc);

        while (!isEOF(dc) && !whitespaceChars[peek_u8(dc)])
            skip(dc, 1);

        

        if (!isEOF(dc)) 
        { 
			endName = tell_pointer(dc);
			skip(dc, 1);
        }
        else {
            endName = tell_pointer(dc);
        }
        auto name = make_chunk(beginName, endName);

        
        // Get the attribute key/value pairs for the element
        while (!end && !isEOF(dc) && (nattr < NSVG_XML_MAX_ATTRIBS - 3)) 
        {
            
            uint8_t * beginAttrName = NULL;
			uint8_t * endAttrName = NULL;
            uint8_t* beginattrValue = NULL;
			uint8_t* endattrValue = NULL;


            // Skip white space before the attrib name
			skipWhitespace(dc);

			if (isEOF(dc))
				break;

            
            if (peek_u8(dc) == '/') {
                end = 1;
                break;
            }

            beginAttrName = tell_pointer(dc);
            
            // Find end of the attrib name.
			skipWhitespace(dc);
            while (!isEOF(dc) && !whitespaceChars[peek_u8(dc)] && peek_u8(dc) != '=')
                skip(dc, 1);
            
            if (!isEOF(dc)) 
            {
				endAttrName = tell_pointer(dc);
                skip(dc, 1);
            }
			else {
				endAttrName = tell_pointer(dc);
			}
            DataChunk attrName = make_chunk(beginAttrName, endAttrName);

            // Skip until the beginning of the value.
            while (!isEOF(dc) && (peek_u8(dc) != '\"') && (peek_u8(dc) != '\''))
                skip(dc, 1);

            
            if (isEOF(dc)) 
                break;
            
            
            quote = get_u8(dc);
            
            // Store value and find the end of it.
			beginattrValue = tell_pointer(dc);

			// Skip until end of the value.
            while (!isEOF(dc) && peek_u8(dc) != quote) 
                skip(dc,1);
            
            if (!isEOF(dc))
            { 
				endattrValue = tell_pointer(dc);
				skip(dc, 1);
            }

            // Store only well formed attributes

			DataChunk attrValue = make_chunk(beginattrValue, endattrValue);
			attr.insert(std::make_pair(attrName, attrValue));

			//printf("    %-20s%s\n", attrName.c_str(), value.c_str());
            //printf("    %-20s\n", attrName.c_str());

			nattr++;
        }

        // Call callbacks.
        if (start && startelCb)
            startelCb(p, name, attr);
        if (end && endelCb)
            endelCb(p, name);

    }
    
	static void parseXML(SVGParser& p, DataChunk& chunk, SVGDocument_t& doc, StartElementCallback startelCb=nullptr, EndElementCallback endelCb=nullptr, ContentCallback contentCb=nullptr)
	{
        auto dc = make_cursor_chunk(chunk);
        
		auto beginMark = tell_pointer(dc);
        uint8_t* endMark = beginMark;
        
		int state = NSVG_XML_CONTENT;   // Starting state
        
		while (!isEOF(dc) )
        {
            // We're parsing up to this point
            // So, the beginMark has already been set
            // and we're looking for the endMark
			// we do NOT want to consume the '<' or '>'
			if (peek_u8(dc) == '<' && (state == NSVG_XML_CONTENT)) 
            {
				endMark = tell_pointer(dc);
                auto achunk = make_chunk(beginMark, endMark);
                
				xml_parseContent(p, achunk, contentCb);
                
				// We now skip over the '<' and set the state to being in a tag
				skip(dc, 1);
				beginMark = tell_pointer(dc);   // mark beginning of next thing
				state = NSVG_XML_TAG;
			}
			else if (peek_u8(dc) == '>' && (state == NSVG_XML_TAG)) 
            {
				// Start of a content or new tag.
				endMark = tell_pointer(dc);
				auto achunk = make_chunk(beginMark, endMark);

				xml_parseElement(p, achunk, startelCb, endelCb);
                skip(dc, 1);
                beginMark = tell_pointer(dc);
				state = NSVG_XML_CONTENT;
			}
			else {
				skip(dc,1);
			}
		}
	}
 
    
    //
    // parseSVGDocument
    // Parses the chunk of memory into an internal representation of the SVG Document.  This 
    // routine is not tied to the graphics system, another step must be taken to convert
    // from this internal form to the specific graphics system.
    // 
    // It's done this way, just like with SVG Path parsing, so the components can
    // be more compact, and separable.  It may be that another goal would be to 
    // print out the graphics, or share across the network, or tie to some other
    // graphics representation. 
    //
    static bool parseSVGDocument(DataChunk &chunk, SVGDocument_t& doc, const char *units = "px", float dpi=96)
    {

		SVGParser p{};
        p.fDpi = dpi;
        
		parseXML(p, chunk, doc, svg_startElement, svg_endElement, svg_content);
        
		return true;
    }
}