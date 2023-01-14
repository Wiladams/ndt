#pragma once

#include "chunkutil.h"
#include "shaper.h"
#include "maths.hpp"
#include "coloring.h"
#include "svgcolors.h"
#include "graphic.hpp"
#include "xmlscan.h"
#include "cssscanner.h"
#include "base64.h"
#include "gifdec.h"


// https://www.w3.org/TR/css3-values/#numbers
//

namespace svg {
    using namespace ndt;
    using namespace maths;
}

namespace svg {

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
    struct IMapSVGNodes;
    
    struct SVGObject : public IDrawable
    {
        IMapSVGNodes* fRoot{ nullptr };
        std::string fId{};      // The id of the element
        std::string fName{};    // The tag name of the element
        BLVar fVar{};
        bool fIsVisible{ true };
        maths::bbox2f fExtent{};

        
		SVGObject() = delete;
        SVGObject(const SVGObject& other) :fName(other.fName) 
        {
			fRoot = other.fRoot;
            fId = other.fId;
            fName = other.fName;
			fIsVisible = other.fIsVisible;
			fExtent = other.fExtent;
        }
        SVGObject(IMapSVGNodes* root) :fRoot(root) {}
		virtual ~SVGObject() = default;
        
		SVGObject& operator=(const SVGObject& other) {
            fRoot = other.fRoot;
            fId = other.fId;
            fName = other.fName;
			fIsVisible = other.fIsVisible;
            fExtent = other.fExtent;
			blVarAssignWeak(&fVar, &other.fVar);
            
            return *this;
		}
        
		IMapSVGNodes* root() const { return fRoot; }
        virtual void setRoot(IMapSVGNodes* root) { fRoot = root; }
        
        const std::string& id() const { return fId; }
        void setId(const std::string& id) { fId = id; }
        
        const std::string& name() const { return fName; }
        void setName(const std::string& name) { fName = name; }

		const bool visible() const { return fIsVisible; }
		void setVisible(bool visible) { 
            fIsVisible = visible; 
        }
        
        
        // sub-classes should return something interesting as BLVar
        // This can be used for styling, so images, colors, patterns, gradients, etc
        virtual const BLVar& getVariant()
        {
            return fVar;
        }
        

        
        void draw(IGraphics& ctx) override
        {
            ;// draw the object
        }
        
        virtual void loadSelfFromXml(const XmlElement& elem)
        {
            
        }

        virtual void loadFromXmlElement(const ndt::XmlElement& elem)
        {
            auto id = elem.getAttribute("id");
            if (id)
                setId(std::string(id.fStart, id.fEnd));
            

            
            // load the common attributes
            setName(elem.name());

            // call to loadselffromxml
            // so sub-class can do its own loading
            loadSelfFromXml(elem);
        }
    };
    
    struct IMapSVGNodes
    {
        virtual std::shared_ptr<SVGObject> findNodeById(const std::string& name) = 0;
        virtual std::shared_ptr<SVGObject> findNodeByHref(const DataChunk& href) = 0;

        
        virtual void addDefinition(const std::string& name, std::shared_ptr<SVGObject> obj) = 0;

        virtual void setInDefinitions(bool indefs) = 0;
        virtual bool inDefinitions() const = 0;
    };
    
    
    
    // SVGVisualProperty
    // This is meant to be the base class for things that are optionally
    // used to alter the graphics context.
    // If isSet() is true, then the drawSelf() is called.
	// sub-classes should override drawSelf() to do the actual drawing
    //
    // This is used for things like; Paint, Transform, Miter, etc.
    //
    struct SVGVisualProperty :  public SVGObject
    {
        bool fIsSet{ false };

        //SVGVisualProperty() :SVGObject(),fIsSet(false){}
        SVGVisualProperty(IMapSVGNodes *root):SVGObject(root),fIsSet(false){}
        SVGVisualProperty(const SVGVisualProperty& other)
            :SVGObject(other)
            ,fIsSet(other.fIsSet)
        {}

        SVGVisualProperty operator=(const SVGVisualProperty& rhs)
        {
            SVGObject::operator=(rhs);
            fIsSet = rhs.fIsSet;
            
            return *this;
        }

        void set(const bool value) { fIsSet = value; }
        bool isSet() const { return fIsSet; }

		virtual void loadSelfFromChunk(const DataChunk& chunk)
        {
			;
        }

        virtual void loadFromChunk(const DataChunk& chunk)
        {
			loadSelfFromChunk(chunk);
        }
        
        // Apply propert to the context conditionally
        virtual void drawSelf(IGraphics& ctx)
        {
            ;
        }

        void draw(IGraphics& ctx) override
        {
            if (isSet())
                drawSelf(ctx);
        }

    };
}

