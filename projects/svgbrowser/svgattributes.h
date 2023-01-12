#pragma once

#include <map>
#include <string>

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
}

