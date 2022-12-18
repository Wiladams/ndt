#pragma once


#include "maths.hpp"
#include "coloring.h"
#include "shaper.h"
#include "geometry.h"
#include "bstream.h"
#include "blend2d.h"

#include "svgxform.h"

#include <functional>
#include <charconv>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <stack>

namespace ndt {
    void writeChunk(const DataChunk& chunk)
    {
        DataCursor cur = make_cursor_chunk(chunk);

        while (!isEOF(cur))
            printf("%c", get_u8(cur));
    }

    void printChunk(const DataChunk& chunk)
    {
        if (size(chunk) > 0)
        {
            writeChunk(chunk);
            printf("\n");
        }
        else
            printf("BLANK==CHUNK\n");

    }
}

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
}

namespace svg
{
    using namespace maths;
    using namespace ndt;

    constexpr auto  SVG_KAPPA90 = (0.5522847493f);	// Length proportional to radius of a cubic bezier handle for 90deg arcs.
    constexpr auto  SVG_EPSILON = (1e-12);
    
    enum SVGAlignType {
        SVG_ALIGN_NONE = 0,
        SVG_ALIGN_MIN = 0,
        SVG_ALIGN_MID = 1,
        SVG_ALIGN_MAX = 2,
        SVG_ALIGN_MEET = 1,
        SVG_ALIGN_SLICE = 2,
    };

    // Various Attributes to be found in an SVG image


    enum SVGspreadType {
        SVG_SPREAD_PAD = 0,
        SVG_SPREAD_REFLECT = 1,
        SVG_SPREAD_REPEAT = 2
    };


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


    enum SVGflags {
        SVG_FLAGS_VISIBLE = 0x01
    };
}

namespace svg
{
    using namespace maths;


    // Database of SVG colors
    // BUGBUG - it might be better if these used float instead of byte values
    // Then they can be converted to various forms as needed
    // https://www.w3.org/TR/SVG11/types.html#ColorKeywords
    std::map<std::string, maths::vec4b> colors =
    {
        {"white",  rgb(255, 255, 255)},
        {"ivory", rgb(255, 255, 240)},
        {"lightyellow", rgb(255, 255, 224)},
        {"mintcream", rgb(245, 255, 250)},
        {"azure", rgb(240, 255, 255)},
        {"snow", rgb(255, 250, 250)},
        {"honeydew", rgb(240, 255, 240)},
        {"floralwhite", rgb(255, 250, 240)},
        {"ghostwhite", rgb(248, 248, 255)},
        {"lightcyan", rgb(224, 255, 255)},
        {"lemonchiffon", rgb(255, 250, 205)},
        {"cornsilk", rgb(255, 248, 220)},
        {"lightgoldenrodyellow", rgb(250, 250, 210)},
        {"aliceblue", rgb(240, 248, 255)},
        {"seashell", rgb(255, 245, 238)},
        {"oldlace", rgb(253, 245, 230)},
        {"whitesmoke", rgb(245, 245, 245)},
        {"lavenderblush", rgb(255, 240, 245)},
        {"beige", rgb(245, 245, 220)},
        {"linen", rgb(250, 240, 230)},
        {"papayawhip", rgb(255, 239, 213)},
        {"blanchedalmond", rgb(255, 235, 205)},
        {"antiquewhite", rgb(250, 235, 215)},
        {"yellow", rgb(255, 255, 0)},
        {"mistyrose", rgb(255, 228, 225)},
        {"lavender", rgb(230, 230, 250)},
        {"bisque", rgb(255, 228, 196)},
        {"moccasin", rgb(255, 228, 181)},
        {"palegoldenrod", rgb(238, 232, 170)},
        {"khaki", rgb(240, 230, 140)},
        {"navajowhite", rgb(255, 222, 173)},
        {"aquamarine", rgb(127, 255, 212)},
        {"paleturquoise", rgb(175, 238, 238)},
        {"wheat", rgb(245, 222, 179)},
        {"peachpuff", rgb(255, 218, 185)},
        {"palegreen", rgb(152, 251, 152)},
        {"greenyellow", rgb(173, 255, 47)},
        {"gainsboro", rgb(220, 220, 220)},
        {"powderblue", rgb(176, 224, 230)},
        {"lightgreen", rgb(144, 238, 144)},
        {"lightgray", rgb(211, 211, 211)},
        {"chartreuse", rgb(127, 255, 0)},
        {"gold", rgb(255, 215, 0)},
        {"lightblue", rgb(173, 216, 230)},
        {"lawngreen", rgb(124, 252, 0)},
        {"pink", rgb(255, 192, 203)},
        {"aqua", rgb(0, 255, 255)},
        {"cyan", rgb(0, 255, 255)},
        {"lightpink", rgb(255, 182, 193)},
        {"thistle", rgb(216, 191, 216)},
        {"lightskyblue", rgb(135, 206, 250)},
        {"lightsteelblue", rgb(176, 196, 222)},
        {"skyblue", rgb(135, 206, 235)},
        {"silver", rgb(192, 192, 192)},
        {"springgreen", rgb(0, 255, 127)},
        {"mediumspringgreen", rgb(0, 250, 154)},
        {"turquoise", rgb(64, 224, 208)},
        {"burlywood", rgb(222, 184, 135)},
        {"tan", rgb(210, 180, 140)},
        {"yellowgreen", rgb(154, 205, 50)},
        {"lime", rgb(0, 255, 0)},
        {"mediumaquamarine", rgb(102, 205, 170)},
        {"mediumturquoise", rgb(72, 209, 204)},
        {"darkkhaki", rgb(189, 183, 107)},
        {"lightsalmon", rgb(255, 160, 122)},
        {"plum", rgb(221, 160, 221)},
        {"sandybrown", rgb(244, 164, 96)},
        {"darkseagreen", rgb(143, 188, 143)},
        {"orange", rgb(255, 165, 0)},
        {"darkgray", rgb(169, 169, 169)},
        {"goldenrod", rgb(218, 165, 32)},
        {"darksalmon", rgb(233, 150, 122)},
        {"darkturquoise", rgb(0, 206, 209)},
        {"limegreen", rgb(50, 205, 50)},
        {"violet", rgb(238, 130, 238)},
        {"deepskyblue", rgb(0, 191, 255)},
        {"darkorange", rgb(255, 140, 0)},
        {"salmon", rgb(250, 128, 114)},
        {"rosybrown", rgb(188, 143, 143)},
        {"lightcoral", rgb(240, 128, 128)},
        {"coral", rgb(255, 127, 80)},
        {"mediumseagreen", rgb(60, 179, 113)},
        {"lightseagreen", rgb(32, 178, 170)},
        {"cornflowerblue", rgb(100, 149, 237)},
        {"cadetblue", rgb(95, 158, 160)},
        {"peru", rgb(205, 133, 63)},
        {"hotpink", rgb(255, 105, 180)},
        {"orchid", rgb(218, 112, 214)},
        {"palevioletred", rgb(219, 112, 147)},
        {"darkgoldenrod", rgb(184, 134, 11)},
        {"lightslategray", rgb(119, 136, 153)},
        {"tomato", rgb(255, 99, 71)},
        {"gray", rgb(128, 128, 128)},
        {"dodgerblue", rgb(30, 144, 255)},
        {"mediumpurple", rgb(147, 112, 219)},
        {"olivedrab", rgb(107, 142, 35)},
        {"slategray", rgb(112, 128, 144)},
        {"chocolate", rgb(210, 105, 30)},
        {"steelblue", rgb(70, 130, 180)},
        {"olive", rgb(128, 128, 0)},
        {"mediumslateblue", rgb(123, 104, 238)},
        {"indianred", rgb(205, 92, 92)},
        {"mediumorchid", rgb(186, 85, 211)},
        {"seagreen", rgb(46, 139, 87)},
        {"darkcyan", rgb(0, 139, 139)},
        {"forestgreen", rgb(34, 139, 34)},
        {"royalblue", rgb(65, 105, 225)},
        {"dimgray", rgb(105, 105, 105)},
        {"orangered", rgb(255, 69, 0)},
        {"slateblue", rgb(106, 90, 205)},
        {"teal", rgb(0, 128, 128)},
        {"darkolivegreen", rgb(85, 107, 47)},
        {"sienna", rgb(160, 82, 45)},
        {"green", rgb(0, 128, 0)},
        {"darkorchid", rgb(153, 50, 204)},
        {"saddlebrown", rgb(139, 69, 19)},
        {"deeppink", rgb(255, 20, 147)},
        {"blueviolet", rgb(138, 43, 226)},
        {"magenta", rgb(255, 0, 255)},
        {"fuchsia", rgb(255, 0, 255)},
        {"darkslategray", rgb(47, 79, 79)},
        {"darkgreen", rgb(0, 100, 0)},
        {"darkslateblue", rgb(72, 61, 139)},
        {"brown", rgb(165, 42, 42)},
        {"mediumvioletred", rgb(199, 21, 133)},
        {"crimson", rgb(220, 20, 60)},
        {"firebrick", rgb(178, 34, 34)},
        {"red", rgb(255, 0, 0)},
        {"darkviolet", rgb(148, 0, 211)},
        {"darkmagenta", rgb(139, 0, 139)},
        {"purple", rgb(128, 0, 128)},
        {"midnightblue", rgb(25, 25, 112)},
        {"darkred", rgb(139, 0, 0)},
        {"maroon", rgb(128, 0, 0)},
        {"indigo", rgb(75, 0, 130)},
        {"blue", rgb(0, 0, 255)},
        {"mediumblue", rgb(0, 0, 205)},
        {"darkblue", rgb(0, 0, 139)},
        {"navy", rgb(0, 0, 128)},
        {"black", rgb(0, 0, 0)},
    };

}

