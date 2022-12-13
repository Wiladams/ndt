#pragma once

#include "maths.hpp"
#include "coloring.h"

#include <map>
#include <vector>

namespace svg
{
    using namespace maths;
    using namespace ndt;


    
    // Various Attributes to be found in an SVG image
    enum SVGpaintType {
        SVG_PAINT_NONE = 0,
        SVG_PAINT_COLOR = 1,
        SVG_PAINT_LINEAR_GRADIENT = 2,
        SVG_PAINT_RADIAL_GRADIENT = 3
    };

    enum SVGspreadType {
        SVG_SPREAD_PAD = 0,
        SVG_SPREAD_REFLECT = 1,
        SVG_SPREAD_REPEAT = 2
    };

    enum SVGlineJoin {
        SVG_JOIN_MITER = 0,
        SVG_JOIN_ROUND = 1,
        SVG_JOIN_BEVEL = 2
    };

    enum SVGlineCap {
        SVG_CAP_BUTT = 0,
        SVG_CAP_ROUND = 1,
        SVG_CAP_SQUARE = 2
    };

    enum SVGfillRule {
        SVG_FILLRULE_NONZERO = 0,
        SVG_FILLRULE_EVENODD = 1
    };

    enum SVGflags {
        SVG_FLAGS_VISIBLE = 0x01
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

    typedef struct SVGpath
    {
        float* pts;					// Cubic bezier points: x0,y0, [cpx1,cpx1,cpx2,cpy2,x1,y1], ...
        int npts;					// Total number of bezier points.
        char closed;				// Flag indicating if shapes should be treated as closed.
        float bounds[4];			// Tight bounding box of the shape [minx,miny,maxx,maxy].
        struct SVGpath* next;		// Pointer to next path, or NULL if last element.
    } SVGpath;

    typedef struct SVGshape
    {
        char id[64];				// Optional 'id' attr of the shape or its group
        SVGpaint fill;				// Fill paint
        SVGpaint stroke;			// Stroke paint
        float opacity;				// Opacity of the shape.
        float strokeWidth;			// Stroke width (scaled).
        float strokeDashOffset;		// Stroke dash offset (scaled).
        float strokeDashArray[8];			// Stroke dash array (scaled).
        char strokeDashCount;				// Number of dash values in dash array.
        char strokeLineJoin;		// Stroke join type.
        char strokeLineCap;			// Stroke cap type.
        float miterLimit;			// Miter limit
        char fillRule;				// Fill rule, see NSVGfillRule.
        unsigned char flags;		// Logical or of NSVG_FLAGS_* flags
        float bounds[4];			// Tight bounding box of the shape [minx,miny,maxx,maxy].
        SVGpath* paths;			    // Linked list of paths in the image.
        struct SVGshape* next;		// Pointer to next shape, or NULL if last element.
    } SVGshape;

    typedef struct SVGimage
    {
        float width;				// Width of the image.
        float height;				// Height of the image.
        SVGshape* shapes;			// Linked list of shapes in the image.
    } SVGimage;
}

namespace svg
{
	using namespace maths;
    
    struct ColorEntry {
        //const char* name = nullptr;
        maths::vec4b value{};
    };

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