namespace svg {
    // simple type parsing
    static INLINE int64_t toInteger(const DataChunk& inChunk);
    static INLINE float toNumber(const DataChunk& inChunk);
    static INLINE std::string toString(const DataChunk& inChunk);
	

    
	// Utility, for viewbox, points, etc
    static INLINE double nextNumber(DataChunk& inChunk, const charset& delims);
}

/// Some utility routines
namespace svg {

    // return a number next in a list of numbers
    static INLINE double nextNumber(DataChunk& inChunk, const charset& delims)
    {
        // First, trim the front of whitespace
        inChunk = chunk_ltrim(inChunk, wspChars);
        
        // now go for the next number separated by delimiters
        DataChunk numChunk;
        numChunk = chunk_token(inChunk, delims);
        double anum = chunk_to_double(numChunk);
        
        return anum;
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
	    
        SVGDimension(const SVGDimension &other)
            :fValue(other.fValue)
            ,fUnits(other.fUnits)
            ,fDeviceValue(other.fDeviceValue)
        {}

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
        
        void loadSelfFromChunk(const DataChunk &inChunk)
		{
            DataChunk s = inChunk;
            DataChunk numChunk;
            auto nextPart = scanNumber(s, numChunk);
            float value = (float)chunk_to_double(numChunk);
            SVGDimensionUnits units = parseDimensionUnits(nextPart);
            
            reset(value, units);
		}
    };


    
    static INLINE SVGDimension toDimension(const DataChunk& inChunk)
    {
        SVGDimension dim{};
        dim.loadSelfFromChunk(inChunk);

        return dim;
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

            point.fX = (float)nextNumber(s, numDelims);
            point.fY = (float)nextNumber(s, numDelims);

            return point;
        }


    };
}


namespace svg {
    struct SVGOpacity : public SVGVisualProperty
    {
        double fValue{ 1.0 };

		SVGOpacity(IMapSVGNodes* iMap):SVGVisualProperty(iMap){}

        virtual const BLVar& getVariant()
        {
            fVar = fValue;
            return fVar;
        }
        
        void drawSelf(IGraphics& ctx)
        {
			SVGVisualProperty::drawSelf(ctx);
            ctx.fillOpacity(fValue);    // BUGBUG - fill or stroke?  Need to decide
                                        // perhaps a lambda gets attached??
        }

		void loadSelfFromChunk(const DataChunk& inChunk)
		{
            fValue = toDimension(inChunk).calculatePixels(1);
            set(true);
		}

        static std::shared_ptr<SVGOpacity> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
            std::shared_ptr<SVGOpacity> sw = std::make_shared<SVGOpacity>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                sw->loadFromChunk(inChunk);

            return sw;
        }

        static std::shared_ptr<SVGOpacity> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            return createFromChunk(root, name, elem.getAttribute(name));
        }
	};

}

//==================================================================
//  SVG Text Properties
//================================================================= =
namespace svg {
    struct SVGFontSize : public SVGVisualProperty
    {
        double fValue{ 12.0 };

		SVGFontSize(IMapSVGNodes* inMap) : SVGVisualProperty(inMap) {}
        SVGFontSize(const SVGFontSize& other) :SVGVisualProperty(other) { fValue = other.fValue; }
        
        SVGFontSize& operator=(const SVGFontSize& rhs)
        {
            SVGVisualProperty::operator=(rhs);
            fValue = rhs.fValue;
            return *this;
        }

        void drawSelf(IGraphics& ctx)
        {
            ctx.textSize(fValue);
        }

        void loadSelfFromChunk(const DataChunk& inChunk)
        {
            fValue = toDimension(inChunk).calculatePixels(96);
            set(true);
        }

        static std::shared_ptr<SVGFontSize> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
            std::shared_ptr<SVGFontSize> sw = std::make_shared<SVGFontSize>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                sw->loadFromChunk(inChunk);

            return sw;
        }

