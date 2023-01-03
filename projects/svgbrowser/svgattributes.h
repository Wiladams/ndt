#pragma once


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