namespace svg
{
    using namespace maths;

    typedef struct SVGgradientStop {
        vec4b color;
        float offset;
    } NSVGgradientStop;

    typedef struct SVGgradient {
        float xform[6];
        char spread;
        float fx, fy;
        int nstops;
        // BUGBUG - should be a vector
        SVGgradientStop stops[1];
    } NSVGgradient;

    typedef struct SVGpaint {
        char type;
        union {
            vec4b color;
            SVGgradient* gradient;
        };
    } SVGpaint;



    struct SVGattrib
    {
        char id[64];
        float xform[6];

        BLVar fFill;
        vec4b fFillColor;
        
        BLVar fStroke;
        vec4b fStrokeColor;
        
        float strokeWidth;

        float opacity;
        float fillOpacity;
        float strokeOpacity;
        char fillGradient[64];
        char strokeGradient[64];

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

		void setFillColor(const vec4b& color)
		{
            fFillColor = color;
			blVarAssignRgba32(&fFill, color.value);
            
            hasFill = 1;
		}
        
		void setStrokeColor(const vec4b& color)
		{
            fStrokeColor = color;
			blVarAssignRgba32(&fStroke, color.value);
            
            hasStroke = 1;
		}
    } ;
    
    // Contains geometry and stroke/fill attributes
    struct SVGShape : GraphicElement
    {
        SVGattrib fAttributes;
        BLPath fPath{};
        rectf fFrame;
        
        SVGShape(const rectf &fr)
        {
            fFrame = fr;
        }

		void setAttributes(const SVGattrib& attr)
		{
			fAttributes.setFillColor(attr.fFillColor);
			fAttributes.setStrokeColor(attr.fStrokeColor);


			fAttributes.strokeWidth = attr.strokeWidth;
			fAttributes.opacity = attr.opacity;
			fAttributes.fillOpacity = attr.fillOpacity;
			fAttributes.strokeOpacity = attr.strokeOpacity;
			fAttributes.strokeDashOffset = attr.strokeDashOffset;
			fAttributes.strokeDashCount = attr.strokeDashCount;
			fAttributes.strokeLineJoin = attr.strokeLineJoin;
			fAttributes.strokeLineCap = attr.strokeLineCap;
			fAttributes.miterLimit = attr.miterLimit;
			fAttributes.fillRule = attr.fillRule;
			fAttributes.fontSize = attr.fontSize;
			fAttributes.stopColor = attr.stopColor;
			fAttributes.stopOpacity = attr.stopOpacity;
			fAttributes.stopOffset = attr.stopOffset;
			fAttributes.hasFill = attr.hasFill;
			fAttributes.hasStroke = attr.hasStroke;
			fAttributes.visible = attr.visible;

			memcpy(fAttributes.strokeDashArray, attr.strokeDashArray, sizeof(attr.strokeDashArray));

			memcpy(fAttributes.xform, attr.xform, sizeof(attr.xform));

			memcpy(fAttributes.id, attr.id, sizeof(attr.id));
			memcpy(fAttributes.fillGradient, attr.fillGradient, sizeof(attr.fillGradient));
			memcpy(fAttributes.strokeGradient, attr.strokeGradient, sizeof(attr.strokeGradient));

		}
        
        void setID(const std::string& s)
        {
            //fAttributes.fID = s;
        }

        void setFill(BLVar& fill)
        {
            fAttributes.fFill = fill;
        }

        void setPath(const BLPath& apath)
        {
            fPath = apath;
        }
        
        void setStrokeWidth(float w)
        {
            fAttributes.strokeWidth = w;
        }

        void setStroke(BLVar& stroke) 
        {
            fAttributes.fStroke = stroke;
        }

        void draw(IGraphics& ctx)
        {
            ctx.push();
            
			if (fAttributes.hasFill)
			{
				ctx.fill(fAttributes.fFill);
            }
            else
                ctx.noFill();
            
			if (fAttributes.hasStroke)
			{
				ctx.stroke(fAttributes.fStroke);
				ctx.strokeWeight(fAttributes.strokeWidth);
			}
			else
				ctx.noStroke();

            ctx.path(fPath);
            
            ctx.pop();
        }
    };


}