        static std::shared_ptr<SVGFontSize> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            return createFromChunk(root, name, elem.getAttribute(name));
        }
    };

	// attribute name="font-family" type="string" default="sans-serif"
	// attribute name="font-style" type="string" default="normal"
    struct SVGFontFamily : public SVGVisualProperty
    {
        std::string fValue{ "Arial" };

		SVGFontFamily(IMapSVGNodes* inMap) : SVGVisualProperty(inMap) {}
		SVGFontFamily(const SVGFontFamily& other) :SVGVisualProperty(other) { fValue = other.fValue; }
        
        SVGFontFamily& operator=(const SVGFontFamily& rhs)
        {
			SVGVisualProperty::operator=(rhs);
			fValue = rhs.fValue;
			return *this;
        }

		void drawSelf(IGraphics& ctx) override
		{
			ctx.textFont(fValue.c_str());
		}
        
        void loadSelfFromChunk(const DataChunk& inChunk)
        {
            if (!inChunk)
                return;
            
			fValue = toString(chunk_trim(inChunk, wspChars));
			set(true);
        }

        static std::shared_ptr<SVGFontFamily> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
            std::shared_ptr<SVGFontFamily> sw = std::make_shared<SVGFontFamily>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                sw->loadFromChunk(inChunk);

            return sw;
        }

        static std::shared_ptr<SVGFontFamily> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            return createFromChunk(root, name, elem.getAttribute(name));
        }
        
    };
    

    struct SVGTextAnchor : public SVGVisualProperty
    {
        ALIGNMENT fValue{ ALIGNMENT::LEFT };

		SVGTextAnchor(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
        SVGTextAnchor(const SVGTextAnchor& other) :SVGVisualProperty(other) { fValue = other.fValue; }
        
        SVGTextAnchor& operator=(const SVGTextAnchor& rhs)
        {
            SVGVisualProperty::operator=(rhs);
            fValue = rhs.fValue;
            return *this;
        }

        void drawSelf(IGraphics& ctx)
        {
            ctx.textAlign(fValue, ALIGNMENT::BASELINE);
        }

        void loadSelfFromChunk(const DataChunk& inChunk)
        {
            if (inChunk == "start")
                fValue = ALIGNMENT::LEFT;
			else if (inChunk == "middle")
				fValue = ALIGNMENT::CENTER;
			else if (inChunk == "end")
				fValue = ALIGNMENT::RIGHT;


            set(true);
        }

        static std::shared_ptr<SVGTextAnchor> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
            std::shared_ptr<SVGTextAnchor> sw = std::make_shared<SVGTextAnchor>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                sw->loadFromChunk(inChunk);

            return sw;
        }

        static std::shared_ptr<SVGTextAnchor> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            return createFromChunk(root, name, elem.getAttribute(name));
        }
    };
    
    struct SVGTextAlign : public SVGVisualProperty
    {
        ALIGNMENT fValue{ ALIGNMENT::LEFT };

        //SVGTextAlign() : SVGVisualProperty() {}
		SVGTextAlign(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
        SVGTextAlign(const SVGTextAlign& other) :SVGVisualProperty(other) { fValue = other.fValue; }
        
        SVGTextAlign& operator=(const SVGTextAlign& rhs)
        {
            SVGVisualProperty::operator=(rhs);
            fValue = rhs.fValue;
            return *this;
        }

        void drawSelf(IGraphics& ctx)
        {
            ctx.textAlign(fValue, ALIGNMENT::BASELINE);
        }

        void loadSelfFromChunk(const DataChunk& inChunk)
        {
            if (inChunk == "center")
				fValue = ALIGNMENT::CENTER;
            
            set(true);
        }

        static std::shared_ptr<SVGTextAlign> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
            std::shared_ptr<SVGTextAlign> sw = std::make_shared<SVGTextAlign>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                sw->loadFromChunk(inChunk);

            return sw;
        }

        static std::shared_ptr<SVGTextAlign> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            return createFromChunk(root, name, elem.getAttribute(name));
        }
    };
}

//=================================================================
// Style attribute parsing
//=================================================================
namespace svg {
    // Turn the style element into attributes of an XmlElement, 
    // then, the caller can use that to more easily parse whatever they're
    // looking for.
    void parseStyleAttribute(const DataChunk & inChunk, XmlElement &styleElement)
    {
        DataChunk styleChunk = inChunk;
        
        if (styleChunk) {
            // use CSSInlineIterator to iterate through the key value pairs
            // creating a visual attribute, using the gSVGPropertyCreation map
            CSSInlineStyleIterator iter(styleChunk);

            while (iter.next())
            {
                std::string name = std::string((*iter).first.fStart, (*iter).first.fEnd);
                if (!name.empty() && (*iter).second)
                {
                    styleElement.addAttribute(name, (*iter).second);
                }
            }

        }

        return;
    }
}

//=================================================================
// SVGImage
// BUGBUG - this should be a compound node as </image> is possible
// even if superfluous
//=================================================================
namespace svg {
    void saveChunk(const uint8_t *data, size_t sz, const char* filename)
    {
        FILE *f{};
        fopen_s(&f, filename, "wb");
        fwrite(data, 1, sz, f);
        fclose(f);
    }

