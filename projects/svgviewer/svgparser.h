#pragma once


#include "maths.hpp"

#include "shaper.h"
#include "geometry.h"

#include "blend2d.h"
#include "chunkutil.h"

#include "svgxform.h"
#include "svgcolors.h"

#include <functional>
#include <charconv>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <stack>



namespace svg
{
    using namespace ndt;

    static charset nonprintWSPChars("\r\n\f\v");
    static charset whitespaceChars(",\t\n\f\r\v ");
	static charset wspChars("\t\n\f\r\v ");
    static charset digitChars("0123456789");
    static charset numberChars("0123456789.-+eE");

    constexpr auto NSVG_XML_START_TAG = 1;
    constexpr auto NSVG_XML_CONTENT = 2;
    constexpr auto NSVG_XML_MAX_ATTRIBS = 256;


    constexpr auto SVG_MAX_ATTR = 128;
    constexpr auto SVG_MAX_DASHES = 8;
}

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

    struct SVGcoordinate {
        float value;
        int units;
    } ;
    
    struct SVGgradientStop {
        vec4b color;
        float offset;
    } ;

    struct SVGgradient {
        float xform[6];
        char spread;
        float fx, fy;
        int nstops;
        // BUGBUG - should be a vector
        SVGgradientStop stops[1];
    } ;

    struct SVGpaint {
        char type;
        union {
            vec4b color;
            SVGgradient* gradient;
        };
    };


    // Represents the actual kind of data in the Attribute
    // union data section
    enum SVGAttributeKind
    {
        INVALID = 0
		, BOOL              // bool
		, ENUM              // int
        , NUMBER            // floating point number
        , COORDINATE
        , TRANSFORM
        , CHUNK

    };
    
    struct SVGAttribute
    {
		std::string fName{};        // Name of the attribute
        DataChunk fRawData{};         // The original chunk of data
		SVGAttributeKind fKind;     // The kind of data
        
        union {
            bool            fBool{ false };
            int             fEnum;
            float           fNumber;
			SVGcoordinate   fCoordinate;
			BLMatrix2D      fTransform;
			DataChunk       fChunk;
        };
    };

    struct SVGattrib
    {
        std::string fID;
        
        BLMatrix2D xform{};
        float opacity{ 1.0 };
        
        BLVar fFill;
        float fillOpacity{ 1.0 };
        char hasFill{ 0 };
        
        // Stroke lines
        char hasStroke{ 0 };
        BLVar fStroke{};
        float strokeWidth{ 1 };
        float strokeOpacity{ 1.0 };
        
        // Dashed lines
        float strokeDashOffset{};
        float strokeDashArray[SVG_MAX_DASHES]{ 0 };
        int strokeDashCount{ 0 };
        char strokeLineJoin{};
        char strokeLineCap{};
        float strokeMiterLimit{1};
        
        char fillRule{};
        
        float fontSize{};
        
        // Gradient related
        char fillGradient[64]{ 0 };
        char strokeGradient[64]{ 0 };
        vec4b stopColor{};
        float stopOpacity{ 1.0 };
        float stopOffset{};


        char visible{ 1 };
            
        SVGattrib()
        {
            // set default attribute values
			setFillColor(maths::vec4b{ 0, 0, 0, 255 });
            
			xform.reset();
        }
        
        SVGattrib& operator=(const SVGattrib& other)
        {   
			fID = other.fID;
            xform = other.xform;
            opacity = other.opacity;
            
            // Filling attributes
            fFill.assign(other.fFill);
            fillOpacity = other.fillOpacity;
            hasFill = other.hasFill;
            
            // Stroking attributes
            fStroke.assign(other.fStroke);
            strokeOpacity = other.strokeOpacity;
            strokeWidth = other.strokeWidth;
            hasStroke = other.hasStroke;
            
            
            // Stroking dashed lines
            strokeDashOffset = other.strokeDashOffset;
            strokeDashCount = other.strokeDashCount;
            memcpy(strokeDashArray, other.strokeDashArray, sizeof(other.strokeDashArray));

            // Drawing polygons
            fillRule = other.fillRule;
            strokeLineJoin = other.strokeLineJoin;
            strokeLineCap = other.strokeLineCap;
            strokeMiterLimit = other.strokeMiterLimit;
            
            // Drawing text
            // BUGBUG - need to add alignment
            fontSize = other.fontSize;

            
            // Related to gradients
            stopColor = other.stopColor;
            stopOpacity = other.stopOpacity;
            stopOffset = other.stopOffset;
            memcpy(fillGradient, other.fillGradient, sizeof(other.fillGradient));
            memcpy(strokeGradient, other.strokeGradient, sizeof(other.strokeGradient));

            visible = other.visible;

            return *this;
        }
        
        void setID(const std::string& s) { fID = s; }

        const std::string& getID() const { return fID; }
        
		void setFillColor(const vec4b& color)
		{
			blVarAssignRgba32(&fFill, color.value);
            
            hasFill = 1;
		}
        
		void setStrokeColor(const vec4b& color)
		{
			blVarAssignRgba32(&fStroke, color.value);
            
            hasStroke = 1;
		}
    } ;
    

    
    struct SVGElement
    {
        SVGattrib fAttrib;

        SVGElement& operator=(const SVGElement& other)
        {
            return *this;
        }

        void setAttributes(const SVGattrib& attr)
        {
            fAttrib = attr;
        }

        void setID(const std::string& s)
        {
            fAttrib.setID(s);
        }
    };
    
    // Contains geometry and stroke/fill attributes
    struct SVGShape : public SVGElement, public GraphicElement
    {
        SVGShape(const rectf &fr)
            :GraphicElement(fr)
        {

        }


        void setFill(BLVar& fill) { fAttrib.fFill = fill;}

        void setStrokeWidth(float w){ fAttrib.strokeWidth = w;}

        void setStroke(BLVar& stroke) { fAttrib.fStroke = stroke;}
        
        void draw(IGraphics& ctx)
        {
            ctx.push();
            
            // If there's a transform, apply it
			if (fAttrib.xform.type() != BL_MATRIX2D_TYPE_IDENTITY)
				ctx.transform(fAttrib.xform.m);
            
			if (fAttrib.hasFill)
			{
				ctx.fill(fAttrib.fFill);
            }
            else
                ctx.noFill();
            

            if (fAttrib.hasStroke>0)
			{
				ctx.stroke(fAttrib.fStroke);
				ctx.strokeWeight(fAttrib.strokeWidth);
				ctx.strokeJoin(fAttrib.strokeLineJoin);
				ctx.strokeCaps(fAttrib.strokeLineCap);
				ctx.strokeMiterLimit(fAttrib.strokeMiterLimit);
			}
            else {
                ctx.noStroke();
            }
            
            
            drawSelf(ctx);
            
            ctx.pop();
        }
    };

	struct SVGPathShape : public SVGShape
	{
        BLPath fPath{};
        
		SVGPathShape(const rectf& fr)
			:SVGShape(fr)
		{

		}

        void setPath(const BLPath& apath) { fPath = apath; }
        
        void drawSelf(IGraphics& ctx) override
        {
            ctx.path(fPath);
        }
	};
    
    struct SVGTextShape : public SVGShape
    {
		std::string fText;

		SVGTextShape(const rectf& fr)
            :SVGShape(fr)
		{
		}

		void setText(const std::string& text) { fText = text; }

        void drawSelf(IGraphics& ctx)
        {

        }
    };
}