namespace svg
{


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







    
    struct SVGParser
    {
		std::array<SVGattrib, SVG_MAX_ATTR> attr{};
        int attrHead=0;
        
        std::map<DataChunk, BLVar> fDefinitions{};

        SVGgradientData* gradients= nullptr;
        
        std::vector<std::shared_ptr<SVGShape> > fShapes;
        BLPath fWorkingPath;

        
        rectf viewBox{};
        float viewMinx, viewMiny, viewWidth, viewHeight;
        float imageWidth = 0;
        float imageHeight = 0;
        int alignX, alignY, alignType;

        float fDpi = 96;
        const char * fUnits = "px";
        bool fDefsFlag = false;
        bool fPathFlag = false;



        

        SVGParser()
        {

            // Init style
            svg_xformIdentity(attr[0].xform);
            memset(attr[0].id, 0, sizeof attr[0].id);
            attr[0].setFillColor(rgb(0, 0, 0));
            attr[0].setStrokeColor(rgb(0, 0, 0));
            attr[0].opacity = 1;
            attr[0].fillOpacity = 1;
            attr[0].strokeOpacity = 1;
            attr[0].stopOpacity = 1;
            attr[0].strokeWidth = 1;
            attr[0].strokeLineJoin = SVG_JOIN_MITER;
            attr[0].strokeLineCap = SVG_CAP_BUTT;
            attr[0].miterLimit = 4;
            attr[0].fillRule = SVG_FILLRULE_NONZERO;
            attr[0].hasFill = 1;
            attr[0].visible = 1;

            return ;
        }
        
        
        SVGattrib& getAttr()
        {
            return attr[attrHead];
            
        }

		void pushAttr()
        {
            if (attrHead < SVG_MAX_ATTR - 1) {
                attrHead++;
                memcpy(&attr[attrHead], &attr[attrHead - 1], sizeof(SVGattrib));
            }
            
        }

        void popAttr()
        {
            if (attrHead > 0)
                attrHead--;
        }

        float actualOrigX() {return viewMinx; }
        float actualOrigY(){ return viewMiny;}
        float actualWidth(){return viewWidth;}
        float actualHeight(){return viewHeight;}
        float actualLength()
        {
            float w = actualWidth(), h = actualHeight();
            return sqrtf(w * w + h * h) / sqrtf(2.0f);
        }

        void resetPath()
        {   
            fWorkingPath.reset();
        }

		void addDefinition(DataChunk name, BLVar value)
		{
			fDefinitions[name] = value;
		}
        
        void addShape()
        {
            // If the path is empty, don't do anything
            if (fWorkingPath.empty())
                return;

            // Create a new shape, using the current working path
            // and accumulated attributes
			auto newShape = std::make_shared<SVGShape>(rectf{ viewMinx,viewMiny,viewWidth, viewHeight });

            newShape->setPath(fWorkingPath);
			newShape->setAttributes(getAttr());
            
            fShapes.push_back(newShape);

            resetPath();
        }


        //============================================================
        // Adding geometries to our shapes storage
        //============================================================
        void addLine(float x1, float y1, float x2, float y2)
        {
			fWorkingPath.addLine(BLLine(x1, y1, x2, y2));
            
            addShape();
        }

        void addRect(float x, float y, float w, float h, float rx, float ry)
        {
            if (w != 0.0f && h != 0.0f) 
            {
                if (rx < 0.00001f || ry < 0.0001f) {
                    fWorkingPath.addRect(x, y, w, h);
                }
                else {
					fWorkingPath.addRoundRect(BLRoundRect(x, y, w, h, rx, ry));
                }

                addShape();

            }
        }

		void addCircle(float cx, float cy, float r)
		{
			if (r > 0.0f) {
				fWorkingPath.addCircle(BLCircle(cx, cy, r));

                addShape();
			}
		}
        
        void addEllipse(float cx, float cy, float rx, float ry)
        {
			if (rx > 0.0f && ry > 0.0f) {
				fWorkingPath.addEllipse(BLEllipse(cx, cy, rx, ry));

                addShape();
			}
        }

        void addPoly(BLArray<BLPoint> &pts, bool closeFlag)
        {
			if (closeFlag)
				fWorkingPath.addPolygon(pts.view());
			else
				fWorkingPath.addPolyline(pts.view());

            addShape();
        }

        void addPath(std::vector<ndt::PathSegment>& segments)
        {
            BLPath apath{};
            blPathFromSegments(segments, apath);
			fWorkingPath.addPath(apath);

            addShape();
        }
    };


    typedef std::function<void(SVGParser& p, const DataChunk& elName, std::map<DataChunk, DataChunk>& attrs)> StartElementCallback;
    typedef std::function<void(SVGParser& p, const DataChunk& elName)> EndElementCallback;
    typedef std::function<void(SVGParser& p, DataCursor& dc)> ContentCallback;


    static INLINE float svg_minf(float a, float b) { return a < b ? a : b; }
    static INLINE float svg_maxf(float a, float b) { return a > b ? a : b; }
    
	static INLINE void skipOverCharset(DataCursor& dc, const charset& cs)
	{
		while (dc && cs.contains(*dc))
			++dc;
	}

    static INLINE void skipUntilCharset(DataCursor &dc, const charset &cs)
	{
		while (dc && !cs.contains(*dc))
			++dc;
	}

    
    static INLINE void skipWhitespace(DataCursor &dc)
    {
		skipOverCharset(dc, whitespaceChars);
	}






    



    