    //
    // Turn a base64 encoded inlined image into a BLImage
    // We are handed the attribute, typically coming from a 
    // href of an <image> tage, or as a lookup for a 
    // fill, or stroke pain attribute.
    //
    bool parseImage(const DataChunk& inChunk, BLImage& img)
    {
        bool success{ false };
        DataChunk value = inChunk;

        // figure out what kind of encoding we're dealing with
        // value starts with: 'data:image/png;base64,<base64 encoded image>
        //
        DataChunk data = chunk_token(value, ":");
        auto mime = chunk_token(value, ";");
        auto encoding = chunk_token(value, ",");


        //DataChunk inChunk = value;
        if (encoding == "base64")
        {
            // allocate some memory to cleanup the buffer
            uint8_t* inBuff{ new uint8_t[chunk_size(value)]{} };
            size_t inCharCount = 0;
            DataChunk valueChunk = value;
            while(valueChunk)
            {
                if (!wspChars.contains(*valueChunk))
                {
                    inBuff[inCharCount] = *valueChunk;
                    inCharCount++;
                }
                valueChunk++;
            }
            DataChunk inBuffChunk = chunk_from_data_size(inBuff, inCharCount);
            
            size_t outBuffSize = BASE64_DECODE_OUT_SIZE(chunk_size(inBuffChunk));
            
            // allocate some memory to decode into
            uint8_t* outBuff{ new uint8_t[outBuffSize]{} };
            DataChunk outChunk = chunk_from_data_size(outBuff, outBuffSize);
            
            if ((mime == "image/gif"))
            {
                auto decodedSize = base64_decode((const char *)inBuffChunk.fStart, chunk_size(inBuffChunk), outBuff);

                //saveChunk(outBuff, decodedSize, "chunk.gif");

                if (decodedSize)
                {
                    bool success{false};
                    success = parseGIF(outBuff, decodedSize, img);

                    //printf("parseImage(GIF), readFromData: %d  %dX%d\n", success, img.size().w, img.size().h);
                }
            }
            else if ((mime == "image/png") || (mime == "image/jpeg"))
            {   
                auto decodedSize = base64_decode((const char*)inBuffChunk.fStart, chunk_size(inBuffChunk), outBuff);

                if (decodedSize)
                {
                    BLResult res = img.readFromData(outBuff, decodedSize);
                    success = (res == BL_SUCCESS);
                    //printf("parseImage, readFromData: %d  %dX%d\n", res, img.size().w, img.size().h);
                }
            }
            
            delete[] outBuff;
			delete[] inBuff;
        }

        return success;
    }
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

        // So, now nums contains the individual numeric values, separated by ',' or '[SPC]'
        // The individual numeric values are either
        // 50%
        // 50

        int i = 0;
        uint8_t rgbi[4]{};

        // Get the first token, which is red
        // if it's not there, then return gray
        auto num = chunk_token(nums, ", ");
        if (chunk_size(num) < 1)
            return rgba(128, 128, 128, 255);

