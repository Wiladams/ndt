#pragma once


#include "blend2d.h"

#include "maths.hpp"
#include "shaper.h"
#include "geometry.h"

#include "chunkutil.h"


#include "mmap.hpp"
#include "xmlscan.h"

#include "svgshapes.h"


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
	using namespace ndt_debug;
    
    static charset digitChars("0123456789");
    static charset numberChars("0123456789.-+eE");


    constexpr auto SVG_MAX_ATTR = 128;
    constexpr auto SVG_MAX_DASHES = 8;

    // Turn a chunk into a vector of chunks, splitting on the delimiters
    // BUGBUG - should consider the option of empty chunks, especially at the boundaries
    static INLINE std::vector<DataChunk> chunk_split(const DataChunk& inChunk, const charset& delims, bool wantEmpties = false) noexcept
    {
        std::vector<DataChunk> result;
        DataChunk s = inChunk;
        while (s)
        {
            DataChunk token = chunk_token(s, delims);
            //if (size(token) > 0)
            result.push_back(token);
        }

        return result;
    }

}



namespace svg {


    // A single structure to represent all possible kinds of attributes
    // for an SVG element
    struct SVGAttribute
    {
        std::string fName;                              // Name of the attribute
        int fKind;                                      // The kind of data

        union {
            DataChunk       fChunk;
            SVGDimension   fCoordinate;
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

            case SVG_ATTR_KIND_DIMENSION:          // value/units
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
    
    //static SVGAttribute toEnum(const ndt::DataChunk& s);
    //static SVGAttribute toTransform(const ndt::DataChunk& s);
    //static SVGAttribute toDefault(const ndt::DataChunk& s);
    //static SVGAttribute toOpacity(const ndt::DataChunk& s);
    //static SVGAttribute toStyle(const ndt::DataChunk& s);


    

    
    /*
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
        ,{"clipPathUnits", toDefault}
        ,{"color-profile", toDefault}
        ,{"color-rendering", toDefault}
        ,{"contentScriptType", toDefault}
        ,{"contentStyleType", toDefault}
        ,{"cx", toCoordinate}
        ,{"cy", toCoordinate}
        ,{"d", toPath}
        ,{"decelerate", toDefault}
        ,{"descent", toNumber}
        ,{"diffuseConstant", toDefault}
        ,{"direction", toDefault}

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

        ,{"filterRes", toDefault}
        ,{"filterUnits", toDefault}
        ,{"flood-color", toDefault}
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
        ,{"lighting-color", toDefault }
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
    */
/*
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

    
    //
    // Transform parsing
    //
    SVGAttribute toTransform(const ndt::DataChunk& s)
    {
        return toDefault(s);
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
        //if (gParseAttribute.contains(name)) {
        //    return gParseAttribute[name](inChunk);
        //}

        return SVGAttribute();
    }
    */
}



namespace svg {
    struct SVGDocument : public SVGGroup
    {
        bool fInDefinitions{ false };
        std::shared_ptr<mmap> fFileMap{};
        
        SVGDocument(std::string filename)
			: SVGGroup(nullptr)
		{
            fFileMap = ndt::mmap::create_shared(filename);
            if (fFileMap == nullptr)
                return ;
            
            setRoot(this);

			// BUGBUG - Maybe we should not call the load
            // within the constructor
            //load();

        }
        
        bool load()
        {
			if (fFileMap == nullptr)
				return false;
            
			DataChunk s = fFileMap->getChunk();
			s = chunk_trim(s, wspChars);

			XmlElementIterator iter(s);

			loadFromIterator(iter);

			return true;
        }
        
        // Load the document from an XML Iterator
        // Since this is the top level document, we just want to kick
        // off loading the root node 'svg', and we're done 
        void loadFromIterator(ndt::XmlElementIterator& iter) override
        {

            // skip past any elements that come before the 'svg' element
            while (iter)
            {
				const ndt::XmlElement& elem = *iter;

				if (!elem)
					break;

                printXmlElement(*iter);
                
                // Skip over these node types we don't know how to process
                if (elem.isComment() || elem.isContent() || elem.isProcessingInstruction())
                {
                    iter++;
                    continue;
                }
                
                
				if (elem.isStart() && (elem.name() == "svg"))
				{
					auto node = SVGRootNode::createFromIterator(iter);
					if (node != nullptr)
					{
						addNode(node);
					}
				}

				iter++;
            }
        }
        

		static std::shared_ptr<SVGDocument> createFromFilename(const std::string& filename)
		{
			auto doc = std::make_shared<SVGDocument>(filename);
            doc->load();
            
            return doc;
		}

    };
}