    static float svg_convertToPixels(SVGParser &p, SVGcoordinate &c, float orig, float length)
    {
        const SVGattrib &attr = p.getAttr();
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
	//   1.2em
    // -1.0E2em
    // 2.34ex
    // -2.34e3M10,20
    // The end of this routine, we want to return a data chunk that
    // represents the units, and put the characters for the number inside the numChunk
    // a better way would be to use a cursor for the numeric part
    static DataChunk svg_parseNumber(const DataChunk &inChunk, DataChunk & numchunk)
    {
        DataChunk s = inChunk;
        DataCursor numdc = make_cursor_chunk(numchunk);
        
        // sign
        if (*s == '-' || *s == '+') {
                put_u8(numdc, *s);
			s++;
        }
        
        // integer part
        while (s && digitChars[*s]) {
            put_u8(numdc, *s);
            s++;
        }
        
        if (*s == '.') {
            // decimal point
                put_u8(numdc, *s);
            s++;
            
            // fraction part
            while (size(s) && digitChars[*s]) {
                    put_u8(numdc, *s);
                s++;
            }
        }
        
        // exponent
        // but it could be units (em, ex)
        if ((*s == 'e' || *s == 'E') && (s[1] != 'm' && s[1] != 'x')) {
                put_u8(numdc, *s);
            s++;
            if (*s == '-' || *s == '+') {
                    put_u8(numdc, *s);
                s++;
            }
            while (size(s) && digitChars[*s]) {
                    put_u8(numdc, *s);
                s++;
            }
        }
        
        // BUGBUG - experimental
        // assumes it is OK to alter end of buffer chunk
        // probably is OK because chunk is typically referencing some other storage
        numchunk.fEnd = (const uint8_t *)tell_pointer(numdc);
        
        return s;
    }

    
    //=====================================================
    // Parsing attributes
    //=====================================================
    
    static int svg_parseUnits(const DataChunk &units)
    {
        // if the chunk is blank, then return user units
        if (size(units) < 1)
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
        unsigned int rgbi[3]{};
        float rgbf[3]{};
        
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
        float val = (float)svg_strtof(str);
        return maths::clamp(val, 0.0f, 1.0f);
    }
    
    static float svg_parseMiterLimit(const DataChunk &str)
    {
        float val = (float)svg_strtof(str);
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
    
    static DataChunk svg_getNextDashItem(const DataChunk &inChunk, DataChunk &it)
    {
        int sz = size(it);
        int n = 0;
        it[0] = '\0';
        
		DataChunk s = inChunk;
        
        // Skip white spaces and commas
        while (s && (whitespaceChars(*s) || *s == ',')) 
            s++;
        
        // Advance until next whitespace, comma, or end.
        while (s && (!whitespaceChars(*s) && *s != ',')) 
        {
            if (n < sz-1)
                it[n++] = *s;
            s++;
        }
        
        it[n++] = '\0';
        
        return s;
    }
    
    static int svg_parseStrokeDashArray(SVGParser & p, const DataChunk& inChunk, float* strokeDashArray)
    {
        char item[64];
        int count = 0, i;
        float sum = 0.0f;

		DataChunk itemChunk = make_chunk_size(item, 64);
        DataChunk s = inChunk;
        
        // Handle "none"
        if (*s == 'n')
            return 0;

        // Parse dashes
        while (s) 
        {
            s = svg_getNextDashItem(s, itemChunk);
            if (!*item) 
                break;
            if (count < SVG_MAX_DASHES)
                strokeDashArray[count++] = fabsf(svg_parseCoordinate(p, itemChunk, 0.0f, p.actualLength()));
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
    static DataChunk svg_parseTransformArgs(const DataChunk & inChunk, float* args, int maxNa, int& na)
    {
        na = 0;

        DataChunk s = inChunk;
        
        // Skip past everything until we see a '('
        while (s && *s != '(') 
            s++;
        
        // If we got to the end of the chunk, and did not see the '('
        // then just return
        if (!s)
            return s;
        
        // by the time we're here, we're sitting right on top of the 
        //'(', so skip past that to get to what should be the numbers
        s++;
        
        // We want to createa  chunk that contains all the numbers
        // without the closing ')', so create a chunk that just
        // represents that range.
        // Start the chunk at the current position
        // and expand it after we find the ')'
        DataChunk item = s;
        item.fEnd = item.fStart;
        
        // scan until the closing ')'
        while (s && (*s != ')')) 
            s++;
        
        // At this point, we're either sitting at the ')' or at the end
		// of the chunk.  If we're at the end of the chunk, then we
		// didn't find the closing ')', so just return
		if (*s != ')')
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
        while (item) {
            if (*item == '-' || *item == '+' || *item == '.' || digitChars(*item)) 
            {
                if (na >= maxNa) 
                    return s;
                
                item = scanNumber(item, numChunk);
                args[(na)++] = (float)svg_strtof(numChunk);
            }
            else {
                // skip past what should be whitspace, or commas
                item++;
            }
        }
        
        return s;
    }
    
    static DataChunk svg_parseMatrix(BLMatrix2D& xform, const DataChunk& inChunk)
    {
        float t[6];
        int na = 0;
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, t, 6, na);
        
        if (na != 6) 
            return s;
        
		xform.reset(t[0], t[1], t[2], t[3], t[4], t[5]);
        
        return s;
    }
    
    static DataChunk svg_parseTranslate(BLMatrix2D& xform, const DataChunk & inChunk)
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
    
    static DataChunk svg_parseScale(BLMatrix2D& xform, const DataChunk & inChunk)
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
    
    static DataChunk svg_parseSkewX(BLMatrix2D& xform, const DataChunk & inChunk)
    {
        float args[1];
        int na = 0;
        float t[6];
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, args, 1, na);

		xform.resetToSkewing(args[0], 0.0f);
        
        return s;
    }

    static DataChunk svg_parseSkewY(BLMatrix2D& xform, const DataChunk & inChunk)
    {
        float args[1]{ 0};
        int na = 0;
        float t[6]{ 0 };
        DataChunk s = inChunk;
        s = svg_parseTransformArgs(s, args, 1, na);

		xform.resetToSkewing(0.0f, args[0]);
        
        return s;
    }
    
    static DataChunk svg_parseRotate(BLMatrix2D &xform, const DataChunk & inChunk)
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

    
    static void svg_parseTransform(BLMatrix2D &xform, const DataChunk &inChunk)
    {
        float t[6];
        BLMatrix2D m{};
        m.reset();
        
        
		DataChunk s = inChunk;
        
        while (s)
        {

            
			// Set out temp transform to the identity to start
            // so that if parsing goes wrong, we can still do
            // the multiply without worrying about messing things up
			// That means, the individula parsing functions need to not
			// partially mess up the transform if they fail.
            //svg_xformIdentity(t);
            BLMatrix2D tm{};
            tm.reset();
            
            if (comparen_cstr(s, "matrix", 6) == 0)
                s = svg_parseMatrix(t, s);
            else if (comparen_cstr(s, "translate", 9) == 0)
                s = svg_parseTranslate(t, s);
            else if (comparen_cstr(s, "scale", 5) == 0)
                s = svg_parseScale(t, s);
            else if (comparen_cstr(s, "rotate", 6) == 0)
                s = svg_parseRotate(t, s);
            else if (comparen_cstr(s, "skewX", 5) == 0)
                s = svg_parseSkewX(t, s);
            else if (comparen_cstr(s, "skewY", 5) == 0)
                s = svg_parseSkewY(t, s);
            else {
                s++;
                continue;
            }
            
            tm.reset(t[0], t[1], t[2], t[3], t[4], t[5]);
            m.transform(tm);

        }
    }
    
    
    // Parse specific kinds of attributes
    // As the attribute is parsed, the value is stored in the named
	// attribute of the parser.
    static DataChunk svg_parseStyle(SVGParser& p, const DataChunk& chunk);
    
    static int svg_parseAttr(SVGParser & p, const DataChunk &name, const DataChunk & value)
    {
        BLMatrix2D xform{};


        SVGattrib & attr = p.getAttr();

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
                attr.setFillColor(svg_parseColor(value));
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
                attr.setStrokeColor(svg_parseColor(value));
            }
        }
        else if (name == "stroke-width")  {
            attr.strokeWidth = svg_parseCoordinate(p, value, 0.0f, p.actualLength());
        }
        else if (name == "stroke-dasharray")  {
            attr.strokeDashCount = svg_parseStrokeDashArray(p, value, attr.strokeDashArray);
        }
        else if (name == "stroke-dashoffset")  {
            attr.strokeDashOffset = svg_parseCoordinate(p, value, 0.0f, p.actualLength());
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
            attr.fontSize = svg_parseCoordinate(p, value, 0.0f, p.actualLength());
        }
        else if (name == "transform")  {
            svg_parseTransform(xform, value);
            //svg_xformPremultiply(attr.xform, xform);
            attr.xform.transform(xform);
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

		const char* start = (const char *)begin(nameValueChunk);
        const char *str = (const char *)begin(nameValueChunk);
		const char *ending = (const char *)ndt::end(nameValueChunk);
        const char* val;
        
        while (str < ending && *str != ':') 
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

        while (val < ending && (*val == ':' || whitespaceChars(*val))) 
            val++;

        n = (int)(ending - val);
        if (n > 511) n = 511;
        if (n) memcpy(value, val, n);
        value[n] = 0;
        DataChunk nameChunk = make_chunk_size(name, strlen(name));
		DataChunk valueChunk = make_chunk_size(value, strlen(value));
		return svg_parseAttr(p, nameChunk, valueChunk);
    }

    //
	// Parse a style attribute
	//
    static DataChunk svg_parseStyle(SVGParser &p, const DataChunk & chunk)
    {
		DataChunk s = chunk;
		DataChunk nameValueChunk;
        
        while (s) 
        {
			chunk_ltrim(s, whitespaceChars);
            
            nameValueChunk = s;
            nameValueChunk.fEnd = s.fStart;
            
			// semicolon ';' separates individual name/value pairs
            // so find the next one
            while (s && *s && (*s != ';'))
            {
                s++;
            }
            
            nameValueChunk.fEnd = (const uint8_t *)s.fStart;

			if (*s == ';')
			{
				s++;
			}

            // Right Trim
            chunk_rtrim(nameValueChunk, whitespaceChars);

            // Skip this if the pair was empty
            if (!nameValueChunk)
                continue;
            
            // Parse the value according to what kind it is
            // and store the value in the current attribute
            svg_parseNameValue(p, nameValueChunk);
        }

        return s;
    }

 
    //
	// Parse an element's attributes
	//
    static void svg_parseAttribs(SVGParser &p, std::map<DataChunk, DataChunk>& attr)
    {
        for (auto& a : attr) {
            const DataChunk& name = a.first;
            DataChunk& value = a.second;
            
            if (name == "style")
               svg_parseStyle(p,value);
            else
                svg_parseAttr(p, name, value);
        }
    }

    static SVGgradientData* svg_findGradientData(SVGParser &p, const char* id)
    {
        SVGgradientData* grad = p.gradients;
        if (id == NULL || *id == '\0')
            return NULL;
        while (grad != NULL) {
            if (strcmp(grad->id, id) == 0)
                return grad;
            grad = grad->next;
        }
        return NULL;
    }
    
    static SVGgradient* svg_createGradient(SVGParser & p, const char* id, const float* localBounds, char* paintType)
    {
        SVGattrib & attr = p.getAttr();
        SVGgradientData* data = NULL;
        SVGgradientData* ref = NULL;
        SVGgradientStop* stops = NULL;
        SVGgradient* grad;
        float ox, oy, sw, sh, sl;
        int nstops = 0;
        int refIter;

        data = svg_findGradientData(p, id);
        if (data == NULL) return NULL;

        // TODO: use ref to fill in all unset values too.
        ref = data;
        refIter = 0;
        while (ref != NULL) {
            SVGgradientData* nextRef = NULL;
            if (stops == NULL && ref->stops != NULL) {
                stops = ref->stops;
                nstops = ref->nstops;
                break;
            }
            nextRef = svg_findGradientData(p, ref->ref);
            if (nextRef == ref) break; // prevent infite loops on malformed data
            ref = nextRef;
            refIter++;
            if (refIter > 32) break; // prevent infite loops on malformed data
        }
        if (stops == NULL) return NULL;

		//grad = new SVGgradient();
        grad = (SVGgradient*)malloc(sizeof(SVGgradient) + sizeof(SVGgradientStop) * (nstops - 1));
        if (grad == NULL) return NULL;

        // The shape width and height.
        if (data->units == SVG_OBJECT_SPACE) {
            ox = localBounds[0];
            oy = localBounds[1];
            sw = localBounds[2] - localBounds[0];
            sh = localBounds[3] - localBounds[1];
        }
        else {
            ox = p.actualOrigX();
            oy = p.actualOrigY();
            sw = p.actualWidth();
            sh = p.actualHeight();
        }
        sl = sqrtf(sw * sw + sh * sh) / sqrtf(2.0f);

        if (data->type == SVG_PAINT_LINEAR_GRADIENT) {
            float x1, y1, x2, y2, dx, dy;
            x1 = svg_convertToPixels(p, data->linear.x1, ox, sw);
            y1 = svg_convertToPixels(p, data->linear.y1, oy, sh);
            x2 = svg_convertToPixels(p, data->linear.x2, ox, sw);
            y2 = svg_convertToPixels(p, data->linear.y2, oy, sh);
            // Calculate transform aligned to the line
            dx = x2 - x1;
            dy = y2 - y1;
            grad->xform[0] = dy; grad->xform[1] = -dx;
            grad->xform[2] = dx; grad->xform[3] = dy;
            grad->xform[4] = x1; grad->xform[5] = y1;
        }
        else {
            float cx, cy, fx, fy, r;
            cx = svg_convertToPixels(p, data->radial.cx, ox, sw);
            cy = svg_convertToPixels(p, data->radial.cy, oy, sh);
            fx = svg_convertToPixels(p, data->radial.fx, ox, sw);
            fy = svg_convertToPixels(p, data->radial.fy, oy, sh);
            r = svg_convertToPixels(p, data->radial.r, 0, sl);
            // Calculate transform aligned to the circle
            grad->xform[0] = r; grad->xform[1] = 0;
            grad->xform[2] = 0; grad->xform[3] = r;
            grad->xform[4] = cx; grad->xform[5] = cy;
            grad->fx = fx / r;
            grad->fy = fy / r;
        }

        svg_xformMultiply(grad->xform, data->xform);
        svg_xformMultiply(grad->xform, attr.xform);

        grad->spread = data->spread;
        memcpy(grad->stops, stops, nstops * sizeof(NSVGgradientStop));
        grad->nstops = nstops;

        *paintType = data->type;

        return grad;
    }

    //=====================================================
    // Parsing specific elements
    //=====================================================
    static void svg_parseLine(SVGParser & p, std::map<DataChunk, DataChunk>& attrs)
    {
        float x1 = 0.0;
        float y1 = 0.0;
        float x2 = 0.0;
        float y2 = 0.0;


        for (auto& keyvalue : attrs) 
        {
			if (!svg_parseAttr(p, keyvalue.first, keyvalue.second))

            {
                if (keyvalue.first == "x1") x1 = svg_parseCoordinate(p, keyvalue.second, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "y1") y1 = svg_parseCoordinate(p, keyvalue.second, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "x2") x2 = svg_parseCoordinate(p, keyvalue.second, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "y2") y2 = svg_parseCoordinate(p, keyvalue.second, p.actualOrigY(), p.actualHeight());
            }
        }
        
        p.addLine(x1, y1, x2, y2);

    }
    
    static void svg_parseRect(SVGParser & p, std::map<DataChunk, DataChunk>& attrs)
    {
        float x = 0.0f;
        float y = 0.0f;
        float w = 0.0f;
        float h = 0.0f;
        float rx = -1.0f; // marks not set
        float ry = -1.0f;

        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second)) {
                if (keyvalue.first == "x") x = svg_parseCoordinate(p, keyvalue.second, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "y") y = svg_parseCoordinate(p, keyvalue.second, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "width") w = svg_parseCoordinate(p, keyvalue.second, 0.0f, p.actualWidth());
                if (keyvalue.first == "height") h = svg_parseCoordinate(p, keyvalue.second, 0.0f, p.actualHeight());
                if (keyvalue.first == "rx") rx = fabsf(svg_parseCoordinate(p, keyvalue.second, 0.0f, p.actualWidth()));
                if (keyvalue.first == "ry") ry = fabsf(svg_parseCoordinate(p, keyvalue.second, 0.0f, p.actualHeight()));
            }
        }

        if (rx < 0.0f && ry > 0.0f) rx = ry;
        if (ry < 0.0f && rx > 0.0f) ry = rx;
        if (rx < 0.0f) rx = 0.0f;
        if (ry < 0.0f) ry = 0.0f;
        if (rx > w / 2.0f) rx = w / 2.0f;
        if (ry > h / 2.0f) ry = h / 2.0f;


		p.addRect(x, y, w, h, rx, ry);
    }
    
    static void svg_parseCircle(SVGParser &p, std::map<DataChunk, DataChunk>& attrs)
    {
        float cx = 0.0f;
        float cy = 0.0f;
        float r = 0.0f;


        // parse attributes of a circle
        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second)) 
            {
                if (keyvalue.first == "cx") cx = svg_parseCoordinate(p, keyvalue.second, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "cy") cy = svg_parseCoordinate(p, keyvalue.second, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "r") r = fabsf(svg_parseCoordinate(p, keyvalue.second, 0.0f, p.actualLength()));
            }
        }

        p.addCircle(cx, cy, r);

    }

    static void svg_parseEllipse(SVGParser &p, std::map<DataChunk, DataChunk>& attrs)
    {
        float cx = 0.0f;
        float cy = 0.0f;
        float rx = 0.0f;
        float ry = 0.0f;


        // Get the attributes of the ellipse
        for (auto& keyvalue : attrs) 
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second)) 
            {
                if (keyvalue.first == "cx") cx = svg_parseCoordinate(p, keyvalue.second, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "cy") cy = svg_parseCoordinate(p, keyvalue.second, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "rx") rx = fabsf(svg_parseCoordinate(p, keyvalue.second, 0.0f, p.actualWidth()));
                if (keyvalue.first == "ry") ry = fabsf(svg_parseCoordinate(p, keyvalue.second, 0.0f,p.actualHeight()));
            }
        }

		p.addEllipse(cx, cy, rx, ry);
    }
    
    static DataChunk nextNumber(const DataChunk& inChunk, DataChunk& numChunk)
    {
        DataChunk s = inChunk;

		// Skip leading whitespace
        while (s && (whitespaceChars[*s] || *s == '%'))
            s++;

        if (!s)
            return s;
        
        // We should be at the beginning of some digits
        if (*s == '-' || *s == '+' || *s == '.' || digitChars(*s))
        {
            s = ndt::scanNumber(s, numChunk);
        }
        /*
        numChunk.fStart = s.fStart;
        numChunk.fEnd = s.fStart;

        // Get extent of first number
        while (s && !whitespaceChars[*s])
            s++;

        numChunk.fEnd = s.fStart;
        */
            
        return s;
    }
    
    static DataChunk svg_getNextPathItem(const DataChunk &inChunk, DataChunk &outChunk)
    {
        DataChunk s = inChunk;
        
        // Skip white spaces and commas
        while (s && (whitespaceChars(*s) || *s == ',')) 
            s++;
        
        if (!s) 
            return s;
        
        if (*s == '-' || *s == '+' || *s == '.' || digitChars(*s)) 
        {
            s = ndt::scanNumber(s, outChunk);
        }
        else {
            // Parse command
            outChunk[0] = *s++;
            outChunk[1] = '\0';
            return s;
        }
        
        return s;
    }
    
    static void svg_parsePoly(SVGParser & p, std::map<DataChunk, DataChunk>& attrs, bool closeFlag)
    {
        DataChunk s{};
        float args[2];
        int nargs, npts = 0;
        uint8_t itembuff[64]{ 0 };

		BLArray <BLPoint> points;

        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second)) 
            {
                if (keyvalue.first == "points") 
                {
                    s = keyvalue.second;
                    nargs = 0;
                    while (*s) {
						//memset(itembuff, 0, 64);
                        DataChunk item = make_chunk_size(itembuff, 64); // reset for each run

                        s = svg_getNextPathItem(s, item);
                        args[nargs++] = (float)svg_strtof(item);
                        if (nargs >= 2) {
                            if (npts == 0)
								points.append(BLPoint{ args[0], args[1] });
                            else
                                points.append(BLPoint{ args[0], args[1] });

                            nargs = 0;
                            npts++;
                        }
                    }
                }
            }
        }

		p.addPoly(points, closeFlag);

    }
    
	static void svg_parsePolygon(SVGParser& p, std::map<DataChunk, DataChunk>& attrs)
	{
		svg_parsePoly(p, attrs, true);
	}
    
    static void svg_parsePath(SVGParser& p, std::map<DataChunk, DataChunk>& attrs)
    {
        DataChunk s{};
        std::vector<ndt::PathSegment> segments{};
        
        // Cycle through the attributes
        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second))
            {
                if (keyvalue.first == "d")
                {
                    // parse into segments
                    ndt::tokenizePath(keyvalue.second, segments);
                }
            }
        }
        
        // add the path to the parser
        p.addPath(segments);
        
    }
    



    static void svg_parseSVG(SVGParser &p, std::map<DataChunk, DataChunk>& attrs)
    {

        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second)) {
                if (keyvalue.first == "width") {
                    p.imageWidth = svg_parseCoordinate(p, keyvalue.second, 0.0f, 0.0f);
                }
                else if (keyvalue.first == "height") {
                    p.imageHeight = svg_parseCoordinate(p, keyvalue.second, 0.0f, 0.0f);
                }
                else if (keyvalue.first == "viewBox") {
                    // Assuming we're already on the numeric part
                    DataChunk s = keyvalue.second;
                    DataChunk numChunk{};

                    s = nextNumber(s, numChunk);
                    p.viewMinx = (float)svg_strtof(numChunk);

                    s = nextNumber(s, numChunk);
                    p.viewMiny = (float)svg_strtof(numChunk);

                    s = nextNumber(s, numChunk);
                    p.viewWidth = (float)svg_strtof(numChunk);

                    s = nextNumber(s, numChunk);
                    p.viewHeight = (float)svg_strtof(numChunk);

                }
                else if (keyvalue.first == "preserveAspectRatio") 
                {
                    /*
                    if (strstr(attr[i + 1], "none") != 0) {
                        // No uniform scaling
                        p.alignType = SVG_ALIGN_NONE;
                    }
                    else {
                        // Parse X align
                        if (strstr(attr[i + 1], "xMin") != 0)
                            p.alignX = SVG_ALIGN_MIN;
                        else if (strstr(attr[i + 1], "xMid") != 0)
                            p.alignX = SVG_ALIGN_MID;
                        else if (strstr(attr[i + 1], "xMax") != 0)
                            p.alignX = SVG_ALIGN_MAX;
                        // Parse X align
                        if (strstr(attr[i + 1], "yMin") != 0)
                            p.alignY = SVG_ALIGN_MIN;
                        else if (strstr(attr[i + 1], "yMid") != 0)
                            p.alignY = SVG_ALIGN_MID;
                        else if (strstr(attr[i + 1], "yMax") != 0)
                            p.alignY = SVG_ALIGN_MAX;
                        // Parse meet/slice
                        p.alignType = SVG_ALIGN_MEET;
                        if (strstr(attr[i + 1], "slice") != 0)
                            p.alignType = SVG_ALIGN_SLICE;
                    }
                    */
                }
            }
            
        }
    }
    
    
    
    static void svg_parseGradient(SVGParser &p, std::map<DataChunk, DataChunk>& attrs, char type)
    {
		BLGradient gradient{};
		std::string id{};
        
       // SVGgradientData* grad = (SVGgradientData*)malloc(sizeof(SVGgradientData));
        //if (grad == NULL) 
        //    return;
        //memset(grad, 0, sizeof(SVGgradientData));
        //grad->units = SVG_OBJECT_SPACE;
        //grad->type = type;
        if (type == SVG_PAINT_LINEAR_GRADIENT) 
        {
			gradient = BLGradient(BLLinearGradientValues{0,0,100,100});
            //grad->linear.x1 = { 0.0f, SVG_UNITS_PERCENT };
            //grad->linear.y1 = {0.0f, SVG_UNITS_PERCENT};
            //grad->linear.x2 = { 100.0f, SVG_UNITS_PERCENT };
            //grad->linear.y2 = { 0.0f, SVG_UNITS_PERCENT };
        }
        else if (type == SVG_PAINT_RADIAL_GRADIENT) 
        {
			gradient = BLGradient(BLRadialGradientValues{ 50, 50, 50, 50, 50 });
            //grad->radial.cx = { 50.0f, SVG_UNITS_PERCENT };
            //grad->radial.cy = { 50.0f, SVG_UNITS_PERCENT };
            //grad->radial.r = { 50.0f, SVG_UNITS_PERCENT };
        }

        //gradient.setMatrix(identityMatrix);

		BLLinearGradientValues linear{};
		BLRadialGradientValues radial{};
        
        BLMatrix2D xform{};
        xform.reset();

        int units{ SVG_USER_SPACE };
        
        // Get all the attributes
        for (auto& keyvalue : attrs)
        {
            if (keyvalue.first == "id") 
            {
				id = std::string(keyvalue.second.fStart, keyvalue.second.fEnd);
                //auto len = copy_to_cstr(grad->id, 63, keyvalue.second);
                //strncpy_s(grad->id, (const char *)keyvalue.second.fStart, 63);
                //grad->id[len] = '\0';
            }
            else if (!svg_parseAttr(p, keyvalue.first, keyvalue.second)) {
                if (keyvalue.first == "gradientUnits") 
                {
                    if (keyvalue.second == "objectBoundingBox")
                        units = SVG_OBJECT_SPACE;
                    else
                        units = SVG_USER_SPACE;
                }
                else if (keyvalue.first == "gradientTransform") {
                    svg_parseTransform(xform, keyvalue.second);
                    //gradient.setMatrix(identityMatrix);
                }
                else if (keyvalue.first == "cx") {
                    
                   //radial.x0 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "cy") {
                    //radial.y0 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "r") {
                    //radial.r0 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "fx") {
                    //radial.x1 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "fy") {
                    //radial.y1 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "x1") {
                    //linear.x0 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "y1") {
                    //linear.y0 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "x2") {
                    //linear.x1 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "y2") {
                    //linear.y1 = svg_parseCoordinateRaw(keyvalue.second);
                }
                else if (keyvalue.first == "spreadMethod") {
                    //if (keyvalue.first == "pad")
                    //    grad->spread = SVG_SPREAD_PAD;
                    //else if (keyvalue.first == "reflect")
                    //    grad->spread = SVG_SPREAD_REFLECT;
                    //else if (keyvalue.first == "repeat")
                    //    grad->spread = SVG_SPREAD_REPEAT;
                }
                else if (keyvalue.first == "xlink:href") {
                    //auto len = copy_to_cstr(grad->ref, 62, keyvalue.second);
                    const char* href = (const char *)keyvalue.second.fStart;
                    //strncpy_s(grad->ref, href + 1, 62);
                    //grad->ref[62] = '\0';
                }
            }
        }

        //grad->next = p.gradients;
        //p.gradients = grad;
    }
    
    /*
            BLGradient gradient(BLRadialGradientValues(fCenter.x, fCenter.y, fCenter.x, fCenter.y, fRadius));
        gradient.addStop(0, p5::color(220, 127));       // center
        gradient.addStop(0.20, p5::color(fBackgroundColor.r(), fBackgroundColor.g(), fBackgroundColor.b(), 127));     // center
        gradient.addStop(0.80, fBackgroundColor);
        gradient.addStop(1.0, p5::color(65, 127));     // edge
  BL_INLINE BLResult addStop(double offset, const BLRgba32& rgba32) noexcept { return blGradientAddStopRgba32(this, offset, rgba32.value); }

    */
    static void svg_parseGradientStop(SVGParser &p, std::map<DataChunk, DataChunk>& attrs)
    {
        SVGattrib & curAttr = p.getAttr();
        SVGgradientData* grad;
        SVGgradientStop* stop;
        int i, idx;

        curAttr.stopOffset = 0;
        curAttr.stopColor = vec4b{ 0 };
        curAttr.stopOpacity = 1.0f;

        for (auto & keyvalue : attrs)
        {
            svg_parseAttr(p, keyvalue.first, keyvalue.second);
        }

        // Add stop to the last gradient.
        grad = p.gradients;
        if (grad == NULL) 
            return;

        grad->nstops++;
        grad->stops = (SVGgradientStop*)realloc(grad->stops, sizeof(SVGgradientStop) * grad->nstops);
        if (grad->stops == NULL) 
            return;

        // Insert
        idx = grad->nstops - 1;
        for (i = 0; i < grad->nstops - 1; i++) {
            if (curAttr.stopOffset < grad->stops[i].offset) {
                idx = i;
                break;
            }
        }
        if (idx != grad->nstops - 1) {
            for (i = grad->nstops - 1; i > idx; i--)
                grad->stops[i] = grad->stops[i - 1];
        }

        stop = &grad->stops[idx];
        stop->color = curAttr.stopColor;
        stop->color.a = (unsigned int)(curAttr.stopOpacity * 255) << 24;
        stop->offset = curAttr.stopOffset;
    }
    
    //
    // SVG specific callback routines
    // These are used to actually turn text values
    // into programmatic values
    //
    static void svg_startElement(SVGParser &p, const DataChunk &el, std::map<DataChunk, DataChunk> & attr)
    {
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
            p.pushAttr();
            svg_parseAttribs(p, attr);
        }
        else if (el == "path") {
            if (p.fPathFlag)	// Do not allow nested paths.
            {
                printf("== NESTED PATH ==\n");
                return;
            }
            p.pushAttr();
            svg_parsePath(p, attr);
            p.popAttr();
        }
        else if (el == "rect") {
            p.pushAttr();
            svg_parseRect(p, attr);
            p.popAttr();
        }
        else if (el == "circle") {
            p.pushAttr();
            svg_parseCircle(p, attr);
            p.popAttr();
        }
        else if (el == "ellipse") {
            p.pushAttr();
            svg_parseEllipse(p, attr);
            p.popAttr();
        }
        else if (el == "line") {
            p.pushAttr();
            svg_parseLine(p, attr);
            p.popAttr();
        }
        else if (el == "polyline") {
            p.pushAttr();
            svg_parsePoly(p, attr, 0);
            p.popAttr();
        }
        else if (el == "polygon") {
            p.pushAttr();
            svg_parsePolygon(p, attr);
            p.popAttr();
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
    }

    static void svg_endElement(SVGParser &p, const DataChunk &el)
    {
        if (el == "g") {
            p.popAttr();
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
        DataChunk s = chunk;
		//DataCursor dc = make_cursor_chunk(chunk);
        
        // storage for raw attribute key/value pairs
		std::map<DataChunk, DataChunk> attr{};
        
        int nattr = 0;
        uint8_t* beginName{};
        uint8_t* endName{};
        bool start = false;
        bool end = false;
        uint8_t quote{};

        // Skip any white space after the '<'
        while (s && whitespaceChars[*s])
            s++;
        
		if (!s)
			return;
        
        // Check if the tag is end tag
        if (*s == '/') 
        {
		    // End tag
            s++;
            end = true;
        }
        else {
            start = true;
        }

        // Skip comments, data and preprocessor stuff.
        if (!s || *s == '?' || *s == '!')
            return;
        

        // Get tag name
        beginName = (uint8_t *)s.fStart;

        while (s && !whitespaceChars[*s])
            s++;

        if (s) 
        { 
            endName = (uint8_t *)s.fStart;
            s++;
        }
        else {
            endName = (uint8_t*)s.fStart;
        }
        auto name = make_chunk(beginName, endName);

        // BUGBUG - let's see what element we're dealing with
        //printf("BEGIN : ");
        //printChunk(name);
        
        // Get the attribute key/value pairs for the element
        while (!end && s && (nattr < NSVG_XML_MAX_ATTRIBS - 3)) 
        {
            uint8_t * beginAttrName = NULL;
			uint8_t * endAttrName = NULL;
            uint8_t* beginattrValue = NULL;
			uint8_t* endattrValue = NULL;


            // Skip white space before the attrib name
            while (s && whitespaceChars[*s])
                s++;

			if (!s)
				break;

            
            if (*s == '/') {
                end = true;
                break;
            }

            beginAttrName = (uint8_t *)s.fStart;
            
            // Find end of the attrib name.
            //while (s && whitespaceChars[*s])
            //    s++;
            
            while (s && !whitespaceChars[*s] && *s != '=')
                s++;
            
            if (s) 
            {
                endAttrName = (uint8_t *)s.fStart;
                s++;
            }
			else {
				endAttrName = (uint8_t*)s.fStart;
			}
            DataChunk attrName = make_chunk(beginAttrName, endAttrName);
            //printf("     ATTR :");
            //writeChunk(attrName);
            
            // Skip until the beginning of the value.
            while (s && (*s != '\"') && (*s != '\''))
                s++;


            
            if (!s) 
                break;
            
            
            quote = *s;
            
            // Store value and find the end of it.
            s++;
			beginattrValue = (uint8_t *)s.fStart;

			// Skip until end of the value.
            while (s && *s != quote) 
                s++;
            
            if (s)
            { 
				endattrValue =(uint8_t *)s.fStart;
				s++;
            }

            // Store only well formed attributes
			DataChunk attrValue = make_chunk(beginattrValue, endattrValue);
            //printf("    VALUE :");
            //printChunk(attrValue);
            
			attr.insert(std::make_pair(attrName, attrValue));

			nattr++;
        }

        // Call callbacks.
        if (start && startelCb)
            startelCb(p, name, attr);
        if (end && endelCb)
            endelCb(p, name);
    }
    
    static int parseXML(SVGParser& p, const DataChunk& chunk, StartElementCallback startelCb = nullptr, EndElementCallback endelCb = nullptr, ContentCallback contentCb = nullptr)
    {
		char* s = (char*)chunk.fStart;
        char* mark = s;
        int state = NSVG_XML_CONTENT;
        
        while (s < (char *)chunk.fEnd) 
        {
            // Start of a tag
            if (*s == '<' && state == NSVG_XML_CONTENT) 
            {
				// consume the '<' to be placed at the start of the tag
                // which is not part of the content
                s++;
                DataChunk markChunk = make_chunk(mark, s);
                xml_parseContent(p, markChunk, contentCb);
                mark = s;
                state = NSVG_XML_TAG;
            }
            else if (*s == '>' && state == NSVG_XML_TAG) 
            {
            // mark the end of a tag.
            DataChunk markChunk = make_chunk(mark, s);
            s++;

            xml_parseElement(p, markChunk, startelCb, endelCb);
            mark = s;
            state = NSVG_XML_CONTENT;
        }
        else {
            s++;
        }
    }

    return 1;
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
    static bool parseSVGDocument(const DataChunk &chunk, SVGParser& p, const char *units = "px", float dpi=96)
    {   
        p.fDpi = dpi;
        
		parseXML(p, chunk, svg_startElement, svg_endElement, svg_content);
        
		return true;
    }
}