        while (num)
        {
            auto cv = toDimension(num);

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
                if (i==3) // last number of rgba is a decimal indicating opacity
                    rgbi[i] = (uint8_t)(cv.value()  * 255.0f);
                else 
                    rgbi[i] = (uint8_t)cv.value();
            }
            i++;
            num = chunk_token(nums, ", ");
        }

        if (i == 4)
            return rgba(rgbi[0], rgbi[1], rgbi[2], rgbi[3]);

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
            return rgba(128, 128, 128, 255);

        return svg::colors[cName];
    }



    constexpr int SVG_PaintForFill = 1;
    constexpr int SVG_PaintForStroke = 2;

    struct SVGPaint : public SVGVisualProperty
    {
        BLVar fPaint{};
        bool fExplicitNone{ false };
        int fPaintFor{ SVG_PaintForFill };

		SVGPaint(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
        SVGPaint(const SVGPaint& other) :SVGVisualProperty(other) 
        {

        }

        SVGPaint& operator=(const SVGPaint& rhs)
        {
            SVGVisualProperty::operator=(rhs);

            blVarAssignWeak(&fPaint, &rhs.fPaint);
            fExplicitNone = rhs.fExplicitNone;
            fPaintFor = rhs.fPaintFor;

            return *this;
        }

        const BLVar& getVariant() override
        {
            return fPaint;
        }
        
        void setPaintFor(int pfor) { fPaintFor = pfor; }
        void setOpacity(float opacity)
        {
            uint32_t outValue;
            if (BL_SUCCESS == blVarToRgba32(&fPaint, &outValue))
            {
                BLRgba32 newColor(outValue);
                newColor.setA(opacity * 255);
                blVarAssignRgba32(&fPaint, newColor.value);
            }
        }

        // Need to distinguish which function gets called
        // BUGBUG
        void drawSelf(IGraphics& ctx)
        {
            switch (fPaintFor)
            {

            case SVG_PaintForFill:
                if (fExplicitNone)
                    ctx.noFill();
                else
                    ctx.fill(fPaint);
                break;

            case SVG_PaintForStroke:
                if (fExplicitNone)
                    ctx.noStroke();
                else
                    ctx.stroke(fPaint);
                break;
            }

        }

        void loadFromUrl(const DataChunk &inChunk)
        {
            DataChunk str = inChunk;
            
            // the id we want should look like this
            // url(#id)
            // so we need to skip past the 'url(#'
            // and then find the closing ')'
            // and then we have the id
            auto url = chunk_token(str, "(");
            auto id = chunk_trim(chunk_token(str, ")"), wspChars);

            // The first character could be '.' or '#'
            // so we need to skip past that
            if (*id == '.' || *id == '#')
                id++;

            if (!id)
                return;

            // lookup the thing we're referencing
            std::string idStr = toString(id);

            if (fRoot != nullptr)
            {
                auto node = fRoot->findNodeById(idStr);
                
                // pull out the color value
                if (node != nullptr)
                {
                    const BLVar& aVar = node->getVariant();

                    auto res = blVarAssignWeak(&fPaint, &aVar);
                    if (res == BL_SUCCESS)
                        set(true);
                }
            }
        }
        
        void loadSelfFromChunk (const DataChunk& inChunk)
        {
            maths::vec4b c{};
            
            blVarAssignRgba32(&fPaint, c.value);

            DataChunk str = inChunk;
            DataChunk rgbStr = chunk_from_cstr("rgb(");
            DataChunk rgbaStr = chunk_from_cstr("rgba(");

            size_t len = 0;

            len = chunk_size(str);
            if (len >= 1 && *str == '#')
            {
                c = parseColorHex(str);
                blVarAssignRgba32(&fPaint, c.value);
                set(true);
            }
            else if (chunk_starts_with(str, rgbStr) || chunk_starts_with(str, rgbaStr))
            {
                c = parseColorRGB(str);
                blVarAssignRgba32(&fPaint, c.value);
                set(true);
            }
            else if (chunk_starts_with_cstr(str, "url(")) 
            {
                loadFromUrl(str);
            }
            else {
                std::string cName = std::string(str.fStart, str.fEnd);
                if (cName == "none") {
                    fExplicitNone = true;
                    set(true);
                }
                else if (svg::colors.contains(cName))
                {
                    c = svg::colors[cName];
                    blVarAssignRgba32(&fPaint, c.value);
                    set(true);
                }
                else {
                    // user wants some sort of color, which is either invalid name
                    // or a color function we don't support yet
                    // so set a default gray color
                    c = rgb(128, 128, 128);
                    blVarAssignRgba32(&fPaint, c.value);
                    set(true);
                }
            }


        }

        static std::shared_ptr<SVGPaint> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
            std::shared_ptr<SVGPaint> paint = std::make_shared<SVGPaint>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                paint->loadFromChunk(inChunk);

            return paint;
        }

        static std::shared_ptr<SVGPaint> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            auto paint = createFromChunk(root, name, elem.getAttribute(name));

			if (!paint->isSet())
				return paint;
            
            if (name == "fill")
            {
                paint->setPaintFor(SVG_PaintForFill);
                // look for fill-opacity as well
                auto o = elem.getAttribute("fill-opacity");
                if (o)
                {
                    auto onum = toNumber(o);
                    paint->setOpacity(onum);
                }
            }
            else if (name == "stroke")
            {
                paint->setPaintFor(SVG_PaintForStroke);
                // look for stroke-opacity as well
                auto o = elem.getAttribute("stroke-opacity");
                if (o)
                {
                    auto onum = toNumber(o);
                    paint->setOpacity(onum);
                }
            }
			else if (name == "stop-color")
			{
				// look for stop-opacity as well
				auto o = elem.getAttribute("stop-opacity");
				if (o)
				{
					auto onum = toNumber(o);
					paint->setOpacity(onum);
				}
			}


            return paint;
        }
    };
}

//enum class FILLRULE : unsigned
//{
//    NON_ZERO = 0,
//    EVEN_ODD = 1,
//};
namespace svg {
    struct SVGFillRule : public SVGVisualProperty
    {
		unsigned int fValue{ SVG_FILLRULE_NONZERO };

   

        //SVGFillRule() : SVGVisualProperty() {}
		SVGFillRule(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
        SVGFillRule(const SVGFillRule& other) :SVGVisualProperty(other)
        {
            fValue = other.fValue;
        }

        SVGFillRule& operator=(const SVGFillRule& rhs)
        {
            SVGVisualProperty::operator=(rhs);
            fValue = rhs.fValue;
            return *this;
        }

        void drawSelf(IGraphics& ctx)
        {
            ctx.fillRule(fValue);
        }

        void loadSelfFromChunk(const DataChunk& inChunk)
        {
            DataChunk s = chunk_trim(inChunk, wspChars);

            set(true);

            if (s == "nonzero")
                fValue = SVG_FILLRULE_NONZERO;
            else if (s == "evenodd")
                fValue = SVG_FILLRULE_EVENODD;
            else
                set(false);
        }

        static std::shared_ptr<SVGFillRule> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
            std::shared_ptr<SVGFillRule> node = std::make_shared<SVGFillRule>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                node->loadFromChunk(inChunk);

            return node;
        }