namespace svg
{
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
		std::stack<SVGattrib> fAttribStack;
        
        std::map<std::string, SVGElement> fDefinitions{};

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

        DataChunk fCurrentContent{};

        

        SVGParser()
        {
			// Start the attribute stack with a default attribute
			fAttribStack.push(SVGattrib());
            
            
            return ;
        }
        
        
        SVGattrib& getAttr()
        {
			return fAttribStack.top();
        }

		void pushAttr()
        {
			fAttribStack.push(fAttribStack.top()); // copy the top of the stack
        }

        void popAttr()
        {
            if (fAttribStack.size() > 0)
                fAttribStack.pop();
            else
                printf("popAttr - EMPTY STACK\n");
        }

		void setContent(const DataChunk& content)
		{
			fCurrentContent = content;
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

		void addDefinition(std::string & name, SVGElement &value)
		{
			fDefinitions[name] = value;
		}
        
        void addShape()
        {
			// we might be adding a shape with no path
            // like gradients, or other style elements
            // 
            
            // If the path is empty, don't do anything
            if (fWorkingPath.empty())
                return;

            // Create a new shape, using the current working path
            // and accumulated attributes
			auto newShape = std::make_shared<SVGPathShape>(rectf{ viewMinx,viewMiny,viewWidth, viewHeight });

            newShape->setPath(fWorkingPath);
			newShape->setAttributes(getAttr());
            
			if (fDefsFlag)
			{
				//printf("DEFINITION: %s\n", getAttr().fID.c_str());
				//addDefinition(getAttr().fID, newShape);
			}
			else
			{
				fShapes.push_back(newShape);
			}
            
            //fShapes.push_back(newShape);

            resetPath();
        }

        void addText()
        {
            if (!fCurrentContent)
                return;

            //auto newShape = std::make_shared<SVGTextShape>(rectf{ viewMinx,viewMiny,viewWidth, viewHeight });

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


	using attributeMap = std::map<std::string, SVGattrib>;
    
    typedef std::function<void(SVGParser& p, const DataChunk& elName, attributeMap& attrs)> StartElementCallback;
    typedef std::function<void(SVGParser& p, const DataChunk& elName)> EndElementCallback;
    typedef std::function<void(SVGParser& p, DataChunk& dc)> ContentCallback;


    static INLINE float svg_minf(float a, float b) { return a < b ? a : b; }
    static INLINE float svg_maxf(float a, float b) { return a > b ? a : b; }
    
    static INLINE DataChunk skipWhitespace(DataChunk& dc)
    {
        chunk_ltrim(dc, wspChars);
        return dc;
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
    

    //=====================================================
    // Parsing attributes
    //=====================================================
    
    static int svg_parseUnits(const DataChunk &units)
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
    
    static SVGcoordinate svg_parseCoordinateRaw(const DataChunk & strChunk)
    {
        SVGcoordinate coord{ 0, SVG_UNITS_USER };
        char buf[64]{};
		DataChunk numChunk = make_chunk_size(buf, 64);
        coord.units = svg_parseUnits(ndt::scanNumber(strChunk, numChunk));
        coord.value = (float)chunk_to_double(numChunk);
        
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
			return rgba(128, 128, 128,0);
        
        while (num)
        {
            //writeChunk(num);
			if (chunk_find_char(num, '%'))
			{
                // it's a percentage
                // BUGBUG - we're assuming it's a range of [0..255]
                auto cv = svg_parseCoordinateRaw(num);
                rgbi[i] = (uint8_t)(cv.value / 100.0f * 255.0f);
			}
			else
			{
				// it's a regular number
				rgbi[i] = (uint8_t)svg_parseCoordinateRaw(num).value;
			}
            i++;
			num = chunk_token(nums, ",");
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
		DataChunk str = inChunk;
		DataChunk rgbStr = make_chunk_cstr("rgb(");
		DataChunk rgbaStr = make_chunk_cstr("rgba(");
        
        size_t len = 0;
		skipWhitespace(str);

        
        len = size(str);
        if (len >= 1 && *str == '#')
        {
            return svg_parseColorHex(str);
        } else if (chunk_starts_with(str, rgbStr) || chunk_starts_with(str, rgbaStr))
        {
            //else if (len >= 4 && str[0] == 'r' && str[1] == 'g' && str[2] == 'b' && str[3] == '(')
            return svg_parseColorRGB(str);
        }
        
        return svg_parseColorName(str);
    }
    
    static float svg_parseOpacity(const DataChunk &inChunk)
    {
        DataChunk s = inChunk;
        float val = (float)chunk_to_double(s);
        return maths::clamp(val, 0.0f, 1.0f);
    }
    
    static float svg_parseMiterLimit(const DataChunk &inChunk)
    {
        DataChunk s = inChunk;
        float val = (float)chunk_to_double(s);
        if (val < 0.0f) val = 0.0f;
        return val;
    }

    static DataChunk svg_parseUrl(char* id, const DataChunk &inChunk)
    {
        DataChunk str = inChunk;
        DataChunk outChunk{};
        
		// skp past 'url('
        int i = 0;
		str += 4;


		// skip past leading '#'
        if (*str == '#')
            str++;
		
        outChunk = str;
        outChunk.fEnd = outChunk.fStart;
        
        while (str && *str != ')')
            str++;
        
		// Only report if the parameter is well formed (bracketd by '(' and ')')
        if (*str == ')')
			outChunk.fEnd = str.fStart;

        return outChunk;
    }
    
    std::map<std::string, int> enumMap = {
        {"butt", SVG_CAP_BUTT}
        ,{"round", SVG_CAP_ROUND}
		,{"square", SVG_CAP_SQUARE}
		,{"miter", SVG_JOIN_MITER}
		,{"round", SVG_JOIN_ROUND}
		,{"bevel", SVG_JOIN_BEVEL}
		//,{"none", SVG_FILL_NONE}
        ,{"nonzero", SVG_FILLRULE_NONZERO}
		,{"evenodd", SVG_FILLRULE_EVENODD}
        
    };
    
    static char svg_parseLineCap(const DataChunk & str)
    {
        if (str == "butt")
            return SVG_CAP_BUTT;
        else if (str == "round")
            return SVG_CAP_ROUND;
        else if (str == "square")
            return SVG_CAP_SQUARE;
        
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
        
        return SVG_JOIN_MITER;
    }
    
    static char svg_parseFillRule(const DataChunk & str)
    {
        if (str == "nonzero")
            return SVG_FILLRULE_NONZERO;
        else if (str == "evenodd")
            return SVG_FILLRULE_EVENODD;
        
        return SVG_FILLRULE_NONZERO;
    }
    
    static DataChunk svg_getNextDashItem(const DataChunk &inChunk, DataChunk &it)
    {
        int sz = size(it);
        int n = 0;
        it[0] = '\0';
        
		DataChunk s = inChunk;
        
        // Skip white spaces and commas
        while (s && (wspChars(*s) || *s == ',')) 
            s++;
        
        // Advance until next whitespace, comma, or end.
        while (s && (!wspChars(*s) && *s != ',')) 
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
                args[(na)++] = (float)chunk_to_double(numChunk);
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

		DataChunk s = inChunk;
        BLMatrix2D tm{};
        
        while (s)
        {
			// Set out temp transform to the identity to start
            // so that if parsing goes wrong, we can still do
            // the multiply without worrying about messing things up
			// That means, the individula parsing functions need to not
			// partially mess up the transform if they fail.
            //svg_xformIdentity(t);

            tm.reset();
            
            if (comparen_cstr(s, "matrix", 6) == 0)
                s = svg_parseMatrix(tm, s);
            else if (comparen_cstr(s, "translate", 9) == 0)
                s = svg_parseTranslate(tm, s);
            else if (comparen_cstr(s, "scale", 5) == 0)
                s = svg_parseScale(tm, s);
            else if (comparen_cstr(s, "rotate", 6) == 0)
                s = svg_parseRotate(tm, s);
            else if (comparen_cstr(s, "skewX", 5) == 0)
                s = svg_parseSkewX(tm, s);
            else if (comparen_cstr(s, "skewY", 5) == 0)
                s = svg_parseSkewY(tm, s);
            else {
                s++;
                continue;
            }
            
            //xform.transform(tm);
        }
        xform = tm;
    }
    
    
    // Parse specific kinds of attributes
    // As the attribute is parsed, the value is stored in the named
	// attribute of the parser.
    static DataChunk svg_parseStyle(SVGParser& p, const DataChunk& chunk);
    
    static int svg_parseAttr(SVGParser & p, const std::string &name, const DataChunk & value)
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
                auto url = svg_parseUrl(attr.fillGradient, value);
                // BUGBUG - need to use the URL to lookup a fill object
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
            attr.strokeMiterLimit = svg_parseMiterLimit(value);
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
            attr.xform = xform;
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
            std::string idvalue(value.fStart, value.fEnd);
            attr.setID(idvalue);
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
        // split nameValueChunk at the ':'
        charset keyvaldelim(":");
		auto nameChunk = chunk_trim(chunk_token(nameValueChunk, keyvaldelim), wspChars);
		auto valueChunk = chunk_trim(nameValueChunk, wspChars);

        std::string attrName = std::string(nameChunk.fStart, nameChunk.fEnd);
        
        //printf("%20s| ", attrName.c_str());
        //printChunk(valueChunk);
        
        return svg_parseAttr(p, attrName, valueChunk);
    }

    //
	// Parse a style attribute
	//
    static DataChunk svg_parseStyle(SVGParser &p, const DataChunk & chunk)
    {
		DataChunk s = chunk;
        //DataChunk nameValueChunk{};
        
        charset delims(";");


        auto nameValue = chunk_token(s, delims);

        while (nameValue)
        {
            // We have a style key/value pair
            // split that into the key, and value
            // and parse the attribute from there
            svg_parseNameValue(p, nameValue);

            // Look for the next keyvalue pair
            nameValue = chunk_token(s, delims);
        }

        return s;
    }

 
    //
	// Parse an element's attributes
	//
    static void svg_parseAttribs(SVGParser &p, attributeMap& attr)
    {
        for (auto& a : attr) {

            DataChunk& value = a.second.fRawData;
            
            if (a.first == "style")
               svg_parseStyle(p,value);
            else
                svg_parseAttr(p, a.first, value);
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

        //svg_xformMultiply(grad->xform, data->xform);
        //svg_xformMultiply(grad->xform, attr.xform);

        grad->spread = data->spread;
        //memcpy(grad->stops, stops, nstops * sizeof(NSVGgradientStop));
        grad->nstops = nstops;

        *paintType = data->type;

        return grad;
    }

    //=====================================================
    // Parsing specific elements
    //=====================================================
    static void svg_parseLine(SVGParser & p, attributeMap& attrs)
    {
        float x1 = 0.0;
        float y1 = 0.0;
        float x2 = 0.0;
        float y2 = 0.0;


        for (auto& keyvalue : attrs) 
        {
			if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData))

            {
                if (keyvalue.first == "x1") x1 = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "y1") y1 = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "x2") x2 = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "y2") y2 = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigY(), p.actualHeight());
            }
        }
        
        p.addLine(x1, y1, x2, y2);

    }
    
    static void svg_parseRect(SVGParser & p, attributeMap& attrs)
    {
        float x = 0.0f;
        float y = 0.0f;
        float w = 0.0f;
        float h = 0.0f;
        float rx = -1.0f; // marks not set
        float ry = -1.0f;

        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData)) {
                if (keyvalue.first == "x") x = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "y") y = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "width") w = svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, p.actualWidth());
                if (keyvalue.first == "height") h = svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, p.actualHeight());
                if (keyvalue.first == "rx") rx = fabsf(svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, p.actualWidth()));
                if (keyvalue.first == "ry") ry = fabsf(svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, p.actualHeight()));
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
    
    static void svg_parseCircle(SVGParser &p, attributeMap& attrs)
    {
        float cx = 0.0f;
        float cy = 0.0f;
        float r = 0.0f;


        // parse attributes of a circle
        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData))
            {
                if (keyvalue.first == "cx") cx = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "cy") cy = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "r") r = fabsf(svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, p.actualLength()));
            }
        }

        p.addCircle(cx, cy, r);

    }

    static void svg_parseEllipse(SVGParser &p, attributeMap& attrs)
    {
        float cx = 0.0f;
        float cy = 0.0f;
        float rx = 0.0f;
        float ry = 0.0f;


        // Get the attributes of the ellipse
        for (auto& keyvalue : attrs) 
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData))
            {
                if (keyvalue.first == "cx") cx = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigX(), p.actualWidth());
                if (keyvalue.first == "cy") cy = svg_parseCoordinate(p, keyvalue.second.fRawData, p.actualOrigY(), p.actualHeight());
                if (keyvalue.first == "rx") rx = fabsf(svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, p.actualWidth()));
                if (keyvalue.first == "ry") ry = fabsf(svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f,p.actualHeight()));
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
    
    static void svg_parsePoly(SVGParser & p, attributeMap& attrs, bool closeFlag)
    {
        DataChunk s{};
        float args[2];
        int nargs, npts = 0;
        uint8_t itembuff[64]{ 0 };

		BLArray <BLPoint> points;

        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData))
            {
                if (keyvalue.first == "points") 
                {
                    s = keyvalue.second.fRawData;
                    nargs = 0;
                    while (*s) {

                        DataChunk item = make_chunk_size(itembuff, 64); // reset for each run

                        s = svg_getNextPathItem(s, item);
                        args[nargs++] = (float)chunk_to_double(item);
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
    
	static void svg_parsePolygon(SVGParser& p, attributeMap& attrs)
	{
		svg_parsePoly(p, attrs, true);
	}
    
    static void svg_parsePath(SVGParser& p, attributeMap& attrs)
    {
        DataChunk s{};
        std::vector<ndt::PathSegment> segments{};
        
        // Cycle through the attributes
        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData))
            {
                if (keyvalue.first == "d")
                {
                    // parse into segments
                    ndt::tokenizePath(keyvalue.second.fRawData, segments);
                }
            }
        }
        
        // add the path to the parser
        p.addPath(segments);
    }
    
    
    static void svg_parseText(SVGParser& p, attributeMap& attrs)
    {
        svg_parseAttribs(p, attrs);
    }

    

    static void svg_parseSVG(SVGParser &p, attributeMap& attrs)
    {

        for (auto& keyvalue : attrs)
        {
            if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData)) {
                if (keyvalue.first == "width") {
                    p.imageWidth = svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, 0.0f);
                }
                else if (keyvalue.first == "height") {
                    p.imageHeight = svg_parseCoordinate(p, keyvalue.second.fRawData, 0.0f, 0.0f);
                }
                else if (keyvalue.first == "viewBox") {
                    // Assuming we're already on the numeric part
                    DataChunk s = keyvalue.second.fRawData;
                    DataChunk numChunk{};

                    s = nextNumber(s, numChunk);
                    p.viewMinx = (float)chunk_to_double(numChunk);

                    s = nextNumber(s, numChunk);
                    p.viewMiny = (float)chunk_to_double(numChunk);

                    s = nextNumber(s, numChunk);
                    p.viewWidth = (float)chunk_to_double(numChunk);

                    s = nextNumber(s, numChunk);
                    p.viewHeight = (float)chunk_to_double(numChunk);

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
    
    
    
    static void svg_parseGradient(SVGParser &p, attributeMap& attrs, char type)
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
				id = std::string(keyvalue.second.fRawData.fStart, keyvalue.second.fRawData.fEnd);

            }
            else if (!svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData)) {
                if (keyvalue.first == "gradientUnits") 
                {
                    if (keyvalue.second.fRawData == "objectBoundingBox")
                        units = SVG_OBJECT_SPACE;
                    else
                        units = SVG_USER_SPACE;
                }
                else if (keyvalue.first == "gradientTransform") {
                    svg_parseTransform(xform, keyvalue.second.fRawData);
                    //gradient.setMatrix(identityMatrix);
                }
                else if (keyvalue.first == "cx") {
                    
                   //radial.x0 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "cy") {
                    //radial.y0 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "r") {
                    //radial.r0 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "fx") {
                    //radial.x1 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "fy") {
                    //radial.y1 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "x1") {
                    //linear.x0 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "y1") {
                    //linear.y0 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "x2") {
                    //linear.x1 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                }
                else if (keyvalue.first == "y2") {
                    //linear.y1 = svg_parseCoordinateRaw(keyvalue.second.fRawData);
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
                    //auto len = copy_to_cstr(grad->ref, 62, keyvalue.second.fRawData);
                    const char* href = (const char *)keyvalue.second.fRawData.fStart;
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
    static void svg_parseGradientStop(SVGParser &p, attributeMap& attrs)
    {
        SVGattrib & curAttr = p.getAttr();
        
		BLGradientStop astop{};
        
        //SVGgradientData* grad;
        //SVGgradientStop* stop;
        //int i, idx;

        curAttr.stopOffset = 0;
        curAttr.stopColor = vec4b{ 0 };
        curAttr.stopOpacity = 1.0f;

        for (auto & keyvalue : attrs)
        {
            svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData);
        }

        // Add stop to the last gradient.
        //grad = p.gradients;
        //if (grad == NULL) 
        //    return;

        //grad->nstops++;
        //grad->stops = (SVGgradientStop*)realloc(grad->stops, sizeof(SVGgradientStop) * grad->nstops);
        //if (grad->stops == NULL) 
        //    return;
/*
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
        */
    }
    
    static void svg_parseLinearGradient(SVGParser& p, attributeMap& attrs)
    {
        BLLinearGradientValues linearValues{};
        std::string id{};
        
        for (auto& keyvalue : attrs)
        {
            svg_parseAttr(p, keyvalue.first, keyvalue.second.fRawData);
            
            if (keyvalue.first == "id")
            {
                id = std::string(keyvalue.second.fRawData.fStart, keyvalue.second.fRawData.fEnd);
            }
            else if (keyvalue.first == "x1") {
				// BUGBUG - should use parseCoordinate() to convert to pixels?
                auto c = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                linearValues.x0 = c.value;
            }
            else if (keyvalue.first == "y1") {
                auto c = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                linearValues.y0 = c.value;
            }
            else if (keyvalue.first == "x2") {
                auto c = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                linearValues.x1 = c.value;
            }
            else if (keyvalue.first == "y2") {
                auto c = svg_parseCoordinateRaw(keyvalue.second.fRawData);
                linearValues.y1 = c.value;
            }
        }

        // After constructing the base gradient
        // save it on the gradient stack so that when stops are added
        // they will be added to this last gradient
        
    }
    
    static void svg_parseGroup(SVGParser& p, attributeMap& attrs)
    {
        svg_parseAttribs(p, attrs);
    }


    
    //
    // SVG specific callback routines
    // These are used to actually turn text values
    // into programmatic values
    //
    static void svg_startElement(SVGParser &p, const DataChunk &el, attributeMap& attr)
    {
        //printf("BEGIN ELEMENT : ");
        //printChunk(el);
        
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
        */
        
        if (el == "g") {
            p.pushAttr();
            svg_parseGroup(p, attr);
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
            p.pushAttr();
            svg_parseLinearGradient(p, attr);
            p.popAttr();
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
        else if (el == "text") {
            p.pushAttr();
            svg_parseText(p,attr);
        }
        else {
            //printf("UNKNOWN ELEMENT: ");
            //printChunk(el);
        }
    }

    static void svg_endElement(SVGParser &p, const DataChunk &el)
    {
        // BUGBUG
        // If ending a gradient
		// pop it off the stack
        // and either add it to the current attribute
        // or save it in definitions map if p.fDefsFlag == true
                // BUGBUG - let's see what element we're dealing with
        //printf("END ELEMENT : ");
        //printChunk(el);
        
        if (el == "g") {
            p.popAttr();
        }
        else if (el == "path") {
            p.fPathFlag = false;
        }
        else if (el == "defs") {
            p.fDefsFlag = false;
        }
		else if (el == "text") {
            p.addText();
            p.popAttr();
		}
    }
    
    static void svg_content(SVGParser &p, DataChunk &dc)
    {
        //printf("CONTENT\n");
        //printChunk(dc);
        p.setContent(dc);
        
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
		DataChunk dc = content;
        
        // Trim front and back white space
        // if we're left with a blank content
        // by then, don't do anything.
        // This will essentially NOT preserve whitespace
        // we could do it better by only trimming the non-printable
		// whitespace, leaving 'SPACE' and 'TAB' alone
        chunk_trim(dc, whitespaceChars);
        
        if (!dc)
            return;
   
        
        if (contentCb != nullptr)
            contentCb(p,dc);
    }
    
    //
	// Parse an XML element
	// We should be sitting on the first character of the element tag after the '<'
    // There are several things that need to happen here
	// 1) Scan the element name
	// 2) Scan the attributes, creating key/value pairs
	// 3) Figure out if this is a self closing element
	// 4) Call the startElement callback
	// 5) Call the endElement callback
    // 
    // We do NOT scan the content of the element here, that happens
    // outside this routine.  We only deal with what comes up the the closing '>'
    //
    static void xml_parseElement(SVGParser& p, DataChunk &chunk, StartElementCallback startelCb,EndElementCallback endelCb)
    {
        DataChunk s = chunk;
        
        // storage for raw attribute key/value pairs
        attributeMap attr{};
        
        int nattr = 0;
        bool start = false;
        bool end = false;
        uint8_t quote{};

        // Skip any white space after the '<'
		skipWhitespace(s);
        
        // If the chunk is empty, just return
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
        // BUGBUG - these are already handled in parseXML
        if (!s || *s == '?' || *s == '!')
            return;
        

        // Get tag name
        DataChunk tagName = s;
        tagName.fEnd = s.fStart;

        while (s && !whitespaceChars[*s])
            s++;

        tagName.fEnd = (uint8_t*)s.fStart;
        s++;
        
        // Get the attribute key/value pairs for the element
        while (s && !end && (nattr < NSVG_XML_MAX_ATTRIBS - 3)) 
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
            
            // Find end of the attrib name.
            static charset equalChars("=");
			auto attrNameChunk = chunk_token(s, equalChars);
            chunk_trim(attrNameChunk, whitespaceChars);
            
			std::string attrName = std::string(attrNameChunk.fStart, attrNameChunk.fEnd);
            
            //printf("     ATTR : %s", attrName.c_str());
            

            // Skip stuff past '=' until the beginning of the value.
            while (s && (*s != '\"') && (*s != '\''))
                s++;

            if (!s) 
                break;
            
            // capture the quote character
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
            
            auto apair = std::make_pair(attrName, SVGAttribute{ attrName,attrValue });
			attr.insert(apair);

			nattr++;
        }

        // Call callbacks.
        if (start && startelCb)
            startelCb(p, tagName, attr);
        if (end && endelCb)
            endelCb(p, tagName);
    }
    
    static int parseXML(SVGParser& p, const DataChunk& chunk, StartElementCallback startelCb = nullptr, EndElementCallback endelCb = nullptr, ContentCallback contentCb = nullptr)
    {
        DataChunk s = chunk;
        DataChunk mark = s;
        int state = NSVG_XML_CONTENT;

		while (s)
		{
			if (state == NSVG_XML_CONTENT)
			{
				if (*s == '<')
				{
					if (s != mark)
					{
						// Content callback
                        DataChunk content = { mark.fStart, s.fStart };
						xml_parseContent(p, content, contentCb);
					}
					state = NSVG_XML_START_TAG;
                    s++;
                    mark = s;
                }
                else {
                    s++;
                }

			}
			else if (state == NSVG_XML_START_TAG)
			{
                // Create a chunk that encapsulates the element tag 
				// up to, but not including, the '>' character
                DataChunk elementChunk = s;
                elementChunk.fEnd = s.fStart;
                
                while (s && *s != '>')
                    s++;
                
				elementChunk.fEnd = (uint8_t*)s.fStart;

                // Element
                xml_parseElement(p, elementChunk, startelCb, endelCb);
                state = NSVG_XML_CONTENT;
                
				// Move past the '>' character
                if (s)
                    s++;
                mark = s;

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