        static std::shared_ptr<SVGFillRule> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            return createFromChunk(root, name, elem.getAttribute(name));
        }
    };
}

namespace svg {
    struct SVGStrokeWidth : public SVGVisualProperty
    {
		double fWidth{ 1.0};

		//SVGStrokeWidth() : SVGVisualProperty() {}
		SVGStrokeWidth(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
		SVGStrokeWidth(const SVGStrokeWidth& other) :SVGVisualProperty(other) { fWidth = other.fWidth; }
        
		SVGStrokeWidth& operator=(const SVGStrokeWidth& rhs)
		{
			SVGVisualProperty::operator=(rhs);
			fWidth = rhs.fWidth;
			return *this;
		}

		void drawSelf(IGraphics& ctx)
		{
			ctx.strokeWeight(fWidth);
		}

		void loadSelfFromChunk(const DataChunk& inChunk)
		{
			fWidth = toNumber(inChunk);
			set(true);
		}

		static std::shared_ptr<SVGStrokeWidth> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
		{
			std::shared_ptr<SVGStrokeWidth> sw = std::make_shared<SVGStrokeWidth>(root);

			// If the chunk is empty, return immediately 
			if (inChunk)
				sw->loadFromChunk(inChunk);

			return sw;
		}

		static std::shared_ptr<SVGStrokeWidth> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
		{
			return createFromChunk(root, name, elem.getAttribute(name));
		}
    };
    
    /// <summary>
    ///  SVGStrokeMiterLimit
	/// A visual property to set the miter limit for a stroke
    /// </summary>
    struct SVGStrokeMiterLimit : public SVGVisualProperty
    {
		double fMiterLimit{ 4.0 };
        
        //SVGStrokeMiterLimit() : SVGVisualProperty() {}
		SVGStrokeMiterLimit(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
		SVGStrokeMiterLimit(const SVGStrokeMiterLimit& other) :SVGVisualProperty(other) { fMiterLimit = other.fMiterLimit; }
        
        SVGStrokeMiterLimit& operator=(const SVGStrokeMiterLimit& rhs)
		{
			SVGVisualProperty::operator=(rhs);
			fMiterLimit = rhs.fMiterLimit;
			return *this;
        }

		void drawSelf(IGraphics& ctx)
		{
			ctx.strokeMiterLimit(fMiterLimit);
		}
        
		void loadSelfFromChunk(const DataChunk& inChunk)
		{
			fMiterLimit = toNumber(inChunk);
			fMiterLimit = maths::clamp((float)fMiterLimit, 1.0f, 10.0f);
            
			set(true);
		}

		static std::shared_ptr<SVGStrokeMiterLimit> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
		{
			std::shared_ptr<SVGStrokeMiterLimit> sw = std::make_shared<SVGStrokeMiterLimit>(root);

			// If the chunk is empty, return immediately 
			if (inChunk)
				sw->loadFromChunk(inChunk);

			return sw;
		}

        // stroke-miterlimit
		static std::shared_ptr<SVGStrokeMiterLimit> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
		{
			return createFromChunk(root, name, elem.getAttribute(name));
		}
	
    };
    
    struct SVGStrokeLineCap : public SVGVisualProperty
    {
        SVGlineCap fLineCap{ SVG_CAP_BUTT };

		//SVGStrokeLineCap() : SVGVisualProperty() {}
		SVGStrokeLineCap(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
		SVGStrokeLineCap(const SVGStrokeLineCap& other) :SVGVisualProperty(other)
		{
			fLineCap = other.fLineCap;
		}

		SVGStrokeLineCap& operator=(const SVGStrokeLineCap& rhs)
		{
			SVGVisualProperty::operator=(rhs);
			fLineCap = rhs.fLineCap;
			return *this;
		}

		void drawSelf(IGraphics& ctx)
		{
			ctx.strokeCaps(fLineCap);
		}

		void loadSelfFromChunk(const DataChunk& inChunk)
		{
            DataChunk s = chunk_trim(inChunk, wspChars);

            set(true);
            
			if (s == "butt")
				fLineCap = SVG_CAP_BUTT;
			else if (s == "round")
				fLineCap = SVG_CAP_ROUND;
			else if (s == "square")
				fLineCap = SVG_CAP_SQUARE;
            else
			    set(false);
		}

		static std::shared_ptr<SVGStrokeLineCap> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
		{
			std::shared_ptr<SVGStrokeLineCap> stroke = std::make_shared<SVGStrokeLineCap>(root);

			// If the chunk is empty, return immediately 
			if (inChunk)
				stroke->loadFromChunk(inChunk);

			return stroke;
		}

		static std::shared_ptr<SVGStrokeLineCap> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem )
		{
			return createFromChunk(root, name, elem.getAttribute(name));
		}
	};

    // SVGStrokeLineJoin
	// A visual property to set the line join for a stroke
    struct SVGStrokeLineJoin : public SVGVisualProperty
    {
        SVGlineJoin fLineJoin{ SVG_JOIN_MITER_BEVEL };

		//SVGStrokeLineJoin() : SVGVisualProperty() {}
		SVGStrokeLineJoin(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
		SVGStrokeLineJoin(const SVGStrokeLineJoin& other) :SVGVisualProperty(other), fLineJoin(other.fLineJoin) {}  
        
        SVGStrokeLineJoin& operator=(const SVGStrokeLineJoin& rhs)
        {
			SVGVisualProperty::operator=(rhs);
			fLineJoin = rhs.fLineJoin;
			return *this;
        }
        
        void drawSelf(IGraphics& ctx)
        {
			ctx.strokeJoin(fLineJoin);
        }
        
        void loadSelfFromChunk(const DataChunk& inChunk)
        {
            DataChunk s = chunk_trim(inChunk, wspChars);

            set(true);
            
			if (s == "miter")
				fLineJoin = SVG_JOIN_MITER_BEVEL;
			else if (s == "round")
				fLineJoin = SVG_JOIN_ROUND;
			else if (s == "bevel")
				fLineJoin = SVG_JOIN_BEVEL;
            //else if (s == "arcs")
			//	fLineJoin = SVG_JOIN_ARCS;
			else if (s == "miter-clip")
				fLineJoin = SVG_JOIN_MITER_CLIP;
			else
				set(false);

        }
        
		static std::shared_ptr<SVGStrokeLineJoin> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
		{
			std::shared_ptr<SVGStrokeLineJoin> stroke = std::make_shared<SVGStrokeLineJoin>(root);

			// If the chunk is empty, return immediately
			if (inChunk)
				stroke->loadFromChunk(inChunk);

			return stroke;
		}

        // stroke-linejoin
        static std::shared_ptr<SVGStrokeLineJoin> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
			return createFromChunk(root, name, elem.getAttribute(name));
        }
        
    };
    
}
namespace svg {
    
    //======================================================
    // SVGViewbox
    // A document may or may not have this property
    //======================================================
    
    struct SVGViewbox : public SVGVisualProperty
    {
        maths::rectf fRect{};

        SVGViewbox() :SVGVisualProperty(nullptr) {}
		SVGViewbox(IMapSVGNodes* iMap):SVGVisualProperty(iMap){}
        SVGViewbox(const SVGViewbox& other)
            : SVGVisualProperty(other)
            ,fRect(other.fRect)
        {

        }

        SVGViewbox& operator=(const SVGViewbox& rhs)
        {
            SVGVisualProperty::operator=(rhs);
            fRect = rhs.fRect;

            return *this;
        }

        float x() const { return fRect.x; }
        float y() const { return fRect.y; }
        float width() const { return fRect.w; }
        float height() const {return fRect.h;}
        
        void drawSelf(IGraphics& ctx)
        {
            //ctx.scale(fWidth / fViewbox.width(), fHeight / fViewbox.height());
            ctx.translate(-x(), -y());
        }
        
        void loadSelfFromChunk(const DataChunk& inChunk)
        {
			DataChunk s = inChunk;
			DataChunk numChunk;
            charset numDelims = wspChars+',';
                
            fRect.x = (float)nextNumber(s, numDelims);
			fRect.y = (float)nextNumber(s, numDelims);
			fRect.w = (float)nextNumber(s, numDelims);
			fRect.h = (float)nextNumber(s, numDelims);

            set(true);  // Yes, we will draw
        }

        static SVGViewbox createFromChunk(IMapSVGNodes* root, const DataChunk& inChunk)
        {
            SVGViewbox vbox{};

            // If the chunk is empty, return immediately 
            if (!inChunk)
                return vbox;

            vbox.loadFromChunk(inChunk);

            
            return vbox;
        }

        // "viewBox"
        static SVGViewbox createFromXml(IMapSVGNodes* root, const XmlElement& elem, const std::string &name)
        {
            return createFromChunk(root, elem.getAttribute(name));
        }
    };
    


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

//================================================
// SVGTransform
// Transformation matrix
//================================================

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

    static DataChunk parseMatrix(DataChunk& inMatrix, BLMatrix2D &m)
    {
        DataChunk s = inMatrix;
        m.reset();  // start with identity

        
        double t[6];    // storage for our 6 numbers
		int na = 0;     // Number of arguments parsed

        s = parseTransformArgs(s, t, 6, na);

        if (na != 6)
            return s;

		m.reset(t[0], t[1], t[2], t[3], t[4], t[5]);

        return s;
    }
    
    
    static DataChunk parseTranslate(const DataChunk& inChunk, BLMatrix2D& xform)
    {
        double args[2];
        int na = 0;
        DataChunk s = inChunk;
        s = parseTransformArgs(s, args, 2, na);
        if (na == 1)
            args[1] = 0.0;

        xform.translate(args[0], args[1]);

        return s;
    }
    
    static DataChunk parseScale(const DataChunk& inChunk, BLMatrix2D& xform)
    {
        double args[2]{ 0 };
        int na = 0;
        DataChunk s = inChunk;
        
        s = parseTransformArgs(s, args, 2, na);

        if (na == 1)
            args[1] = args[0];

        xform.scale(args[0], args[1]);

        return s;
    }
    
    
    static DataChunk parseSkewX(const DataChunk& inChunk, BLMatrix2D& xform)
    {
        double args[1];
        int na = 0;
        DataChunk s = inChunk;
        s = parseTransformArgs(s, args, 1, na);

        xform.resetToSkewing(maths::radians(args[0]), 0.0f);

        return s;
    }

    static DataChunk parseSkewY(const DataChunk& inChunk, BLMatrix2D& xform)
    {
        double args[1]{ 0 };
        int na = 0;
        DataChunk s = inChunk;
        s = parseTransformArgs(s, args, 1, na);

        xform.resetToSkewing(0.0f, maths::radians(args[0]));

        return s;
    }
    
    
    static DataChunk parseRotate(const DataChunk& inChunk, BLMatrix2D& xform)
    {
        double args[3]{ 0 };
        int na = 0;
        float m[6]{ 0 };
        float t[6]{ 0 };
        DataChunk s = inChunk;

		s = parseTransformArgs(s, args, 3, na);

        if (na == 1)
            args[1] = args[2] = 0.0f;

		xform.rotate(maths::radians(args[0]), args[1], args[2]);

        return  s;
    }


    struct SVGTransform : public SVGVisualProperty
    {
        BLMatrix2D fTransform{};

        //SVGTransform() : SVGVisualProperty() {}
		SVGTransform(IMapSVGNodes* iMap) : SVGVisualProperty(iMap) {}
        SVGTransform(const SVGTransform& other)
            :SVGVisualProperty(other)
            ,fTransform(other.fTransform)
        {

        }

        SVGTransform& operator=(const SVGTransform& rhs)
        {
            SVGVisualProperty::operator=(rhs);
            fTransform = rhs.fTransform;

            return *this;
        }

        BLMatrix2D& getTransform() { return fTransform; }

		void loadSelfFromChunk(const DataChunk& inChunk) override
		{
            DataChunk s = inChunk;
            fTransform.reset();     // set to identity initially
            

            while (s)
            {
				s = chunk_skip_wsp(s);
                
                // Set out temp transform to the identity to start
                // so that if parsing goes wrong, we can still do
                // the multiply without worrying about messing things up
                // That means, the individula parsing functions need to not
                // partially mess up the transform if they fail.
                //svg_xformIdentity(t);

                BLMatrix2D tm{};
				tm.reset();

                if (chunk_starts_with_cstr(s, "matrix"))
                {
                    s = parseMatrix(s, tm);
                    fTransform = tm;
                    set(true);
                }
                else if (chunk_starts_with_cstr(s, "translate"))
                {
                    s = parseTranslate(s, tm);
                    fTransform.transform(tm);
                    set(true);
                }
                else if (chunk_starts_with_cstr(s, "scale"))
                {
                    s = parseScale(s, tm);
					fTransform.transform(tm);
                    set(true);
                }
                else if (chunk_starts_with_cstr(s, "rotate"))
                {
                    s = parseRotate(s, tm);
                    fTransform.transform(tm);
                    set(true);
                }
                else if (chunk_starts_with_cstr(s, "skewX"))
                {
                    s = parseSkewX(s,tm);
                    fTransform.transform(tm);
                    set(true);
                }
                else if (chunk_starts_with_cstr(s, "skewY"))
                {
                    s = parseSkewY(s,tm);
                    fTransform.transform(tm);
                    set(true);
                }
                else {
                    s++;
                }

            }

		}

        void drawSelf(IGraphics& ctx) override
        {
			ctx.transform(fTransform.m);
        }

        static std::shared_ptr<SVGTransform> createFromChunk(IMapSVGNodes* root, const std::string& name, const DataChunk& inChunk)
        {
			std::shared_ptr<SVGTransform> tform = std::make_shared<SVGTransform>(root);

            // If the chunk is empty, return immediately 
            if (inChunk)
                tform->loadFromChunk(inChunk);

            return tform;
        }

        // "transform"
        static std::shared_ptr<SVGTransform> createFromXml(IMapSVGNodes* root, const std::string& name, const XmlElement& elem)
        {
            return createFromChunk(root, name, elem.getAttribute(name));
        }
    };
    

}